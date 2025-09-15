#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

void printVar2Num(const std::map<std::string, int> &var2num)
{
    std::cout << "Printing var2num map:" << std::endl;
    for (const auto &pair : var2num)
    {
        std::cout << "  Key: " << pair.first << ", Value: " << pair.second << std::endl;
    }
}

void printTable(const std::map<std::vector<int>, std::pair<int, std::string>> &table)
{
    std::cout << "Printing table map:" << std::endl;
    for (const auto &entry : table)
    {
        // Print the key (the vector<int>)
        std::cout << "  Key Vector: {";
        for (size_t i = 0; i < entry.first.size(); ++i)
        {
            std::cout << entry.first[i];
            if (i < entry.first.size() - 1)
            {
                std::cout << ", ";
            }
        }
        std::cout << "}";

        // Print the value (the pair<int, string>)
        std::cout << ", Value Pair: (" << entry.second.first << ", \"" << entry.second.second << "\")" << std::endl;
    }
}

int constant_folding_int(const std::string &op, const std::vector<int> &args, bool &success)
{
    if (args.size() != 2)
    {
        success = false;
        return 0;
    }
    if (op == "add")
    {
        return args[0] + args[1];
    }
    else if (op == "sub")
    {
        return args[0] - args[1];
    }
    else if (op == "mul")
    {
        return args[0] * args[1];
    }
    else if (op == "div" && args[1] != 0)
    {
        return args[0] / args[1];
    }
    success = false;
    return 0;
}

bool constant_folding_bool(const std::string &op, const std::vector<int> &args, bool &success)
{
    if (op == "not" && args.size() == 1)
    {
        return (args[0] == 0) ? true : false;
    }
    if (args.size() != 2)
    {
        success = false;
        return false;
    }
    if (op == "eq")
    {
        return (args[0] == args[1]) ? true : false;
    }
    else if (op == "ne")
    {
        return (args[0] != args[1]) ? true : false;
    }
    else if (op == "lt")
    {
        return (args[0] < args[1]) ? true : false;
    }
    else if (op == "le")
    {
        return (args[0] <= args[1]) ? true : false;
    }
    else if (op == "gt")
    {
        return (args[0] > args[1]) ? true : false;
    }
    else if (op == "ge")
    {
        return (args[0] >= args[1]) ? true : false;
    }
    else if (op == "and")
    {
        return (args[0] && args[1]) ? true : false;
    }
    else if (op == "or")
    {
        return (args[0] || args[1]) ? true : false;
    }
    success = false;
    return 0;
}

bool localValueNumbering(json &j)
{
    bool changed = false;
    map<vector<int>, pair<int, string>> table;
    map<string, int> var2num;
    map<string, int> const2var; // map from const value (string) to var num (int)
    std::set<std::string> commutative_ops = {"add", "mul", "eq", "ne", "and", "or"};
    for (auto &function : j["functions"])
    {
        int nextNum = 1;

        auto &instrs = function["instrs"];
        for (auto &instr : instrs)
        {
            // Building the value tuple
            vector<int> value;
            if (instr["op"].is_null()){
                continue;
            }
            string op = instr["op"];
            value.push_back(hash<string>{}(op));
            if (instr["op"] == "const")
            {
                value.push_back(instr["value"]);
                const2var[instr["dest"]] = instr["value"];
            }

            if (instr.contains("args"))
            {
                if (commutative_ops.count(op))
                {
                    // For commutative ops with two args, sort the arg nums
                    if (instr["args"][1] < instr["args"][0])
                    {
                        std::swap(instr["args"][1], instr["args"][0]);
                    }
                }
                for (auto &a : instr["args"])
                {
                    string var = a;
                    int num = var2num.count(var) ? var2num[var] : -1;
                    value.push_back(num);
                }
            }

            if (instr["op"] == "id") // copy propagation
            {
                string arg = instr["args"][0];
                string dest = instr["dest"];
                var2num[dest] = var2num[arg];
                int tempNum = var2num[arg];
                for (auto &kv : table)
                {
                    if (kv.second.first == tempNum)
                    {
                        instr["args"][0] = kv.second.second;
                        break;
                    }
                }
            }
            else if (table.count(value)) // instruction already exists
            {
                int num = table[value].first;
                string var = table[value].second;

                instr = {
                    {"op", "id"},
                    {"args", json::array({var})},
                    {"dest", instr["dest"]},
                    {"type", instr["type"]}};
                var2num[instr["dest"]] = num;
                changed = true;
            }
            else
            {
                int num = nextNum++;
                string dest = instr.contains("dest") ? string(instr["dest"]) : "";
                table[value] = {num, dest};

                if (instr.contains("args"))
                {
                    std::vector<int> argVals;
                    for (auto &a : instr["args"])
                    {
                        string var = a;
                        if (var2num.count(var))
                        {
                            int tempNum = var2num[var];
                            for (auto &kv : table)
                            {
                                if (kv.second.first == tempNum)
                                {
                                    a = kv.second.second;
                                    if (const2var.count(a))
                                    {
                                        argVals.push_back(const2var[a]);
                                    }
                                    break;
                                }
                            }
                        }
                    }
                    if (argVals.size() == instr["args"].size()) // constant folding
                    {
                        bool success = true;
                        if (instr["op"] == "add" || instr["op"] == "sub" || instr["op"] == "mul" || instr["op"] == "div")
                        {
                            int folded = constant_folding_int(op, argVals, success);
                            if (success)
                            {
                                instr = {
                                    {"op", "const"},
                                    {"value", folded},
                                    {"dest", instr["dest"]},
                                    {"type", instr["type"]}};
                                var2num[instr["dest"]] = num;
                                const2var[instr["dest"]] = folded;
                                changed = true;
                            }
                        }
                        else
                        {
                            auto folded = constant_folding_bool(op, argVals, success);
                            if (success)
                            {
                                instr = {
                                    {"op", "const"},
                                    {"value", folded},
                                    {"dest", instr["dest"]},
                                    {"type", instr["type"]}};
                                var2num[instr["dest"]] = num;
                                const2var[instr["dest"]] = folded;
                                changed = true;
                            }
                        }
                    }
                }
                if (!dest.empty())
                {
                    var2num[dest] = num;
                }
            }
        }
    }
    // printTable(table);
    // printVar2Num(var2num);

    return changed;
}

int main(int argc, char *argv[])
{
    // std::ifstream f(argv[1]);
    // json j = json::parse(f);
    json j;
    std::cin >> j;

    bool notConverged = true;
    while (notConverged)
    {
        notConverged = localValueNumbering(j);
    }

    std::cout << j << std::endl;
    return 0;
}