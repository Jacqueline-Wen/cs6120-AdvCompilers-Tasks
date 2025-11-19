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
            json cur_line = json::parse(line);

            if (!cur_line.contains("op")) {
                continue;
            }

            auto cur_op = cur_line["op"];

            if (cur_op == "br")
            {
                getline(trace_raw, line);
                string cond = cur_line["args"][0];

                if (line == "false")
                {
                    json neg = {
                        {"op", "not"},
                        {"args", {cond}},
                        {"dest", "temp"},
                        {"type", "bool"}
                    };
                    trace.push_back(neg);
                    cond = "temp";
                }

                json guard = {
                    {"op", "guard"},
                    {"args", {cond}},
                    {"labels", {"guard_failed"}},
                };
                trace.push_back(guard);
            }
            else if (cur_op == "ret")
            {
                auto ret_data = ret_dest.top();
                ret_dest.pop();
                json ret_id = {
                    {"op", "id"},
                    {"args", {cur_line["args"][0]}},
                    {"dest", ret_data.first},
                    {"type", ret_data.second}
                };
                trace.push_back(ret_id);
            }
            else if (cur_op == "call")
            {
                auto func_name = cur_line["funcs"][0];
                for (int i = 0; i < cur_line["args"].size(); i++)
                {
                    auto param = func_args[func_name][i];
                    json arg_id = {
                        {"op", "id"},
                        {"args", {cur_line["args"][i]}},
                        {"dest", param["name"]},
                        {"type", param["type"]}};
                    trace.push_back(arg_id);
                    }
                ret_dest.push(make_pair(cur_line["dest"], cur_line["type"]));
            }
            else if (cur_op != "jmp")
            {
                trace.push_back(cur_line);
            }
        }
        catch (const nlohmann::json::parse_error &)
        {
            t.lastLabel = line;
            getline(trace_raw, line);
            t.lastOffset = stoi(line);
        }
    }

    // inserting them all
    json speculate = { {"op", "speculate"} };
    trace.insert(trace.begin(), speculate);
    json commit = { {"op", "commit"} };
    trace.push_back(commit);
    json jmp_trace_sucess = {
        {"op", "jmp"},
        {"labels", {"trace_sucess"}}
    };
    trace.push_back(jmp_trace_sucess);
    json guard_failed = { {"label", "guard_failed"} };
    trace.push_back(guard_failed);

    trace_raw.close();
    return t;
}

void printTrace(Trace t)
{
    cout << "\nGOT TRACE:" << endl;
    for (auto &cur : t.trace)
    {
        cout << cur << endl;
    }
    cout << "last data: " << t.lastLabel << " " << t.lastOffset << "\n"
         << endl;
}

void insertTrace(json &func, Trace t)
{
    auto &original_instrs = func["instrs"];
    auto &trace = t.trace;

    // 1. Insert hot path (t.trace) at the beginning of the function
    auto &instrs_vec = original_instrs.get_ref<std::vector<json> &>();
    instrs_vec.insert(instrs_vec.begin(), trace.begin(), trace.end());

    // 2. SIMPLE VERSION: Always insert trace_sucess label right after the trace.
    json trace_success = {
        {"label", "trace_sucess"}
    };

    instrs_vec.insert(instrs_vec.begin() + trace.size(), trace_success);
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

    // trace logic
    auto t = getTrace(trace_file, func_args);

    // put the trace in
    for (auto &func : j["functions"])
    {
        if (func["name"] == "main")
        {
            insertTrace(func, t);
        }
    }

    cout << j.dump(4);
    return 0;
}