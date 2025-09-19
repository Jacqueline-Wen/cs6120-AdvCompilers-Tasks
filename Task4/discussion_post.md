**Team Members**
Serena Zhang (syz8), Maggie Gao (mg2447), Jacqueline Wen (jw2347)


[Source code URL](https://github.com/Jacqueline-Wen/cs6120-AdvCompilers-Tasks/tree/main/Task4)

##### Reaching Definitions
For the implementation of reaching definitions, we mostly followed the pseudocode logic provided in class. While we still had "unprocessed" blocks, we would 


##### Refactoring Utilities
To make basic blocks easier to access/track, we implemented a `BasicBlocks` class that automatically parsed a json file and extracted the basic blocks, as well as its predecessors and successors. This refactoring was mostly done to increase code readability. 