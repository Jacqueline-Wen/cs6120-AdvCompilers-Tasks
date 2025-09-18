#include <iostream>
#include <fstream>
#include <queue>
#include <set>
#include <nlohmann/json.hpp>
#include "../utility/seperateBlocks.hpp"

using namespace std;
using json = nlohmann::json;

void reachingDefinitions(map<string, vector<json>> blocks) {
    map<string, vector<string>> blockIn;
    map<string, vector<string>> blockOut;

    queue<string> q;
    for (auto a: blocks) {
        q.push(a.first);
    }
    while (q.size() > 0) {
        string curLabel = q.front();
        q.pop();
        
    }

}

int main(int argc, char* argv[]) {
    std::ifstream f(argv[1]);
    json j = json::parse(f);

    map<string, vector<json>> blocks = splitBlocks(j);


}
