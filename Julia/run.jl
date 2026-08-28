include("UnicagdAml.jl")
using .UnicagdAml

count = 50000
if length(ARGS) >= 1
    global count = parse(Int, ARGS[1])
end

UnicagdAml.run_tui_simulation(count)
