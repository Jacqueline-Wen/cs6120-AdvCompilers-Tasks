To get test.json: `bril2json < test.bril > test.json`

To get test.trace: `bril2json < test.bril | brili > test.trace`

To compile trace.cpp:
`g++ -o trace trace.cpp -std=c++17 -I/opt/homebrew/opt/nlohmann-json/include`

To run trace.cpp on test.json and test.trace: `./trace test.json test.trace`
