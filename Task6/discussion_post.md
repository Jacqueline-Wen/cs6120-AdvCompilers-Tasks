**Team Members**
Serena Zhang (syz8), Maggie Gao (mg2447), Jacqueline Wen (jw2347)

[Source code URL](https://github.com/Jacqueline-Wen/cs6120-AdvCompilers-Tasks/tree/main/Task6)

## Converting to SSA

## Converting from SSA
This part of the assignment involves converting from SSA back into regular form. Here, we are replacing each $\phi$-node with explicit `id` assignments on the control-flow edges that feed into the $\phi$'s block. Our implementation first identifies all the $\phi$-nodes in each block and creates copies of their corresponding source values in each predecessor. To avoid breaking control flow, we also have the function `isTerminator` which checks for branch, jump, and return instructions. This ensures the inserted copies appear right before these terminators.

## Testing

## Hardest Part

# Michelin Star
