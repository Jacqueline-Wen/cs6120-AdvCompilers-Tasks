**Team Members**
Serena Zhang (syz8), Maggie Gao (mg2447), Jacqueline Wen (jw2347)

[Source code URL](https://github.com/Jacqueline-Wen/cs6120-AdvCompilers-Tasks/tree/main/Task8-LICM)

## Pass
For this assignment, we implemented LICM with LLVM. We noticed that LLVM had a lot of pre-built functions, such as `llvm::hasLoopInvariantOperands`, and we used this to make our lives a little easier. We implemented the LICM optimization as decribed on the website using the same skeleton as Task7. 

Working with LLVM for this assignment was pretty frustrating. Namely, we had to juggle dynamically casting between const and non-const instructions (honestly this is pretty terrible C++ coding) as some built in LLVM functions required the parameters to be const, while others did not.  

## Testing
To first sanity check our code, we write a simple C++ program and compared the intermediate representations with and without the pass. When we manually compared the intermediate representations, we noticed that the optimization was being implemented. 

To more rigorously test our code, we used embench. Here are our results:

| Benchmark       | Original Speed | Optimized Speed |
|-----------------|----------------|-----------------|
| aha-mont64      | 2.45           | 2.45            |
| crc32           | 1.95           | 1.94            |
| cubic           | 191.96         | 190.00          |
| edn             | 9.23           | 9.57            |
| huffbench       | 8.28           | 8.33            |
| matmult-int     | 8.86           | 8.78            |
| md5sum          | 5.79           | 5.36            |
| minver          | 29.79          | 29.84           |
| nbody           | 500.62         | 513.06          |
| nettle-aes      | 6.29           | 6.11            |
| nettle-sha256   | 5.81           | 5.78            |
| nsichneu        | 8.26           | 8.25            |
| picojpeg        | 10.47          | 10.54           |
| primecount      | 3.07           | 3.07            |
| qrduino         | 6.69           | 7.00            |
| sglib-combined  | 4.33           | 4.57            |
| slre            | 5.38           | 6.28            |
| st              | 115.43         | 113.86          |
| statemate       | 9.62           | 12.81           |
| tarfind         | 8.87           | 8.88            |
| ud              | 9.24           | 9.20            |
| wikisort        | 38.85          | 37.85           |

It seems that we were able to speed up some of the programs with our LICM optimization. However, there are also many programs where the speed is unaffected or increased by the optimization. Overall, we observed no substantial results with embench. Perhaps more loop optimizations are necessary for obvious speedups in programs. 

## Michelin Star
We believe that we deserve a michelin start for completely the specification of this assignment. Although we didn't see a desirable outcome, we still learned a lot by completing this assignment. 