#include "../utility/basicBlocks.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <queue>
#include <set>

using namespace std;
using json = nlohmann::json;

void printBlockInAndOut(const map<int, map<string, vector<int>>> &blockIn,
                        const map<int, map<string, vector<int>>> &blockOut) {
    for (const auto &pair : blockIn) {
        int label = pair.first;
        map<string, vector<int>> ins = pair.second;
        map<string, vector<int>> outs = blockOut.at(label);
        cout << "Block " << label << endl;
        cout << "  in:" << endl;
        for (const auto &in : ins) {
            cout << "    " << in.first << " ";
            for (const int def : in.second) {
                cout << def << " ";
            }
            cout << endl;
        }
        cout << "  out:" << endl;
        for (const auto &out : outs) {
            cout << "    " << out.first << " ";
            for (const int def : out.second) {
                cout << def << " ";
            }
            cout << endl;
        }
    }
}

map<json, int> assignInstructionIds(json j) {
    map<json, int> instrToId;
    int id = 0;
    for (const auto &function : j["functions"]) {
        const auto &instrs = function["instrs"];
        for (const auto &instr : function["instrs"]) {
            if (instr.contains("dest")) {
                instrToId[instr] = id++;
            }
        }
    }
    return instrToId;
}

map<string, vector<int>> merge(vector<int> predecessors,
                               map<int, map<string, vector<int>>> blockOut) {
    map<string, vector<int>> merged;
    for (const auto pred : predecessors) {
        for (const auto def : blockOut[pred]) {
            for (const auto var : def.second) {
                if (find(merged[def.first].begin(), merged[def.first].end(),
                         var) == merged[def.first].end()) {
                    merged[def.first].push_back(var);
                }
            }
            sort(merged[def.first].begin(), merged[def.first].end());
        }
    }
    return merged;
}

bool transfer(map<string, vector<int>> blockIn,
              map<string, vector<int>> &blockOut, vector<json> instrs,
              map<json, int> instrToId, int label) {
    map<string, vector<int>> updatedOut = blockIn;
    bool outChanged = false;
    for (const auto instr : instrs) {
        if (!instr.contains("op") || !instr.contains("dest")) {
            continue;
        }
        string op = instr["op"];
        string dest = instr["dest"];
        if (op == "id") {
            updatedOut[dest].clear();
            updatedOut[dest] = updatedOut[instr["args"][0]];
        } else {
            updatedOut[dest].clear();
            updatedOut[dest].push_back(instrToId[instr]);
            outChanged = true;
        }
        if (updatedOut[dest].size() != blockOut[dest].size() ||
            !std::equal(updatedOut[dest].begin(),
                        updatedOut[dest].begin() + updatedOut[dest].size(),
                        blockOut[dest].begin())) {
            outChanged = true;
        }
    }
    blockOut = std::move(updatedOut);
    return outChanged;
}

pair<map<int, map<string, vector<int>>>, map<int, map<string, vector<int>>>>
reachingDefinitions(shared_ptr<BasicBlocks> basicBlocks,
                    map<json, int> instrToId) {
    map<int, vector<json>> blocks = basicBlocks->getBlocks();
    map<int, map<string, vector<int>>> blockIn;
    map<int, map<string, vector<int>>> blockOut;
    int index = 0;
    queue<int> q;
    for (const auto a : blocks) {
        q.push(a.first);
    }

    while (q.size() > 0) {
        int label = q.front();
        q.pop();
        map<string, vector<int>> updatedIn =
            merge(basicBlocks->getPredecessors(label), blockOut);
        blockIn[label] = updatedIn;
        bool outChanged = transfer(blockIn[label], blockOut[label],
                                   blocks[label], instrToId, label);
        if (outChanged) {
            for (const auto succ : basicBlocks->getSuccessors(label)) {
                q.push(succ);
            }
        }
        printBlockInAndOut(blockIn, blockOut);
    }
    return {blockIn, blockOut};
}

int main(int argc, char *argv[]) {
    json j;
    std::cin >> j;

    map<json, int> instrToId = assignInstructionIds(j);

    shared_ptr<BasicBlocks> basicBlocks = make_shared<BasicBlocks>(j);
    auto [blockIn, blockOut] =
        reachingDefinitions(std::move(basicBlocks), instrToId);
    printBlockInAndOut(blockIn, blockOut);
}
