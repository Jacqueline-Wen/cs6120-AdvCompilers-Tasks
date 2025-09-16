#include <iostream>
#include <fstream>
#include <set>
#include <nlohmann/json.hpp>
#include "seperateBlocks.hpp"

using namespace std;
using json = nlohmann::json;

int main(int argc, char* argv[]) {
    std::ifstream f(argv[1]);
    json j = json::parse(f);

    auto blocks = splitBlocks(j);

    cout << "finished splitting\n";

    for (auto a: blocks) { // single block
        for (auto b: a) { // single line
            cout << b.dump(4) << endl;
        }
        cout << "______\n";
    }
}