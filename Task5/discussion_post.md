**Team Members**
Serena Zhang (syz8), Maggie Gao (mg2447), Jacqueline Wen (jw2347)

[Source code URL](https://github.com/Jacqueline-Wen/cs6120-AdvCompilers-Tasks/tree/main/Task5)

### Find Dominators
Finding the dominators (`findDominators`) for each block involved starting with the assumption that every block is dominated by all the blocks, except for the entry which only dominates itself. We then repeatedly updated each block's dominator set with the rule: a block’s dominators are the intersection of its predecessors’ dominators plus the block itself. These iterations continue until the sets stop changing. The result is a mapping from blocks to the exact set of dominators.

### Dominance Tree
To find the dominance tree, we constructed `findImmediateDominators` to take the full dominator sets and prune them down to only the immediate dominators. The function involes removing any dominators that also dominate another dominator of the node. After filtering out the non-immediate ones, only the closest dominator remains. The result is a map where each node is paired with its immediate dominator, forming the dominance tree.

### Dominance Frontier
The function for finding the dominance frontier (`findDominanceFrontier`) involves first inverting the dominator relation. This is so we now have a mapping of each node knowing which blocks it dominates. Then, for all blocks that a block dominates, the algorithm checks that if a successor is not itself dominated, it belongs in the block's dominance frontier.

### Testing

### Hardest Part
The hardest part of the assignment was not the coding itself, but the process of carefully reviewing the test results. To make sure our implementation was correct, this involved manually laying out the basic blocks, drawing the control-flow graph, and tracking all the connections. Manually checking dominators, immediate dominators, and dominance frontiers required meticulous detail, as one small mistake could throw off all later computations.

### Michelin Star