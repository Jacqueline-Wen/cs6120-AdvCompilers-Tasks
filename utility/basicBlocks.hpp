#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <set>
#include <vector>
#pragma once

using json = nlohmann::json;
using namespace std;

class BasicBlocks {
public:
    BasicBlocks(json& j);
    map<int, vector<json>> getBlocks();
    vector<int> getPredecessors(int label);
    vector<int> getSuccessors(int label);

private:
    map<int, vector<int>> predecessors_;
    map<int, vector<int>> successors_;
    map<int, vector<json>> blocks_;
    map<int, string> blockToLabelName_;
    map<string, int> labelNameToBlock_;
};
