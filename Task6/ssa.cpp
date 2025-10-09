#include "../utility/basicBlocks.hpp"
#include "../Task5/dominator.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <set>
#include <stack>
#include <vector>
#include <memory>

using namespace std;
using json = nlohmann::json;

// Global data structures for SSA conversion
map<string, set<int>> variableDefs;
map<int, vector<json>> phiNodes;
map<string, stack<string>> variableStack;
map<int, vector<json>> ssaInstructions;
int tempCounter = 0;

// Function declarations
void collectVariableDefinitions(shared_ptr<BasicBlocks> basicBlocks);
void insertPhiNodes(shared_ptr<BasicBlocks> basicBlocks,
                    const map<int, set<int>> &dominanceFrontier);
void renameVariables(shared_ptr<BasicBlocks> basicBlocks,
                     const map<int, set<int>> &dominators);
void renameBlock(int blockId, shared_ptr<BasicBlocks> basicBlocks,
                 const map<int, set<int>> &dominators);
void processBlockInstructions(int blockId, shared_ptr<BasicBlocks> basicBlocks);
void popBlockDefinitions(int blockId, shared_ptr<BasicBlocks> basicBlocks);
vector<int> getDominatedChildren(int blockId, const map<int, set<int>> &dominators);
void printSSA();

// Replace the previous write_back with this one
static inline void write_back_instrs_into_json(json &j) {
    // target standard bril2json shape
    json *fn = nullptr;
    if (j.contains("functions") && j["functions"].is_array() && !j["functions"].empty()) {
        fn = &j["functions"][0];
    } else if (j.contains("instrs") && j["instrs"].is_array()) {
        fn = &j; // fallback: single-function JSON
    } else {
        return; // nothing to do
    }

    // Deterministic order: map<int,...> iterates by ascending key (block id)
    json new_instrs = json::array();
    for (const auto &entry : ssaInstructions) {
        const auto &vec = entry.second;
        for (const auto &instr : vec) new_instrs.push_back(instr);
    }

    (*fn)["instrs"] = std::move(new_instrs);
}


// Main SSA conversion function
void convertToSSA(shared_ptr<BasicBlocks> basicBlocks)
{
    // cout << "Converting to SSA..." << endl;

    // Reset global state
    variableDefs.clear();
    phiNodes.clear();
    variableStack.clear();
    ssaInstructions.clear();
    tempCounter = 0;

    // Compute dominators and dominance frontier
    map<int, set<int>> dominators = findDominators(basicBlocks);
    map<int, set<int>> dominanceFrontier = findDominanceFrontier(basicBlocks, dominators);

    // cout << "\nPrinting dominance frontier:\n";
    // printDominaceFrontier(dominanceFrontier);

    // Perform SSA conversion steps
    collectVariableDefinitions(basicBlocks);
    insertPhiNodes(basicBlocks, dominanceFrontier);
    renameVariables(basicBlocks, dominators);

    // cout << "SSA conversion complete!" << endl;
}

void collectVariableDefinitions(shared_ptr<BasicBlocks> basicBlocks)
{
    auto blocks = basicBlocks->getBlocks();

    for (const auto &blockEntry : blocks)
    {
        int blockId = blockEntry.first;
        const vector<json> &instructions = blockEntry.second;

        for (const auto &instr : instructions)
        {
            if (instr.contains("dest"))
            {
                string var = instr["dest"];
                variableDefs[var].insert(blockId);
            }

            // Track all variables used
            if (instr.contains("args"))
            {
                for (const auto &arg : instr["args"])
                {
                    if (arg.is_string())
                    {
                        string var = arg;
                        if (variableDefs.find(var) == variableDefs.end())
                        {
                            variableDefs[var] = set<int>();
                        }
                    }
                }
            }
        }
    }
}

