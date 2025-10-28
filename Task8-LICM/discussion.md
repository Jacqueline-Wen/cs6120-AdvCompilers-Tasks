**Team Members**
Serena Zhang (syz8), Maggie Gao (mg2447), Jacqueline Wen (jw2347)

[Source code URL](https://github.com/Jacqueline-Wen/cs6120-AdvCompilers-Tasks/tree/main/Task8-LICM)

## Pass
For this assignment, we implemented LICM with LLVM. We noticed that LLVM had a lot of pre-built functions, such as `llvm::hasLoopInvariantOperands`, and we used this to make our lives a little easier. We implemented the LICM optimization as decribed on the website using the same skeleton as Task7. 

Working with LLVM for this assignment was pretty frustrating. Namely, we had to juggle dynamically casting between const and non-const instructions (honestly this is pretty terrible C++ coding) as some built in LLVM functions required the parameters to be const, while others did not.  

## Testing
To first sanity check our code, we write a simple C++ program and compared the intermediate representations with and without the pass. When we manually compared the intermediate representations, we noticed that the optimization was being implemented. 

To more rigorously test our code, we decided to test it against _____. However, upon running it against this test suite, we did not notice significant improvements in runtime. 

## Michelin Star
We believe that we deserve a michelin start for completely the specification of this assignment. Although we didn't see a desirable outcome, we still learned a lot by completing this assignment. 