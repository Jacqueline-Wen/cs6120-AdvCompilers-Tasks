**Team Members**
Serena Zhang (syz8), Maggie Gao (mg2447), Jacqueline Wen (jw2347)

[Source code URL](https://github.com/Jacqueline-Wen/cs6120-AdvCompilers-Tasks/tree/main/Task4)

##### Reaching Definitions
For the implementation of reaching definitions, we mostly followed the pseudocode logic provided in class. While we still had "unprocessed" blocks, if processing that basic block changed the output, we would add its successors back into the list of "unprocessed" blocks. The algorithm converges when the list of "unprocessed" blocks are empty. When we had to merge multiple input blocks, we simply took the union. While processing the basic block (transfers), we took the union of the newly define variables, as well as the defined variables being passed into the basic block, replacing variables that are reassigned. 

##### Refactoring Utilities
To make basic blocks easier to access/track, we implemented a `BasicBlocks` class that automatically parsed a json file and extracted the basic blocks, as well as its predecessors and successors. This refactoring was mostly done to increase code readability. 

##### Testing
For this assignment, we tested by visually inspecting and logically verifying the outputs. 