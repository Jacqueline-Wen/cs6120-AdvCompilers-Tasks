#include <iostream>
#include <fstream>
#include <set>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

map<string, vector<string>> basic_blocks;
map<string, vector<string>> labels;

void form_basic_blocks(json& j) {
    string curLabel = "main";
    queue<string> curBlock;
    bool prevWasTerm = false;
    for (auto a: j["functions"]) {
        for (auto instr: a["instrs"]) {
            // end block
            if (!instr["op"].is_null() && (instr["op"] == "br" || instr["op"] == "jmp" || instr["op"] == "ret")) {
                curBlock.push(to_string(instr));

                for (auto nextLabel: instr["labels"]) {
                    labels[curLabel].push_back(nextLabel.get<std::string>());
                }
                while (curBlock.size() > 0) {
                    basic_blocks[curLabel].push_back(curBlock.front());
                    curBlock.pop();
                }
                prevWasTerm = true;
            } else if (!instr["label"].is_null()) {
                while (curBlock.size() > 0) {
                    basic_blocks[curLabel].push_back(curBlock.front());
                    curBlock.pop();
                }
                if (!prevWasTerm) {
                    labels[curLabel].push_back(instr["label"]);
                }
                prevWasTerm = false;
                curLabel = instr["label"];
            } else {
                curBlock.push(to_string(instr));
                prevWasTerm = false;
            }
        }
    }
    while (curBlock.size() > 0) {
        basic_blocks[curLabel].push_back(curBlock.front());
        curBlock.pop();
    }
}

int main(int argc, char* argv[]) {
    std::ifstream f(argv[1]);
    json j = json::parse(f);

    form_basic_blocks(j);

    for (auto a: basic_blocks) {
        cout << "block: " << a.first << endl;
        for (auto b: a.second) {
            cout << b << endl;
        }
        cout << "-----\n\n";
    }

    cout << "Connections:\n";
    for (auto a: labels) {
        for (auto b: a.second) {
            cout << a.first << "-->" << b << endl;
        }
    }

} 