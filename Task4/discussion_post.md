**Team Members**
Serena Zhang (syz8), Maggie Gao (mg2447), Jacqueline Wen (jw2347)

[Source code URL](https://github.com/Jacqueline-Wen/cs6120-AdvCompilers-Tasks/tree/main/Task4)

##### Reaching Definitions
For the implementation of reaching definitions, we mostly followed the pseudocode logic provided in class. While we still had "unprocessed" blocks, if processing that basic block changed the output, we would add its successors back into the list of "unprocessed" blocks. The algorithm converges when the list of "unprocessed" blocks are empty. When we had to merge multiple input blocks, we simply took the union. While processing the basic block (transfers), we took the union of the newly define variables, as well as the defined variables being passed into the basic block, replacing variables that are reassigned. 

##### Refactoring Utilities
To make basic blocks easier to access/track, we implemented a `BasicBlocks` class that automatically parsed a json file and extracted the basic blocks, as well as its predecessors and successors. This refactoring was mostly done to increase code readability. 

##### Testing
For this assignment, we tested by visually inspecting and logically verifying the outputs. We manually traced through all the tests to confirm the correctness of the `in` and `out` of each block. To ensure a broad coverage, we made sure to use a variety of test cases that looked at various control flow patterns (branches, jumps, recursions, loops, etc.) as well as different types of benchmarks. `ackermann`, `binary-fmt`, and `binpow` are from `benchmarks/core`. `dead-branch` is from `benchmarks/long`. `fib` and `vsmul` are from `benchmarks/mem`. `harmonic-sum` is from `benchmarks/float`.

##### Hardest Part
The hardest part of this assignment was being able to implement the dataflow logic cleanly in C++ without having the convenience of high-level set operations. The algorithm requres frequent unions and replacements, which would have been pretty straightforward with sets. Instead, we had to carefully manage these operations with vectors and maps.

##### Michelin Star
We believe we deserve a michelin star because our implementation is correct, robust, and well-tested. We handled the complexity of dataflow analysis in C++ and validated our work across an array of diverse benchmarks.