void insertPhiNodes(shared_ptr<BasicBlocks> basicBlocks,
                    const map<int, set<int>> &dominanceFrontier)
{
    for (const auto &varEntry : variableDefs)
    {
        const string &var = varEntry.first;
        const set<int> &defs = varEntry.second;

        set<int> worklist(defs.begin(), defs.end());
        set<int> processed;

        while (!worklist.empty())
        {
            int d = *worklist.begin();
            worklist.erase(worklist.begin());

            auto dfIt = dominanceFrontier.find(d);
            if (dfIt != dominanceFrontier.end())
            {
                const set<int> &dfSet = dfIt->second;
                for (int block : dfSet)
                {
                    if (processed.find(block) == processed.end())
                    {
                        // Create phi node as JSON
                        json phi;
                        phi["op"] = "phi";
                        phi["dest"] = var + "_" + to_string(tempCounter++);
                        phi["args"] = json::array();

                        // Initialize with empty args for each predecessor
                        auto preds = basicBlocks->getPredecessors(block);
                        for (size_t i = 0; i < preds.size(); i++)
                        {
                            phi["args"].push_back("");
                        }
                        phi["labels"] = preds;

                        phiNodes[block].push_back(phi);

                        if (variableDefs[var].find(block) == variableDefs[var].end())
                        {
                            variableDefs[var].insert(block);
                            worklist.insert(block);
                        }

                        processed.insert(block);
                    }
                }
            }
        }
    }
}

void renameVariables(shared_ptr<BasicBlocks> basicBlocks,
                     const map<int, set<int>> &dominators)
{
    // Initialize variable stacks with version 0
    for (const auto &varEntry : variableDefs)
    {
        variableStack[varEntry.first].push(varEntry.first + "_0");
    }

    renameBlock(basicBlocks->getMainLabel(), basicBlocks, dominators);
}

void renameBlock(int blockId, shared_ptr<BasicBlocks> basicBlocks,
                 const map<int, set<int>> &dominators)
{
    // Process phi-nodes in this block first
    auto phiIt = phiNodes.find(blockId);
    if (phiIt != phiNodes.end())
    {
        for (auto &phi : phiIt->second)
        {
            string var = phi["dest"].get<string>();
            size_t pos = var.find_last_of('_');
            string baseVar = var.substr(0, pos);

            // Push the phi destination as new version
            variableStack[baseVar].push(var);
            ssaInstructions[blockId].push_back(phi);
        }
    }

    // Process regular instructions
    processBlockInstructions(blockId, basicBlocks);

    // Fill in phi-node arguments in successors
    auto successors = basicBlocks->getSuccessors(blockId);
    for (int succ : successors)
    {
        auto succPhiIt = phiNodes.find(succ);
        if (succPhiIt != phiNodes.end())
        {
            for (auto &phi : succPhiIt->second)
            {
                string var = phi["dest"].get<string>();
                size_t pos = var.find_last_of('_');
                string baseVar = var.substr(0, pos);

                // Find our position in the predecessor list
                auto preds = basicBlocks->getPredecessors(succ);
                auto it = find(preds.begin(), preds.end(), blockId);
                if (it != preds.end())
                {
                    int predIndex = distance(preds.begin(), it);
                    if (predIndex < phi["args"].size())
                    {
                        phi["args"][predIndex] = variableStack[baseVar].top();
                    }
                }
            }
        }
    }

    // Process dominated children
    for (int child : getDominatedChildren(blockId, dominators))
    {
        renameBlock(child, basicBlocks, dominators);
    }

    // Pop variables defined in this block
    popBlockDefinitions(blockId, basicBlocks);
}

