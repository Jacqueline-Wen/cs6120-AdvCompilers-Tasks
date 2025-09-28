#include "basicBlocks.hpp"

BasicBlocks::BasicBlocks(json &j)
{
    // creating basic blocks
    int curLabel = 1;

    set<int> funcStartLabels;
    for (auto a : j["functions"])
    {
        queue<json> curBlock;
        if (a.contains("args"))
        {
            for (auto arg : a["args"])
            {
                json instr;
                instr["dest"] = arg["name"];
                instr["type"] = arg["type"];
                instr["op"] = "arg";
                curBlock.push(instr);
            }
        }

        blockToLabelName_[curLabel] = a["name"];
        labelNameToBlock_[a["name"]] = curLabel;
        funcStartLabels.insert(curLabel);

        for (auto instr : a["instrs"])
        {
            // end block
            if (instr.contains("op") &&
                (instr["op"] == "br" || instr["op"] == "jmp" ||
                 instr["op"] == "ret"))
            {
                curBlock.push(instr);

                while (curBlock.size() > 0)
                {
                    blocks_[curLabel].push_back(curBlock.front());
                    curBlock.pop();
                }
                curLabel++;
            }
            else if (instr.contains("label"))
            {
                if (curBlock.size() > 0)
                {
                    while (curBlock.size() > 0)
                    {
                        blocks_[curLabel].push_back(curBlock.front());
                        curBlock.pop();
                    }
                    curLabel++;
                    curBlock.push(instr);
                }
                blockToLabelName_[curLabel] = instr["label"];
                labelNameToBlock_[instr["label"]] = curLabel;
            }
            else
            {
                curBlock.push(instr);
            }
        }
        if (curBlock.size() == 0)
        {
            continue;
        }
        while (curBlock.size() > 0)
        {
            blocks_[curLabel].push_back(curBlock.front());
            curBlock.pop();
        }
        curLabel++;
    }

    // populating links
    bool saveNext = false;
    int prev = -1;
    for (auto a : blocks_)
    {
        int blockname = a.first;
        auto instrs = a.second;
        if (saveNext && funcStartLabels.find(blockname) == funcStartLabels.end())
        {
            predecessors_[blockname].push_back(prev);
            successors_[prev].push_back(blockname);
            saveNext = false;
        }
        for (auto instr : instrs)
        {
            if (instr.contains("op") && (instr["op"] == "br" || instr["op"] == "jmp"))
            {
                for (auto child : instr["labels"])
                {
                    int childInt = labelNameToBlock_[child];
                    successors_[blockname].push_back(childInt);
                    predecessors_[childInt].push_back(blockname);
                }
                saveNext = false;
                break;
            }
            else if (instr.contains("op") && (instr["op"] == "call"))
            {
                for (auto child : instr["funcs"])
                {
                    int childInt = labelNameToBlock_[child];
                    if (find(successors_[blockname].begin(), successors_[blockname].end(), childInt) == successors_[blockname].end())
                    {
                        successors_[blockname].push_back(childInt);
                        predecessors_[childInt].push_back(blockname);
                    }
                }
                saveNext = true;
            }
            else if (instr.contains("op") && instr["op"] == "ret")
            {
                saveNext = false;
            }
            else
            {
                saveNext = true;
                prev = blockname;
            }
        }
    }
}

map<int, vector<json>> BasicBlocks::getBlocks()
{
    return blocks_;
}

vector<int> BasicBlocks::getPredecessors(int label)
{
    return predecessors_[label];
}

vector<int> BasicBlocks::getSuccessors(int label)
{
    return successors_[label];
}