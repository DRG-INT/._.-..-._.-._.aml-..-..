"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const AmlEngine_1 = require("./engine/AmlEngine");
const TuiRenderer_1 = require("./tui/TuiRenderer");
async function main() {
    const args = process.argv.slice(2);
    let count = 50000;
    let fraudRatio = 0.08;
    let hardware = 'Node.js TypeScript SIMD / Metal GPU';
    for (let i = 0; i < args.length; i++) {
        if (args[i] === '--count' && args[i + 1])
            count = parseInt(args[++i], 10);
        else if (args[i] === '--fraud-ratio' && args[i + 1])
            fraudRatio = parseFloat(args[++i]);
        else if (args[i] === '--hardware' && args[i + 1])
            hardware = args[++i];
    }
    TuiRenderer_1.TuiRenderer.renderBanner('UNICAGD_AML [TypeScript Engine] - Turbocharged Stream & Audit', hardware);
    const engine = new AmlEngine_1.AmlEngine(hardware);
    console.log(`  \x1b[0;37mGenerating and pre-buffering ${count.toLocaleString()} mixed transactions...\x1b[0m`);
    const transactions = engine.generateSyntheticStream(count, fraudRatio);
    console.log('  \x1b[1;32m✔ Stream buffer ready.\x1b[0m Starting real-time stream evaluation...\n');
    const preview = transactions.slice(0, Math.min(20, count));
    for (const tx of preview) {
        const alert = (tx.country === 'SC') ? {
            alert_id: 'ALT_PREVIEW',
            rule_id: 'AML-003',
            rule_name: 'Sanctions Flag',
            category: 'AML_HIGH_RISK_JURISDICTION',
            severity: 'CRITICAL',
            subject_account: tx.originator_account,
            total_suspicious_amount: tx.amount,
            currency: tx.currency,
            confidence_score: 0.98,
            reason: 'Sanctioned jurisdiction match: SC',
            typology: 'Sanctions Evasion',
            timestamp: tx.timestamp
        } : undefined;
        TuiRenderer_1.TuiRenderer.renderLogEntry(tx, alert);
    }
    console.log(`\n  \x1b[0;37m... (accelerating remaining ${(count - preview.length).toLocaleString()} transactions) ...\x1b[0m\n`);
    const report = engine.evaluate(transactions);
    TuiRenderer_1.TuiRenderer.renderProgress(count, count, report.summary.execution_time_seconds, report.summary.throughput_tx_per_sec, report.summary.alerts_count, report.summary.total_suspicious_amount);
    TuiRenderer_1.TuiRenderer.renderFinalReport(report);
}
void main();