void processBlockInstructions(int blockId, shared_ptr<BasicBlocks> basicBlocks)
{
    auto blocks = basicBlocks->getBlocks();
    const vector<json> &instructions = blocks.at(blockId);

    for (const auto &instr : instructions)
    {
        if (!instr.contains("op"))
            continue;

        json newInstr = instr;

        // Rename arguments (uses)
        if (instr.contains("args"))
        {
            vector<string> newArgs;
            for (const auto &arg : instr["args"])
            {
                if (arg.is_string())
                {
                    string var = arg;
                    auto stackIt = variableStack.find(var);
                    if (stackIt != variableStack.end() && !stackIt->second.empty())
                    {
                        newArgs.push_back(stackIt->second.top());
                    }
                    else
                    {
                        newArgs.push_back(var + "_0");
                    }
                }
                else
                {
                    newArgs.push_back(arg);
                }
            }
            newInstr["args"] = newArgs;
        }

        // Rename destination (definition)
        if (instr.contains("dest"))
        {
            string var = instr["dest"];
            string newName = var + "_" + to_string(tempCounter++);
            newInstr["dest"] = newName;
            variableStack[var].push(newName);
        }

        ssaInstructions[blockId].push_back(newInstr);
    }
}

void popBlockDefinitions(int blockId, shared_ptr<BasicBlocks> basicBlocks)
{
    auto blocks = basicBlocks->getBlocks();
    const vector<json> &instructions = blocks.at(blockId);

    // Pop variables defined by regular instructions
    for (const auto &instr : instructions)
    {
        if (instr.contains("dest"))
        {
            string var = instr["dest"];
            auto stackIt = variableStack.find(var);
            if (stackIt != variableStack.end() && !stackIt->second.empty())
            {
                stackIt->second.pop();
            }
        }
    }

    // Pop variables defined by phi nodes
    auto phiIt = phiNodes.find(blockId);
    if (phiIt != phiNodes.end())
    {
        for (const auto &phi : phiIt->second)
        {
            string var = phi["dest"].get<string>();
            size_t pos = var.find_last_of('_');
            string baseVar = var.substr(0, pos);
            auto stackIt = variableStack.find(baseVar);
            if (stackIt != variableStack.end() && !stackIt->second.empty())
            {
                stackIt->second.pop();
            }
        }
    }
}

vector<int> getDominatedChildren(int blockId, const map<int, set<int>> &dominators)
{
    vector<int> children;
    for (const auto &domEntry : dominators)
    {
        int otherBlock = domEntry.first;
        if (otherBlock != blockId)
        {
            const set<int> &otherDominators = domEntry.second;
            if (otherDominators.find(blockId) != otherDominators.end())
            {
                bool isImmediate = true;
                for (int dom : otherDominators)
                {
                    if (dom != blockId && dom != otherBlock)
                    {
                        auto domIt = dominators.find(dom);
                        if (domIt != dominators.end())
                        {
                            const set<int> &domDominators = domIt->second;
                            if (otherDominators.find(dom) != otherDominators.end() &&
                                domDominators.find(blockId) != domDominators.end())
                            {
                                isImmediate = false;
                                break;
                            }
                        }
                    }
                }
                if (isImmediate)
                {
                    children.push_back(otherBlock);
                }
            }
        }
    }
    return children;
}

void printSSA()
{
    cout << "\n=== SSA Form ===" << endl;
    for (const auto &blockEntry : ssaInstructions)
    {
        int blockId = blockEntry.first;
        cout << "Block " << blockId << ":" << endl;

        for (const auto &instr : blockEntry.second)
        {
            if (instr["op"] == "phi")
            {
                cout << "  " << instr["dest"] << " = phi ";
                const auto &args = instr["args"];
                const auto &labels = instr["labels"];
                for (size_t i = 0; i < args.size(); ++i)
                {
                    cout << args[i] << " " << labels[i] << " ";
                }
                cout << endl;
            }
            else
            {
                cout << "  " << instr["op"];
                if (instr.contains("dest"))
                {
                    cout << " " << instr["dest"];
                }
                if (instr.contains("args"))
                {
                    cout << " ";
                    const auto &args = instr["args"];
                    for (size_t i = 0; i < args.size(); ++i)
                    {
                        if (i > 0)
                            cout << ", ";
                        cout << args[i];
                    }
                }
                cout << endl;
            }
        }
    }
}

json getSSAJson()
{
    json result = json::array();

    for (const auto &blockEntry : ssaInstructions)
    {
        for (const auto &instr : blockEntry.second)
        {
            result.push_back(instr);
        }
    }

    return result;
}

