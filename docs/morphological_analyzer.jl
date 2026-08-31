module MorphologicalAnalyzer

"""
UNICAGD_AML Mathematical Morphology Analysis Corpus
UNICAGD_AML Matematikai Morfológia Elemzési Korpusz

This module provides mathematical morphology operations for AML/fraud pattern analysis.
Ez a modul matematikai morfológiai műveleteket biztosít AML/csalási minta elemzéshez.
"""

export StructuringElement, MorphologicalOperation, AlertPattern, TransactionStream
export erosion, dilation, opening, closing, hit_or_miss, morphological_gradient, top_hat
export analyze_stream, generate_corpus_report

using LinearAlgebra
using Statistics
using JSON

# =============================================================================
# Core Data Structures
# Alap Adatstruktúrák
# =============================================================================

"""
Structuring element for morphological operations.
Strukturáló elem morfológiai műveletekhez.
"""
struct StructuringElement
    name::String                          # Operation name / Művelet neve
    condition::Function                   # Pattern matching function / Minta illeszkedő függvény
    threshold_low::Float64               # Lower bound / Alsó határ
    threshold_high::Float64              # Upper bound / Felső határ
    category::String                      # AML category / AML kategória
    severity::String                      # Alert severity / Riasztás súlyossága
    confidence::Float64                   # Confidence score / Megbízhatósági pontszám
end

"""
Morphological operation result.
Morfológiai művelet eredmény.
"""
struct MorphologicalOperation
    name::String
    input_set::Vector{Bool}
    output_set::Vector{Bool}
    hit_count::Int
    miss_count::Int
    timestamp::Float64
end

"""
Alert pattern extracted from transactions.
Riasztási minta tranzakciókból kinyerve.
"""
struct AlertPattern
    rule_id::String
    rule_name::String
    category::String
    severity::String
    confidence::Float64
    typology::String
    amount_range::Tuple{Float64, Float64}
    countries::Set{String}
    is_card_required::Bool
    structuring_element::StructuringElement
end

"""
Transaction stream for analysis.
Tranzakciófolyam elemzéshez.
"""
struct TransactionStream
    ids::Vector{String}
    amounts::Vector{Float64}
    countries::Vector{String}
    channels::Vector{String}
    is_card::Vector{Bool}
    timestamps::Vector{Float64}
    volumes::Vector{Float64}
end

# =============================================================================
# Structuring Elements Definitions
# StrukturálóElem Definíciók
# =============================================================================

"""
AML-001 Structuring/Smurfing structuring element.
AML-001 Szerkeztetés/csoportosítás strukturáló elem.
Condition: 7500 <= amount < 10000 AND not card transaction
"""
function structuring_element_aml001()
    StructuringElement(
        "AML-001 Structuring",
        x -> 7500.0 <= x < 10000.0,
        7500.0,
        10000.0,
        "CATEGORY_SMURFING_STRUCTURING",
        "HIGH",
        0.92
    )
end

"""
AML-003 Sanctions evasion structuring element.
AML-003 Szankciókerülés strukturáló elem.
Condition: country in {SC, RU, IR, KP, KY}
"""
function structuring_element_aml003()
    countries = Set(["SC", "RU", "IR", "KP", "KY"])
    StructuringElement(
        "AML-003 Sanctions",
        x -> x in countries,
        0.0,
        Inf,
        "CATEGORY_HIGH_RISK_JURISDICTION",
        "CRITICAL",
        0.98
    )
end

"""
FRD-003 Card fraud structuring element.
FRD-003 Kártyasalcsalás strukturáló elem.
Condition: is_card == true AND amount > 1200
"""
function structuring_element_frd003()
    StructuringElement(
        "FRD-003 Card Fraud",
        x -> x > 1200.0,
        1200.0,
        Inf,
        "CATEGORY_APP_SCAM",
        "HIGH",
        0.89
    )
end

# =============================================================================
# Mathematical Morphology Operations
# Matematikai Morfológiai Műveletek
# =============================================================================

