#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

bool removeReassign(json &j)
{
    bool changed = false;
    for (auto &function : j["functions"])
    {
        map<string, int> previousUsage;
        map<string, int> previousAssign;
        set<int> linesToRemove;
        int counter = 1;
        for (auto instr : function["instrs"])
        {
            // check for reassignment
            if (!instr["dest"].is_null())
            {
                string d = instr["dest"];
                if (previousAssign[d] > previousUsage[d])
                {
                    linesToRemove.insert(previousAssign[d]);
                }
                previousAssign[d] = counter;
            }
            for (auto a : instr["args"])
            {
                previousUsage[a] = counter;
            }
            counter++;
        }

        auto &instrs = function["instrs"];

        counter = 1;
        for (auto it = instrs.begin(); it != instrs.end();)
        {
            if (linesToRemove.find(counter) != linesToRemove.end())
            {
                it = instrs.erase(it);
                changed = true;
            }
            else {
                it++;
            }
            counter++;
        }
    }
    return changed;
}

bool removeUnusedVar(json &j)
{
    bool changed = false;
    for (auto &function : j["functions"])
    {
        map<string, bool> varUsed;
        for (auto &instr : function["instrs"])
        {
            if (instr.contains("args")){
                for (auto &a : instr["args"])
                {
                    varUsed[a] = true;
                }
            }
        }

        auto &instrs = function["instrs"];

        for (auto it = instrs.begin(); it != instrs.end();)
        {
            if (it -> contains("dest"))
            {
                string d = (*it)["dest"];
                if (!varUsed[d])
                {
                    it = instrs.erase(it);
                    changed = true;
                    continue;
                }
                else{
                    it++;
                }
            }
            else {
                it++;
            }
        }
    }
    return changed;
}

int main(int argc, char *argv[])
{
    // std::ifstream f(argv[1]);
    // json j = json::parse(f);
    // json j;
    // std::cin >> j;
    json j;
    try {
        std::cin >> j;
    } catch (const json::parse_error& e) {
        std::cerr << "ERROR: Failed to parse JSON from stdin: " << e.what() << std::endl;
        return 1;
    }

    bool notConverged = true;

    while (notConverged)
    {
        notConverged = removeUnusedVar(j) || removeReassign(j);
    }

    cout << j.dump(4) << endl;
}