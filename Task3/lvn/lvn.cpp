#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

bool localValueNumbering(json& j){
    for (auto& function: j["functions"]){
        map<vector<int>, pair<int, string>> table;
        map<string, int> var2num;
        int nextNum = 1;

        auto& instrs = function["instrs"];
        for (auto& intstr: instrs){
            // Building the value tuple
            vector<int> value;
            string op = instr["op"];
            value.push_back(hash<string>{}(op));

            if(instr.contains("args")){
                for (auto& a: instr["args"]){
                    string var = a;
                    int num = var2num.count(var) ? var2num[var] : -1
                    value.push_back(num);

                }
            }

            if (table.count(value)){
                int num = table[value].first;
                string var = table[value].second;

                instr = {
                    {"op","id"},
                    {"args", json::array({var})},
                    {"dest", instr["dest"]}
                };
                var2num[instr["dest"]] = num;
                changed = true;
            }
            else {
                int num = nextNum++;
                string dest = instr.contains("dest") ? string(instr["dest"]) : "";
                table[value] = {num, dest};
                
                if (instr.contains("args")){
                    for (auto& a: instr["args"]){
                        string var = a;
                        if var2num.count(var){
                            int tempNum = var2num[var];
                            for (auto& kv : table){
                                if (kv.second.first == tempNum) {
                                    a = kv.second.second;
                                    break;
                                }
                            }
                        }
                    }
                }
                if (!dest.empty()){
                    var2num[dest] = num;
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    std::ifstream f(argv[1]);
    json j = json::parse(f);

    bool notConverged = true
    while (notConverged) {
        notConverged = localValueNumbering(j);
    }

    cout << j.dump(4) << endl;
}