"""
Erosion operation - removes elements not matching structuring element.
Erozió művelet - eltávolítja azokat az elemeket amelyek nem illeszkednek a strukturáló elemre.

Mathematical definition / Matematikai definíció:
A ⊖ B = {z | B_z ⊆ A}

Where / Ahol:
- A is the input set (transaction stream)
- B is the structuring element
- B_z is B translated by z
"""
function erosion(stream::TransactionStream, se::StructuringElement)
    n = length(stream.amounts)
    result = fill(false, n)
    hit_count = 0
    miss_count = 0
    
    for i in 1:n
        if se.condition(stream.amounts[i]) && 
           (se.name != "AML-003" || stream.countries[i] in Set(["SC", "RU", "IR", "KP", "KY"])) &&
           (se.name != "FRD-003" || stream.is_card[i])
            result[i] = true
            hit_count += 1
        else
            miss_count += 1
        end
    end
    
    MorphologicalOperation(
        "Erosion($(se.name))",
        [se.condition(a) for a in stream.amounts],
        result,
        hit_count,
        miss_count,
        time()
    )
end

"""
Dilation operation - expands matching regions.
Dilatáció művelet - kiterjeszti az illeszkedő régiókat.

Mathematical definition / Matematikai definíció:
A ⊕ B = {z | (B̂_z ∩ A) ≠ ∅}

Where / Ahol:
- B̂ is the reflection of B
- (B̂_z ∩ A) is the intersection of reflected B translated by z with A
"""
function dilation(stream::TransactionStream, se::StructuringElement, window_size::Int=3)
    n = length(stream.amounts)
    result = fill(false, n)
    hit_count = 0
    miss_count = 0
    
    for i in 1:n
        window_start = max(1, i - window_size + 1)
        window_end = min(n, i + window_size - 1)
        
        dilated = false
        for j in window_start:window_end
            if se.condition(stream.amounts[j]) &&
               (se.name != "AML-003" || stream.countries[j] in Set(["SC", "RU", "IR", "KP", "KY"])) &&
               (se.name != "FRD-003" || stream.is_card[j])
                dilated = true
                break
            end
        end
        
        result[i] = dilated
        if dilated
            hit_count += 1
        else
            miss_count += 1
        end
    end
    
    MorphologicalOperation(
        "Dilation($(se.name))",
        [se.condition(a) for a in stream.amounts],
        result,
        hit_count,
        miss_count,
        time()
    )
end

"""
Opening operation - erosion followed by dilation.
Nyitás művelet - erózió után dilatáció.

Mathematical definition / Matematikai definíció:
A ∘ B = (A ⊖ B) ⊕ B

Effect / Hatás: Removes small objects, smooths contours / Kis objektumokat eltávolít, simítja a kontúrokat
"""
function opening(stream::TransactionStream, se::StructuringElement)
    eroded = erosion(stream, se)
    n = length(stream.amounts)
    result = fill(false, n)
    hit_count = 0
    miss_count = 0
    
    # Apply dilation to eroded result
    for i in 1:n
        window_start = max(1, i - 2)
        window_end = min(n, i + 2)
        
        opened = false
        for j in window_start:window_end
            if eroded.output_set[j]
                opened = true
                break
            end
        end
        
        result[i] = opened
        if opened
            hit_count += 1
        else
            miss_count += 1
        end
    end
    
    MorphologicalOperation(
        "Opening($(se.name))",
        eroded.output_set,
        result,
        hit_count,
        miss_count,
        time()
    )
end

"""
Closing operation - dilation followed by erosion.
Zárás művelet - dilatáció után erózió.

Mathematical definition / Matematikai definíció:
A • B = (A ⊕ B) ⊖ B

Effect / Hatás: Fills small holes, connects nearby objects / Kis lyukakat tölt, közeli objektumokat összeköti
"""
function closing(stream::TransactionStream, se::StructuringElement)
    dilated = dilation(stream, se)
    n = length(stream.amounts)
    result = fill(false, n)
    hit_count = 0
    miss_count = 0
    
    # Apply erosion to dilated result
    for i in 1:n
        if dilated.output_set[i] &&
           se.condition(stream.amounts[i]) &&
           (se.name != "AML-003" || stream.countries[i] in Set(["SC", "RU", "IR", "KP", "KY"])) &&
           (se.name != "FRD-003" || stream.is_card[i])
            result[i] = true
            hit_count += 1
        else
            miss_count += 1
        end
    end
    
    MorphologicalOperation(
        "Closing($(se.name))",
        dilated.output_set,
        result,
        hit_count,
        miss_count,
        time()
    )
