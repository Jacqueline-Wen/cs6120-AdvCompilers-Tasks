#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

bool removeReassign(json& j) {
    return false;
}

bool removeUnusedVar(json& j) {
    bool changed = false;
    for (auto& function: j["functions"]) {
        map<string, bool> varUsed;
        for (auto instr: function["instrs"]) {
            for (auto a: instr["args"]) {
                varUsed[a] = true;
            }
        }

        auto &instrs = function["instrs"];

        for (auto it = instrs.begin(); it != instrs.end(); ++it) {
            if ((*it).contains("dest")){
                string d = (*it)["dest"];
                if (!varUsed[d]) {
                    it = instrs.erase(it);
                    changed = true;
                    continue;
                }
            }
        }
    }
    return changed;
}


int main(int argc, char* argv[]) {
    std::ifstream f(argv[1]);
    json j = json::parse(f);

    bool notConverged = true;

    while(notConverged) {
        notConverged = removeUnusedVar(j) || removeReassign(j);
    }

    cout << j.dump(4) << endl;
}