const map<int, vector<json>> &getSSABlocks()
{
    return ssaInstructions;
}

static inline bool isTerminator(const json &instr) {
    if (!instr.contains("op")) return false;
    const string op = instr["op"];
    return (op=="br" || op=="jmp" || op=="ret");
}

void insertCopyInPred(int predId, const string &dest, const string &src) {
    json copy;
    copy["op"] = "id";
    copy["dest"] = dest;
    copy["args"] = json::array({src});

    auto &vec = ssaInstructions[predId];
    int pos = (int)vec.size();
    while (pos>0 && isTerminator(vec[pos-1])) {
        --pos;
    }
    vec.insert(vec.begin() + pos, copy);
}

void convertFromSSA(shared_ptr<BasicBlocks> basicBlocks) {
    // cout << "\n===Converting out of SSA (eliminating the phis)==="<<endl;
    auto blocks = basicBlocks->getBlocks();

    for (auto &entry : ssaInstructions)
    {
        int blockId = entry.first;
        
        vector<int> phiIdx;
        for(int i=0; i<(int)entry.second.size(); ++i) {
            const json &instr = entry.second[i];
            if (instr.contains("op") && instr["op"] == "phi") {
                phiIdx.push_back(i);
            }
        }
        if (phiIdx.empty()) continue;

        for (int idx: phiIdx) {
            const json &phi = entry.second[idx];
            string dest = phi["dest"];
            const auto &args = phi["args"];
            const auto &labels = phi["labels"];

            for (size_t k=0; k<labels.size(); ++k) {
                int predId = labels[k];
                string src = args[k];
                insertCopyInPred(predId, dest, src);
            }
        }
        
    }

    //removing phi nodes
    for(auto &entry: ssaInstructions) {
        auto &vec = entry.second;
        vector<json> cleaned;
        cleaned.reserve(vec.size());
        for(auto &instr: vec) {
            if (instr.contains("op") && instr["op"] == "phi") {

            }
            else {
                cleaned.push_back(instr);
            }
        }
        vec.swap(cleaned);
    }
}

int main(int argc, char** argv) //char *argv[])
{
    ios::sync_with_stdio(false);

    bool to_ssa = false; 
    bool from_ssa = false;
    bool round_trip = false;

    for (int i = 1; i < argc; ++i) {
        string a = argv[i];
        if (a == "--round-trip") {round_trip = true;}
        else if (a == "--to-ssa") {to_ssa = true; }
        else if (a == "--from-ssa") {from_ssa = true;}
    }

    json j;
    cin >> j;

    shared_ptr<BasicBlocks> basicBlocks = make_shared<BasicBlocks>(j);

    if (to_ssa) {
        convertToSSA(basicBlocks);
        // printSSA();
        write_back_instrs_into_json(j);

        // Output the SSA as JSON
        // json ssaJson = getSSAJson();
        // cout << "\n=== SSA JSON ===" << endl;
        cout << j.dump(2) << endl;
        return 0;
    }
    else if (round_trip) {
        convertToSSA(basicBlocks);
        // json ssaJson = getSSAJson();
        write_back_instrs_into_json(j);
        cout << "\n=== SSA JSON ===" << endl;
        cout << j.dump(2) << endl;

        convertFromSSA(basicBlocks);
        // json nonSsaJson = getSSAJson();
        write_back_instrs_into_json(j);
        cout << "\n=== Final JSON (no phis) ===\n";
        cout << j.dump(2) << endl;
    }

    else if (from_ssa){
        convertToSSA(basicBlocks);
        // json ssaJson = getSSAJson();
        write_back_instrs_into_json(j);

        convertFromSSA(basicBlocks);
        write_back_instrs_into_json(j);
        // json nonSsaJson = getSSAJson();
        // cout << "\n=== Final JSON (no phis) ===\n";
        cout << j.dump(2) << endl;
    }

    return 0;
}