#include "basicBlocks.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <set>

using namespace std;
using json = nlohmann::json;

int main(int argc, char *argv[]) {
    // std::ifstream f(argv[1]);
    // json j = json::parse(f);
    json j;
    cin >> j;

    shared_ptr<BasicBlocks> basicBlocks = make_shared<BasicBlocks>(j);

    cout << "finished splitting\n";

    for (auto a : basicBlocks->getBlocks()) { // single block
        cout << "for label " << a.first << endl;
        for (auto b : a.second) { // single line
            cout << b.dump(4) << endl;
        }
        cout << "______\n";
    }

    for (auto a : basicBlocks->getBlocks()) {
        cout << "label " << a.first << endl;
        for (auto b : basicBlocks->getSuccessors(a.first)) {
            cout << b << endl;
        }
    }

    cout << "predecessors\n";

    for (auto a : basicBlocks->getBlocks()) {
        cout << "label " << a.first << endl;
        for (auto b : basicBlocks->getPredecessors(a.first)) {
            cout << b << endl;
        }
    }
}