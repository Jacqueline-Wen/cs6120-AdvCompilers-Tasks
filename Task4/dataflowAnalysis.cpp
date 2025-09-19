#include "../utility/basicBlocks.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <queue>
#include <set>

using namespace std;
using json = nlohmann::json;

map<string, vector<int>> merge(vector<int> predecessors, map<int, map<string, vector<int>>> blockOut)
{
    map<string, vector<int>> merged;
    for (const auto pred : predecessors)
    {
        for (const auto def : blockOut[pred])
        {
            for (const auto var : def.second)
            {
                if (find(merged[def.first].begin(), merged[def.first].end(), var) == merged[def.first].end())
                {
                    merged[def.first].push_back(var);
                }
            }
        }
    }
    return merged;
}

bool transfer(map<string, vector<int>> blockIn, map<string, vector<int>> blockOut, vector<json> instrs, int label, int index)
{
    set<string> actionOps = {"add", "sub", "mul", "div", "not", "eq", "ne", "lt", "le", "gt", "ge", "and", "or"};
    bool outChanged = false;
    map<string, vector<int>> updatedOut = {};
    for (const auto instr : instrs)
    {
        string op = instr["op"];
        string dest = instr["dest"];
        if (op == "const")
        {
            updatedOut[dest].push_back(index++);
            outChanged = true;
        }
        else if (op == "id")
        {
            for (const auto &def : blockIn[dest])
            {
                updatedOut[dest].push_back(def);
                if (find(blockOut[dest].begin(), blockOut[dest].end(), def) == blockOut[dest].end())
                {
                    outChanged = true;
                }
            }
        }
        else if (actionOps.count(op))
        {
            for (const auto &def : blockIn[dest])
            {
                updatedOut[dest].push_back(index++);
            }
            outChanged = true;
        }
    }
    blockOut = updatedOut;
    return outChanged;
}

pair<map<int, map<string, vector<int>>>, map<int, map<string, vector<int>>>> reachingDefinitions(shared_ptr<BasicBlocks> basicBlocks)
{
    map<int, vector<json>> blocks = basicBlocks->getBlocks();
    map<int, map<string, vector<int>>> blockIn;
    map<int, map<string, vector<int>>> blockOut;
    int index = 0;
    queue<int> q;
    for (const auto a : blocks)
    {
        q.push(a.first);
    }

    while (q.size() > 0)
    {
        int label = q.front();
        q.pop();
        map<string, vector<int>> updatedIn = merge(basicBlocks->getPredecessors(label), blockOut);
        blockIn[label] = updatedIn;
        bool outChanged = transfer(blockIn[label], blockOut[label], blocks[label], label, index);
        if (outChanged)
        {
            for (const auto succ : basicBlocks->getSuccessors(label))
            {
                q.push(succ);
            }
        }
    }
    return {blockIn, blockOut};
}

int main(int argc, char *argv[])
{
    std::ifstream f(argv[1]);
    json j = json::parse(f);

    shared_ptr<BasicBlocks> basicBlocks = make_shared<BasicBlocks>(j);

    cout << "finished parsing json file\n";
    cout << basicBlocks->getBlocks().size() << endl;

    // map<int, vector<json>> blocks = basicBlocks->getBlocks();
    // basicBlocks->getPredecessors(8)
    // map<int, vector<string>> successors = {};
    // map<string, vector<string>> predecessors = {};
    auto [blockIn, blockOut] = reachingDefinitions(std::move(basicBlocks));
}
