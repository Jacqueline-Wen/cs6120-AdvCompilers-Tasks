#include <iostream>
#include <fstream>
#include <set>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

// counting the number of different arguments used
int main(int argc, char* argv[]) {
    std::ifstream f(argv[1]);
    json j = json::parse(f);

    set<string>args;
    
    for (auto a: j["functions"]) {
        for (auto b: a["instrs"]) {
            for (auto c: b["args"]) {
                args.insert(c);
            }
        }
    }

    cout << "there are " << args.size() << " different arguments in this code\n";
}