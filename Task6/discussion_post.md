**Team Members**
Serena Zhang (syz8), Maggie Gao (mg2447), Jacqueline Wen (jw2347)

[Source code URL](https://github.com/Jacqueline-Wen/cs6120-AdvCompilers-Tasks/tree/main/Task6)

## Converting to SSA
For converting to SSA, we split our code into 3 parts: collecting variable definitions, inserting the phi nodes, and renaming the variables. `collectVariableDefinitions` builds a map of which variables are defined in which blocks by iterating through all the instructions in a block and saving the `dest` and `args` for each variable definition in a map. For `insertPhiNodes`, we insert $\phi$ nodes based on the dominance frontier. Finally, for `renameVariables`, we assign unique names to each variable and update instructions and phi nodes. 

## Converting from SSA
This part of the assignment involves converting from SSA back into regular form. Here, we are replacing each $\phi$-node with explicit `id` assignments on the control-flow edges that feed into the $\phi$'s block. Our implementation first identifies all the $\phi$-nodes in each block and creates copies of their corresponding source values in each predecessor. To avoid breaking control flow, we also have the function `isTerminator` which checks for branch, jump, and return instructions. This ensures the inserted copies appear right before these terminators.

## Testing
For testing, we first manually verified via random sampling that the test cases with and without SSA had the same results. Then, we measured the overhead of adding SSA by comparing the static overhead of the generated bril files. Our results are shown below:

## Hardest Part
The hardest part was definitely implementing the converting to SSA, as well as debugging existing utility files that were broken. Converting to SSA was difficult purely because we had a lot of different parts to implement, which made debugging difficult. Debugging was also difficult because this project built off of some previous sections we implemented, and we found small bugs in those programs as well.

## Michelin Star
We believe we deserve a michilin star for implementing SSA, as well as integrating our program with previous tasks. 