#include "basicBlocks.hpp"

BasicBlocks::BasicBlocks(json &j) {
    // creating basic blocks
    int curLabel = 1;

    queue<json> curBlock;
    for (auto a : j["functions"]) {
        if (a.contains("args")){
            for(auto arg: a["args"]){
                json instr;
                instr["dest"] = arg["name"];
                instr["type"] = arg["type"];
                instr["op"] = "arg";
                curBlock.push(instr);
            }
        }

        for (auto instr : a["instrs"]) {
            // end block
            if (instr.contains("op") &&
                (instr["op"] == "br" || instr["op"] == "jmp" ||
                 instr["op"] == "ret")) {
                curBlock.push(instr);

                while (curBlock.size() > 0) {
                    blocks_[curLabel].push_back(curBlock.front());
                    curBlock.pop();
                }
                curLabel++;
            } else if (instr.contains("label")) {
                if (curBlock.size() > 0) {
                    while (curBlock.size() > 0) {
                        blocks_[curLabel].push_back(curBlock.front());
                        curBlock.pop();
                    }
                    curLabel++;
                    curBlock.push(instr);
                }
                blockToLabelName_[curLabel] = instr["label"];
                labelNameToBlock_[instr["label"]] = curLabel;
            } else {
                curBlock.push(instr);
            }
        }
    }
    while (curBlock.size() > 0) {
        blocks_[curLabel].push_back(curBlock.front());
        curBlock.pop();
    }

    // populating links
    bool saveNext = false;
    int prev = -1;
    for (auto a: blocks_) {
        int blockname = a.first;
        auto lastCommand = a.second[a.second.size()-1];
        if (saveNext) {
            predecessors_[blockname].push_back(prev);
            successors_[prev].push_back(blockname);
            saveNext = false;
        }
        if (lastCommand.contains("op") && (lastCommand["op"] == "br" || lastCommand["op"] == "jmp")) {
            for (auto child : lastCommand["labels"]) {
                int childInt = labelNameToBlock_[child];
                successors_[blockname].push_back(childInt);
                predecessors_[childInt].push_back(blockname);
            }
        }
        else if (lastCommand.contains("op") && lastCommand["op"] == "ret") {
            //no successors
        } else {
            saveNext = true;
            prev = blockname;
        }
    }
}

map<int, vector<json>> BasicBlocks::getBlocks() {
    return blocks_;
}

vector<int> BasicBlocks::getPredecessors(int label) {
    return predecessors_[label];
}

vector<int> BasicBlocks::getSuccessors(int label) {
    return successors_[label];
}