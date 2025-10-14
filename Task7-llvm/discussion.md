**Team Members**
Serena Zhang (syz8), Maggie Gao (mg2447), Jacqueline Wen (jw2347)

[Source code URL](https://github.com/Jacqueline-Wen/cs6120-AdvCompilers-Tasks/tree/main/Task7-llvm)

## Pass
We were honestly less ambitious for this task due to all of our group members having upcoming prelims. 
For our pass, we implemented a divide by 0 duct tape fix, where every time we encountered a divide by 0, we would replace it with a divide by 1. Additionally, when divide by 0 is encountered, we would print a message indicating so. Although this code is not practical in most ways and it would make production code more difficult to fix, implementing this pass helped us gain an understanding on how to use llvm. 

## Testing
For testing, we manually tested against both cases where we had divide by 0, and cases where we did not, and manually verified that the results matched our expectations. 

## Hardest Part + Gen AI Usage
The hardest part of this project was finding the correct syntax for llvm functions. 

For this assignment, we used ChatGPT to look up documentaton for how we would write various llvm functions. It was mostly useful, except for the times where we would be too lazy to explain context in a chat and the agent would misinterpret our rather ambiguous nessages.   

## Michelin Star
We believe we deserve a michilin star for implementing our (although basic) LLVM pass, which meets the requirement specified. 