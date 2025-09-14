**Team Members**
Serena Zhang (syz8), Maggie Gao (mg2447), Jacqueline Wen (jw2347)


[Source code URL](https://github.com/Jacqueline-Wen/cs6120-AdvCompilers-Tasks/tree/main/Task3)


**Dead Code Elimination**


**Local Value Numbering**

We first implemented the pseudocode for local value numbering given in class. We then extended our LVN implementation to commutative operations, copy propagation, and constant folding. 

For commutative operations, we found all the operations where the order of arguments does not matter (`add`, `mul`, `eq`, `ne`, `and`, `or`). Then, whenever an instruction used one of these operations, we sorted the arguments. This means that our program will be able to recognize identical instructions are the same regardless of the order or arguments (ex. `add a b` is the same as `add b a`). 

For copy propagation, we identify instructions that simply copy a value from one variable to another, such as `b: int = id a `. When we encounter such an instruction, we replace all subsequent uses of the target variable (t1) with the source variable (a) until the source is redefined. This eliminates the need for the intermediate variable and simplifies the code.

For constant folding, we built a const2var map which stores the numeric value for each variable that has been defined as or calculated to be a constant. For an instruction, if all arguments are found in this map, we can perform the computation at compile time. We have two dedicated helper functions for this: constant_folding_int for integer arithmetic operations (`add`, `sub`, `mul`, `div`) and constant_folding_bool for boolean and comparison operations (`not`, `eq`, `ne`, `lt`, `le`, `gt`, `ge`, `and`, `or`). If a computation is successful, the original instruction is replaced with a simple const instruction that assigns the calculated value to the destination variable. 

For all three extensions, we rely on dead code elimination to clean up any unnecessary code at the end. 


**Testing**