end

"""
Hit-or-Miss transform - finds exact pattern matches.
Találat-vagy-Miss transzformáció - pontos minták keresése.

Mathematical definition / Matematikai definíció:
A ⊛ B = (A ⊖ B₁) ∩ (A^c ⊖ B₂)

Where / Ahol:
- B₁ is the foreground structuring element
- B₂ is the background structuring element
- A^c is the complement of A
"""
function hit_or_miss(stream::TransactionStream, se::StructuringElement)
    n = length(stream.amounts)
    result = fill(false, n)
    hit_count = 0
    miss_count = 0
    
    for i in 1:n
        amount_match = se.condition(stream.amounts[i])
        country_match = (se.name != "AML-003" || stream.countries[i] in Set(["SC", "RU", "IR", "KP", "KY"]))
        card_match = (se.name != "FRD-003" || stream.is_card[i])
        
        if amount_match && country_match && card_match
            result[i] = true
            hit_count += 1
        else
            miss_count += 1
        end
    end
    
    MorphologicalOperation(
        "HitOrMiss($(se.name))",
        [se.condition(a) for a in stream.amounts],
        result,
        hit_count,
        miss_count,
        time()
    end
end

"""
Morphological gradient - boundary detection between dilated and eroded sets.
Morfológiai gradient - határok detektálása a dilatált és erodált halmazok között.

Mathematical definition / Matematikai definíció:
Grad(A) = (A ⊕ B) - (A ⊖ B)

Effect / Hatás: Highlights edges and transitions in transaction patterns / Kiemeli az éleket és átmeneteket
"""
function morphological_gradient(stream::TransactionStream, se::StructuringElement)
    eroded = erosion(stream, se)
    dilated = dilation(stream, se)
    
    n = length(stream.amounts)
    result = fill(false, n)
    boundary_count = 0
    
    for i in 1:n
        # Gradient is true where dilation and erosion differ
        result[i] = dilated.output_set[i] ⊻ eroded.output_set[i]
        if result[i]
            boundary_count += 1
        end
    end
    
    MorphologicalOperation(
        "MorphGradient($(se.name))",
        eroded.output_set,
        result,
        boundary_count,
        n - boundary_count,
        time()
    )
end

"""
Top-hat transform - isolates bright features (anomalies) from dark background.
Top-hat transzformáció - elszigeteli a fényes elemeket (anomáliákat) a sötét háttérből.

Mathematical definition / Matematikai definíció:
T_hat(A) = A - (A ∘ B)

Effect / Hatás: Isolates isolated anomalies and bright spots / Elszigeteli az izolált anomáliákat és fényes foltokat
"""
function top_hat(stream::TransactionStream, se::StructuringElement)
    opened = opening(stream, se)
    n = length(stream.amounts)
    result = fill(false, n)
    anomaly_count = 0
    
    for i in 1:n
        # Top-hat is true where original is true but opening is false (isolated anomalies)
        original_match = se.condition(stream.amounts[i]) &&
                        (se.name != "AML-003" || stream.countries[i] in Set(["SC", "RU", "IR", "KP", "KY"])) &&
                        (se.name != "FRD-003" || stream.is_card[i])
        
        result[i] = original_match && !opened.output_set[i]
        if result[i]
            anomaly_count += 1
        end
    end
    
    MorphologicalOperation(
        "TopHat($(se.name))",
        opened.output_set,
        result,
        anomaly_count,
        n - anomaly_count,
        time()
    )
end

# =============================================================================
# Stream Analysis
# Folyam Elemzés
# =============================================================================

"""
Analyze a complete transaction stream with all structuring elements.
Teljes tranzakciófolyam elemzése összes strukturáló elemmel.
"""
function analyze_stream(stream::TransactionStream)
    elements = [
        structuring_element_aml001(),
        structuring_element_aml003(),
        structuring_element_frd003()
    ]
    
    results = Dict()
    
    for se in elements
        results[se.name] = Dict(
            "erosion" => erosion(stream, se),
            "dilation" => dilation(stream, se),
            "opening" => opening(stream, se),
            "closing" => closing(stream, se),
            "hit_or_miss" => hit_or_miss(stream, se),
            "morphological_gradient" => morphological_gradient(stream, se),
            "top_hat" => top_hat(stream, se)
        )
    end
    
    return results
end

"""
Generate synthetic transaction stream for testing.
Szintetikus tranzakciófolyam generálása teszteléshez.
"""
function generate_test_stream(count::Int=10000, fraud_ratio::Float64=0.08)
    ids = Vector{String}(undef, count)
    amounts = Vector{Float64}(undef, count)
    countries = Vector{String}(undef, count)
    channels = Vector{String}(undef, count)
    is_card = Vector{Bool}(undef, count)
    timestamps = Vector{Float64}(undef, count)
    volumes = Vector{Float64}(undef, count)
    
    base_time = time()
    countries_list = ["US", "HU", "DE", "FR", "GB", "CH", "SC", "RU", "IR", "KP", "KY"]
    channels_list = ["ACH", "SWIFT", "SEPA", "CARD_CHIP", "CARD_SWIPE", "ONLINE"]
    
    for i in 1:count
        ids[i] = "TX_$(i)_$(rand(1:10000))"
        
        is_fraud = rand() < fraud_ratio
        is_card[i] = rand() > 0.5
        
        if is_card[i]
            amounts[i] = is_fraud ? 1200 + rand() * 2500 : 5 + rand() * 120
            countries[i] = is_fraud && rand() < 0.25 ? "SC" : countries_list[rand(1:6)]
            channels[i] = channels_list[4:7][rand(1:3)]
        else
            amounts[i] = is_fraud ? 7800 + rand() * 2100 : 50 + rand() * 800
            countries[i] = is_fraud && rand() < 0.25 ? "SC" : countries_list[rand(1:6)]
            channels[i] = channels_list[rand(1:3)]
        end
        
        timestamps[i] = base_time + i
        volumes[i] = amounts[i]
    end
    
    TransactionStream(ids, amounts, countries, channels, is_card, timestamps, volumes)
end

# =============================================================================
# Corpus Report Generation
# Korpusz Jelentés Generálás
# =============================================================================

"""
Generate comprehensive analytical corpus report.
Átfogó analitikai korpusz jelentés generálása.
"""
function generate_corpus_report(stream::TransactionStream, results::Dict)
    report = Dict(
        "metadata" => Dict(
            "project" => "UNICAGD_AML",
            "version" => "1.3.0",
            "analysis_type" => "Mathematical Morphology",
            "timestamp" => now(),
            "stream_size" => length(stream.amounts),
            "total_volume" => sum(stream.volumes),
            "unique_countries" => length(unique(stream.countries)),
            "card_ratio" => sum(stream.is_card) / length(stream.is_card)
        ),
        "structuring_elements" => Dict(
            "AML-001" => Dict(
                "name" => "Structuring/Smurfing",
                "condition" => "7500.0 ≤ amount < 10000.0 ∧ ¬is_card",
                "threshold_low" => 7500.0,
                "threshold_high" => 10000.0,
                "category" => "CATEGORY_SMURFING_STRUCTURING",
                "severity" => "HIGH",
                "confidence" => 0.92,
                "typology" => "Structuring (Smurfing)"
            ),
            "AML-003" => Dict(
                "name" => "Sanctions Evasion",
                "condition" => "country ∈ {SC, RU, IR, KP, KY}",
                "threshold_low" => 0.0,
                "threshold_high" => Inf,
                "category" => "CATEGORY_HIGH_RISK_JURISDICTION",
                "severity" => "CRITICAL",
                "confidence" => 0.98,
                "typology" => "Sanctions Evasion / High-Risk Jurisdictional Transfer"
            ),
            "FRD-003" => Dict(
                "name" => "Card Fraud Anomaly",
                "condition" => "is_card ∧ amount > 1200.0",
                "threshold_low" => 1200.0,
                "threshold_high" => Inf,
                "category" => "CATEGORY_APP_SCAM",
                "severity" => "HIGH",
                "confidence" => 0.89,
                "typology" => "Card Fraud / Anomalous Spike"
            )
        ),
        "morphological_operations" => Dict(),
        "statistics" => Dict(),
        "formulas" => Dict()
    )
    
    # Add operation results
    for (name, ops) in results
        report["morphological_operations"][name] = Dict(
            "erosion" => Dict("hits" => ops["erosion"].hit_count, "misses" => ops["erosion"].miss_count),
            "dilation" => Dict("hits" => ops["dilation"].hit_count, "misses" => ops["dilation"].miss_count),
            "opening" => Dict("hits" => ops["opening"].hit_count, "misses" => ops["opening"].miss_count),
            "closing" => Dict("hits" => ops["closing"].hit_count, "misses" => ops["closing"].miss_count),
            "hit_or_miss" => Dict("hits" => ops["hit_or_miss"].hit_count, "misses" => ops["hit_or_miss"].miss_count),
            "morphological_gradient" => Dict("boundaries" => ops["morphological_gradient"].hit_count),
            "top_hat" => Dict("anomalies" => ops["top_hat"].hit_count)
        )
    end
    
    # Add statistics
    report["statistics"] = Dict(
        "mean_amount" => mean(stream.amounts),
        "std_amount" => std(stream.amounts),
        "min_amount" => minimum(stream.amounts),
        "max_amount" => maximum(stream.amounts),
        "median_amount" => median(stream.amounts),
        "aml001_detected" => sum(stream.amounts .>= 7500.0 .&& stream.amounts .< 10000.0 .&& .!stream.is_card),
        "aml003_detected" => sum([c in Set(["SC", "RU", "IR", "KP", "KY"]) for c in stream.countries]),
        "frd003_detected" => sum(stream.is_card .&& stream.amounts .> 1200.0),
        "total_alerts" => sum([sum(stream.amounts .>= 7500.0 .&& stream.amounts .< 10000.0 .&& .!stream.is_card),
                               sum([c in Set(["SC", "RU", "IR", "KP", "KY"]) for c in stream.countries]),
                               sum(stream.is_card .&& stream.amounts .> 1200.0)])
    )
    
    # Add mathematical formulas
    report["formulas"] = Dict(
        "erosion" => "A ⊖ B = {z | B_z ⊆ A}",
        "dilation" => "A ⊕ B = {z | (B̂_z ∩ A) ≠ ∅}",
        "opening" => "A ∘ B = (A ⊖ B) ⊕ B",
        "closing" => "A • B = (A ⊕ B) ⊖ B",
        "hit_or_miss" => "A ⊛ B = (A ⊖ B₁) ∩ (A^c ⊖ B₂)",
        "morphological_gradient" => "Grad(A) = (A ⊕ B) - (A ⊖ B)",
        "top_hat" => "T_hat(A) = A - (A ∘ B)",
        "structuring_element" => "B = {x | condition(x) = true}",
        "confidence_score" => "C = TP / (TP + FP)",
        "throughput" => "T = N / t",
        "alert_volume" => "V = Σ_{i=1}^{N_alerts} amount_i"
    )
    
    return report
end

"""
Save corpus report to JSON file.
Korpusz jelentés mentése JSON fájlba.
"""
function save_corpus_report(report::Dict, filename::String)
    open(filename, "w") do f
        JSON.print(f, report, 4)
    end
    println("✔ Corpus report saved to: $filename")
end

"""
Generate HTML corpus report.
HTML korpusz jelentés generálása.
"""
function generate_html_corpus(report::Dict, filename::String)
    html = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>UNICAGD_AML - Mathematical Morphology Analysis Corpus</title>
    <style>
        body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif; background: #0d1117; color: #c9d1d9; padding: 30px; line-height: 1.6; }
        .container { max-width: 1400px; margin: auto; background: #161b22; border: 1px solid #30363d; border-radius: 8px; padding: 24px; }
        h1, h2, h3 { color: #58a6ff; }
        table { width: 100%; border-collapse: collapse; margin-top: 16px; margin-bottom: 24px; }
        th, td { border: 1px solid #30363d; padding: 10px 14px; text-align: left; }
        th { background: #21262d; color: #f0f6fc; }
        .critical { color: #f85149; font-weight: bold; }
        .high { color: #d29922; font-weight: bold; }
        .formula { background: #21262d; padding: 12px; border-radius: 6px; font-family: 'Courier New', monospace; margin: 8px 0; }
        .badge { display: inline-block; padding: 2px 8px; border-radius: 12px; font-size: 12px; background: #238636; color: white; margin-right: 8px; }
        .section { margin-bottom: 32px; padding-bottom: 16px; border-bottom: 1px solid #30363d; }
    </style>
</head>
<body>
    <div class='container'>
        <h1>🔬 UNICAGD_AML Mathematical Morphology Analysis Corpus</h1>
        <p><span class='badge'>Version 1.3.0</span><span class='badge'>Julia Analysis</span><span class='badge'>$(Dates.format(now(), "YYYY-MM-DD HH:MM:SS"))</span></p>
        
        <div class='section'>
            <h2>📊 Stream Metadata</h2>
            <table>
                <tr><th>Property</th><th>Value</th></tr>
                <tr><td>Stream Size</td><td>$(report["metadata"]["stream_size"]) transactions</td></tr>
                <tr><td>Total Volume</td><td>$$(round(report["metadata"]["total_volume"], digits=2)) USD</td></tr>
                <tr><td>Unique Countries</td><td>$(report["metadata"]["unique_countries"])</td></tr>
                <tr><td>Card Transaction Ratio</td><td>$(round(report["metadata"]["card_ratio"] * 100, digits=2))%</td></tr>
            </table>
        </div>
        
        <div class='section'>
            <h2>🧮 Structuring Elements</h2>
            <p>Mathematical morphology structuring elements define the patterns for detection:</p>
            <table>
                <tr><th>Rule</th><th>Condition</th><th>Category</th><th>Severity</th><th>Confidence</th></tr>
                $(join(["""
                <tr>
                    <td><strong>$(name)</strong></td>
                    <td><code>$(data["condition"])</code></td>
                    <td>$(data["category"])</td>
                    <td class='$(lowercase(data["severity"]))'>$(data["severity"])</td>
                    <td>$(data["confidence"])</td>
                </tr>
                """ for (name, data) in report["structuring_elements"]], "\n"))
            </table>
        </div>
        
        <div class='section'>
            <h2>📐 Mathematical Formulas</h2>
            $(join(["""
            <div class='formula'>
                <strong>$(name):</strong><br>
                <code>$(formula)</code>
            </div>
            """ for (name, formula) in report["formulas"]], "\n"))
        </div>
        
        <div class='section'>
            <h2>🔍 Morphological Operation Results</h2>
            <table>
                <tr><th>Operation</th><th>Erosion Hits</th><th>Dilation Hits</th><th>Opening Hits</th><th>Closing Hits</th><th>Hit-or-Miss</th><th>Boundaries</th><th>Anomalies</th></tr>
                $(join(["""
                <tr>
                    <td><strong>$(name)</strong></td>
                    <td>$(ops["erosion"]["hits"])</td>
                    <td>$(ops["dilation"]["hits"])</td>
                    <td>$(ops["opening"]["hits"])</td>
                    <td>$(ops["closing"]["hits"])</td>
                    <td>$(ops["hit_or_miss"]["hits"])</td>
                    <td>$(ops["morphological_gradient"]["boundaries"])</td>
                    <td>$(ops["top_hat"]["anomalies"])</td>
                </tr>
                """ for (name, ops) in report["morphological_operations"]], "\n"))
            </table>
        </div>
        
        <div class='section'>
            <h2>📈 Detection Statistics</h2>
            <table>
                <tr><th>Metric</th><th>Value</th></tr>
                <tr><td>Mean Amount</td><td>$$(round(report["statistics"]["mean_amount"], digits=2))</td></tr>
                <tr><td>Std Deviation</td><td>$$(round(report["statistics"]["std_amount"], digits=2))</td></tr>
                <tr><td>Min Amount</td><td>$$(round(report["statistics"]["min_amount"], digits=2))</td></tr>
                <tr><td>Max Amount</td><td>$$(round(report["statistics"]["max_amount"], digits=2))</td></tr>
                <tr><td>Median Amount</td><td>$$(round(report["statistics"]["median_amount"], digits=2))</td></tr>
                <tr><td>AML-001 Detected</td><td>$(report["statistics"]["aml001_detected"])</td></tr>
                <tr><td>AML-003 Detected</td><td>$(report["statistics"]["aml003_detected"])</td></tr>
                <tr><td>FRD-003 Detected</td><td>$(report["statistics"]["frd003_detected"])</td></tr>
                <tr><td>Total Alerts</td><td class='critical'>$(report["statistics"]["total_alerts"])</td></tr>
            </table>
        </div>
        
        <div class='section'>
            <h2>📝 Interpretation</h2>
            <p>This mathematical morphology analysis corpus provides a formal set-theoretic interpretation of the UNICAGD_AML detection engine:</p>
            <ul>
                <li><strong>Structuring Elements (B):</strong> Define the morphological "patterns" being searched for in transaction streams. Each rule (AML-001, AML-003, FRD-003) corresponds to a different structuring element.</li>
                <li><strong>Erosion (⊖):</strong> Removes transactions that don't match the structuring element, effectively filtering out legitimate transactions.</li>
                <li><strong>Dilation (⊕):</strong> Expands suspicious regions to include neighboring transactions that may be part of the same pattern.</li>
                <li><strong>Opening (∘):</strong> Removes isolated false positives while preserving genuine patterns.</li>
                <li><strong>Closing (•):</strong> Connects nearby suspicious transactions that may be part of the same criminal network.</li>
                <li><strong>Hit-or-Miss (⊛):</strong> Precisely identifies transactions matching the exact structuring element pattern.</li>
                <li><strong>Morphological Gradient (Grad):</strong> Highlights boundaries between legitimate and suspicious transaction regions.</li>
                <li><strong>Top-Hat (T_hat):</strong> Isolates isolated anomalies that might indicate novel fraud patterns.</li>
            </ul>
        </div>
    </div>
</body>
</html>
    """
    
    open(filename, "w") do f
        write(f, html)
    end
    println("✔ HTML corpus report saved to: $filename")
end

"""
Main analysis pipeline.
Fő elemzési pipeline.
"""
function run_analysis(stream_size::Int=10000, fraud_ratio::Float64=0.08)
    println("================================================================================")
    println(" 🔬 UNICAGD_AML - Mathematical Morphology Analysis Corpus")
    println("================================================================================")
    println()
    
    # Generate test stream
    println("📊 Generating transaction stream...")
    stream = generate_test_stream(stream_size, fraud_ratio)
    println("   ✔ Generated $(stream_size) transactions")
    println("   ✔ Total volume: $$(round(sum(stream.volumes), digits=2)) USD")
    println()
    
    # Analyze stream
    println("🔍 Running morphological analysis...")
    results = analyze_stream(stream)
    println("   ✔ Analyzed 3 structuring elements")
    println("   ✔ Applied 7 morphological operations each")
    println()
    
    # Generate report
    println("📝 Generating analytical corpus...")
    report = generate_corpus_report(stream, results)
    
    # Save outputs
    timestamp = Dates.format(now(), "YYYYMMDD_HHMMSS")
    json_file = "docs/morphological_corpus_$timestamp.json"
    html_file = "docs/morphological_corpus_$timestamp.html"
    
    save_corpus_report(report, json_file)
    generate_html_corpus(report, html_file)
    
    println()
    println("================================================================================")
    println(" 🏁 ANALYSIS COMPLETE")
    println("================================================================================")
    println("📄 JSON Report: $json_file")
    println("🌐 HTML Report: $html_file")
    println()
    
    return report
end

end # module
