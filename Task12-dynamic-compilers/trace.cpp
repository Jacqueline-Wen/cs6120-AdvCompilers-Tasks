#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;
using json = nlohmann::json;

static void pushIdOp(vector<json> &trace, const json &src, const string &dest,
                     const string &type) {
    trace.push_back(
        {{"op", "id"}, {"args", {src}}, {"dest", dest}, {"type", type}});
}

static void handleBranch(ifstream &trace_raw, vector<json> &trace,
                         const json &cur_line) {
    string next_line;
    getline(trace_raw, next_line);

    string cond = cur_line["args"][0];

    if (next_line == "false") {
        json neg = {{"op", "not"},
                    {"args", {cond}},
                    {"dest", "temp"},
                    {"type", "bool"}};
        trace.push_back(neg);
        cond = "temp";
    }

    trace.push_back(
        {{"op", "guard"}, {"args", {cond}}, {"labels", {"guard_failed"}}});
}

static void handleReturn(vector<json> &trace,
                         stack<pair<string, string>> &return_stack,
                         const json &cur_line) {
    auto ret_info = return_stack.top();
    return_stack.pop();

    pushIdOp(trace, cur_line["args"][0], ret_info.first, ret_info.second);
}

static void handleCall(vector<json> &trace,
                       stack<pair<string, string>> &return_stack,
                       const json &cur_line,
                       const unordered_map<string, vector<json>> &func_args) {
    string func_name = cur_line["funcs"][0];
    const auto &params = func_args.at(func_name);

    for (size_t i = 0; i < cur_line["args"].size(); ++i) {
        const json &param = params[i];
        pushIdOp(trace, cur_line["args"][i], param["name"], param["type"]);
    }

    return_stack.emplace(cur_line["dest"], cur_line["type"]);
}

vector<json> getTrace(const string &trace_f,
                      const unordered_map<string, vector<json>> &func_args) {
    vector<json> trace;
    ifstream trace_raw(trace_f);
    if (!trace_raw.is_open())
        return trace;

    string line;
    stack<pair<string, string>> return_stack;

    while (getline(trace_raw, line)) {
        json cur_line;

        try {
            cur_line = json::parse(line);
        } catch (const nlohmann::json::parse_error &) {
            getline(trace_raw, line);
            continue;
        }

        if (!cur_line.contains("op"))
            continue;
        string op = cur_line["op"];

        if (op == "br") {
            handleBranch(trace_raw, trace, cur_line);
        } else if (op == "ret") {
            handleReturn(trace, return_stack, cur_line);
        } else if (op == "call") {
            handleCall(trace, return_stack, cur_line, func_args);
        } else if (op != "jmp") {
            trace.push_back(cur_line);
        }
    }

    trace.insert(trace.begin(), json{{"op", "speculate"}});
    trace.push_back({{"op", "commit"}});
    trace.push_back({{"op", "jmp"}, {"labels", {"trace_sucess"}}});
    trace.push_back({{"label", "guard_failed"}});

    return trace;
}

void insertTrace(json &func, vector<json> &trace) {
    auto &original_instrs = func["instrs"];

    auto &instrs_vec = original_instrs.get_ref<std::vector<json> &>();
    instrs_vec.insert(instrs_vec.begin(), trace.begin(), trace.end());

    json trace_success = {{"label", "trace_sucess"}};

    instrs_vec.insert(instrs_vec.begin() + trace.size(), trace_success);
}

int main(int argc, char *argv[]) {
    auto json_file = argv[1];
    auto trace_file = argv[2];

    json j;
    ifstream input(json_file);
    if (!input) {
        cerr << "ERROR - Unable to open:" << json_file << "\n";
        return 1;
    }
    try {
        input >> j;
    } catch (const json::parse_error &e) {
        cerr << "ERROR - Unable to parse: " << e.what() << endl;
        return 1;
    }

    unordered_map<string, vector<json>> func_args;
    for (auto &func : j["functions"]) {
        string name = func["name"];
        for (auto arg : func["args"]) {
            func_args[name].push_back(arg);
        }
    }

    auto t = getTrace(trace_file, func_args);

    for (auto &func : j["functions"]) {
        if (func["name"] == "main") {
            insertTrace(func, t);
        }
    }

    cout << j.dump(4);
    return 0;
}