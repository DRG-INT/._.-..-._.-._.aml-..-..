# Julia AML Simulation Runner
# Julia AML Szimuláció Futtató

# Include the main AML module
# Betöltjük a fő AML modult
include("UnicagdAml.jl")

# Import the module into scope
# Importáljuk a modult a névtérbe
using .UnicagdAml

"""
Parse command-line arguments for transaction count.
Parancssori argumentumok feldolgozása tranzakciószámhoz.
"""
count = 50000
if length(ARGS) >= 1
    global count = parse(Int, ARGS[1])
end

"""
Run TUI simulation with specified transaction count.
TUI szimuláció futtatása megadott tranzakciószámmal.
"""
UnicagdAml.run_tui_simulation(count)
