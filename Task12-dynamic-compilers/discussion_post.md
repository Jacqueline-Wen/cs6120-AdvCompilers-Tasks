**Team Members**
Serena Zhang (syz8), Maggie Gao (mg2447), Jacqueline Wen (jw2347)

[Source code URL](https://github.com/Jacqueline-Wen/cs6120-AdvCompilers-Tasks/tree/main/Task12-dynamic-compilers)

## What We Did
For our tracing setup, we modified the Bril interpreter (brili.ts) to record at most 10 dynamic instructions, giving us a short, bounded trace. After generating this trace, our trace.cpp tool processed it by rewriting branches into guard instructions and reconstructing function calls and returns. We then wrapped the transformed trace in a speculative region. If all guards succeed at runtime, execution reaches the trace_success label and commits the speculative state. Otherwise, any mismatch triggers the guard_failed label, which falls back to the original program.

## Testing
We evaluated our trace program on three tests: a custom `sum_loop` bril program and two existing benchmarks from the `core` folder in `bril/benchmarks`: `armstrong` and `sum-divisors`. Our created `sum_loop` program takes an input n and computes the sum of integers from 1 to n - 1. This gives us a simple, loop-heavy test.

For each test, we created four versions in the directory:
1. `*.json`: the original Bril program in JSON form.
2. `*.trace`: the execution trace generated from running the JSON program with a chosen input. We used the recommended input form `armstrong` and `sum-divisors`, which are 407 and 100 respectively. For `sum_loop` we used the input 15.
3. `*_opt.json`: the optimized Bril program produced by `trace.cpp`
4. `*_opt.trace`: the trace of the optimized program, which was ran with the same inputs as the original.

In each case, we used the original (`*.trace`) and optimized (`*_opt.trace`) traces to confirm that both versions produce identical printed outputs (booleans or integers, depending on the program), giving us confidence that our optimization preserved the proper semantics of each program.

Here, we have a table comparing the dynamic instructions before and after tracing.

| Benchmark | Baseline Dynamic Instrs | Traced Dynamic Instrs |
|---------|----------|----------|
| sum_loop | 81  | 92 |
| armstrong | 133  | 144  |
| sum-divisors | 159  | 167  |

<br>

`sum-divisors` - Testing on other inputs we get:

| Input | Traced Dynamic Instrs |
|---------|----------|
| 50 | 121 |
| 73 | 116  |
| 137 | 149 |
| 174 | 195 |

## Hardest Part
The hardest part was properly reconstructing the control flow and behavior from the raw trace file. This is because it is fundamentally stream-based and gives relatively little context. 

## Michelin Star
We believe that we deserve a michelin star because we produced a clean, working optimizer that correctly reconstructs control flow, handles calls and returns, inserts guards, and preserves program behavior across all our tests.
