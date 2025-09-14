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

bool localValueNumbering(json &j)
{
    bool changed = false;
    map<vector<int>, pair<int, string>> table;
    map<string, int> var2num;
    std::set<std::string> commutative_ops = {"add", "mul", "eq", "ne", "and", "or"};
    for (auto &function : j["functions"])
    {
        int nextNum = 1;

        auto &instrs = function["instrs"];
        for (auto &instr : instrs)
        {
            // Building the value tuple
            vector<int> value;
            string op = instr["op"];
            value.push_back(hash<string>{}(op));
            if (instr["op"] == "const")
            {
                value.push_back(instr["value"]);
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
                    {"dest", instr["dest"]}};
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
                                    break;
                                }
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

    std::cout << j.dump(4) << std::endl;
    return 0;
}