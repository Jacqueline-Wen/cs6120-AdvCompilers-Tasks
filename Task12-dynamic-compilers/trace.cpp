#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <stack>
#include <map>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <vector>

using namespace std;
using json = nlohmann::json;

struct Trace
{
    vector<json> trace;
    string lastLabel;
    int lastOffset;
};

// process raw trace file
Trace getTrace(string trace_f, unordered_map<string, vector<json>> func_args)
{
    Trace t;
    auto &trace = t.trace;

    // open trace
    ifstream trace_raw(trace_f);
    if (!trace_raw)
    {
        cerr << "ERROR: Could not open trace " << trace_f << endl;
        return t;
    }

    string line;
    stack<pair<string, string>> ret_dest;
    while (getline(trace_raw, line))
    {
        try
        {
            json cur_line = nlohmann::json::parse(line);
            if (cur_line.contains("op"))
            {
                // convert branches to guards
                auto cur_op = cur_line["op"];
                if (cur_op == "br")
                {
                    getline(trace_raw, line);

                    string cond = cur_line["args"][0];
                    if (line == "false")
                    {
                        // construct negated cond
                        auto neg_cond = json{
                            {"args", {cond}},
                            {"dest", "temp"},
                            {"op", "not"},
                            {"type", "bool"}};
                        trace.push_back(neg_cond);
                        cond = "temp";
                    }

                    // push back guard
                    auto guard = json{
                        {"args", {cond}},
                        {"labels", {"guard_failed"}},
                        {"op", "guard"}};
                    trace.push_back(guard);
                }
                else if (cur_op == "ret")
                {
                    auto ret_data = ret_dest.top();
                    ret_dest.pop();
                    auto ret_id = json{
                        {"args", {cur_line["args"][0]}},
                        {"dest", ret_data.first},
                        {"op", "id"},
                        {"type", ret_data.second}};
                    trace.push_back(ret_id);
                }
                else if (cur_op == "call")
                {
                    auto func_name = cur_line["funcs"][0];
                    for (int i = 0; i < cur_line["args"].size(); i++)
                    {
                        auto arg = cur_line["args"][i];
                        auto param = func_args[func_name][i];
                        auto arg_id = json{
                            {"args", {arg}},
                            {"dest", param["name"]},
                            {"op", "id"},
                            {"type", param["type"]}};
                        trace.push_back(arg_id);
                    }
                    ret_dest.push(make_pair(cur_line["dest"], cur_line["type"]));
                }
                else if (cur_op != "jmp")
                {
                    // remove jumps and rets and calls
                    trace.push_back(cur_line);
                }
            }
        }
        catch (const nlohmann::json::parse_error &e)
        {
            // end of trace
            t.lastLabel = line;
            getline(trace_raw, line);
            t.lastOffset = stoi(line);
        }
    }

    // insert speculate, commit, jmp to .trace_success, .guard_failed
    auto speculate = json{
        {"op", "speculate"},
    };
    auto commit = json{
        {"op", "commit"},
    };
    auto jmp_trace_sucess = json{
        {"labels", {"trace_sucess"}},
        {"op", "jmp"},
    };
    auto guard_failed = json{
        {"label", "guard_failed"},
    };
    trace.insert(trace.begin(), speculate);
    trace.push_back(commit);
    trace.push_back(jmp_trace_sucess);
    trace.push_back(guard_failed);

    trace_raw.close();

    return t;
}

// print trace
void printTrace(Trace t)
{
    cout << "\n--- GOT TRACE ---" << endl;
    for (auto &cur : t.trace)
    {
        cout << cur << endl;
    }
    cout << "last data: " << t.lastLabel << " " << t.lastOffset << "\n"
         << endl;
}

// 💥 MODIFIED: No CFG required 💥
void insertTrace(json &func, Trace t)
{
    // The instructions of the main function
    auto &original_instrs = func["instrs"];
    auto &trace = t.trace;

    // 1. Insert hot path (t.trace) at the beginning of the function
    auto &instrs_vec = original_instrs.get_ref<std::vector<json> &>();
    instrs_vec.insert(instrs_vec.begin(), trace.begin(), trace.end());

    // 2. Insert .trace_sucess label at the original trace exit point.
    // We must scan the instructions to find the label t.lastLabel.
    int current_instr_idx = 0;
    bool label_found = false;
    int target_idx = -1;

    // The instruction list now includes the trace, so we must skip the trace instructions.
    // The original instructions start at index t.trace.size().
    for (size_t i = trace.size(); i < original_instrs.size(); ++i)
    {
        auto &instr = original_instrs[i];

        // Check for the target label
        if (instr.contains("label") && instr["label"] == t.lastLabel)
        {
            label_found = true;
            current_instr_idx = 0; // Reset offset for the block
        }

        if (label_found && current_instr_idx == t.lastOffset)
        {
            // The instruction *after* the target instruction is at index i + 1
            target_idx = i + 1;
            break;
        }

        // Only count instructions, not labels
        if (!instr.contains("label"))
        {
            current_instr_idx++;
        }
    }

    if (target_idx != -1)
    {
        auto trace_sucess = json{
            {"label", "trace_sucess"},
        };
        // Insert the .trace_sucess label at the index immediately following
        // the original instruction where the trace exited.
        original_instrs.insert(original_instrs.begin() + target_idx, trace_sucess);
    }
    // Note: If label is not found, the program will simply not insert the .trace_sucess label,
    // which might lead to incorrect program flow if the trace was valid.
}

int main(int argc, char *argv[])
{
    auto json_file = argv[1];
    auto trace_file = argv[2];

    json j;
    try
    {
        ifstream input(json_file);
        if (!input)
        {
            cerr << "ERROR: Could not open file " << json_file << "\n";
            return 1;
        }
        input >> j;
    }
    catch (const json::parse_error &e)
    {
        cerr << "ERROR: Failed to parse Bril JSON from file, " << e.what() << endl;
        return 1;
    }

    // get function args
    unordered_map<string, vector<json>> func_args;
    for (auto &func : j["functions"])
    {
        string name = func["name"];
        for (auto arg : func["args"])
        {
            func_args[name].push_back(arg);
        }
    }

    // process trace
    auto t = getTrace(trace_file, func_args);
    // printTrace(t);

    // stitch in trace
    for (auto &func : j["functions"])
    {
        if (func["name"] == "main")
        {
            insertTrace(func, t);
        }
    }

    cout << j;

    return 0;
}