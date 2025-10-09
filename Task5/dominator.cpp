#include "../utility/basicBlocks.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <set>

using namespace std;
using json = nlohmann::json;

void printDominators(const map<int, set<int>> &dom) {
    for (const auto &pair : dom) {
        int blockLabel = pair.first;
        cout << "Block " << blockLabel << " is dominated by: ";
        for (int dominator : pair.second) {
            cout << dominator << " ";
        }
        cout << endl;
    }
}

void printDominaceFrontier(const map<int, set<int>> &dom) {
    for (const auto &pair : dom) {
        int blockLabel = pair.first;
        cout << "Block " << blockLabel << " dominance frontier: ";
        for (int dominator : pair.second) {
            cout << dominator << " ";
        }
        cout << endl;
    }
}

map<int, set<int>> findDominanceFrontier(shared_ptr<BasicBlocks> basicBlocks,
                                         const map<int, set<int>> &dom) {
    map<int, set<int>> dominanceFrontiers;
    for (const auto &entry : dom) {
        dominanceFrontiers[entry.first] = {};
    }
    map<int, set<int>> domReverse;
    for (auto &node : dom) {
        for (auto &dominatedBy : node.second) {
            domReverse[dominatedBy].insert(node.first);
        }
    }

    for (auto &node : domReverse) {
        auto dominating =
            node.second; // all the blocks that the node.first is dominating
        for (auto &dominate : node.second) {
            for (auto &successor : basicBlocks->getSuccessors(dominate)) {
                if (!dominating.count(successor)) {
                    dominanceFrontiers[node.first].insert(successor);
                }
            }
        }
    }
    return dominanceFrontiers;
}

map<int, set<int>> findImmediateDominators(const map<int, set<int>> &dom) {
    map<int, set<int>> immediateDominators = dom;
    for (auto &node : immediateDominators) {
        set<int> successorsSuccessors;
        for (auto &successor : node.second) {
            if (successor == node.first)
                continue;
            for (auto ss : dom.at(successor)) {
                if (ss != successor)
                    successorsSuccessors.insert(ss);
            }
        }
        successorsSuccessors.insert(node.first);
        for (auto a : successorsSuccessors) {
            immediateDominators[node.first].erase(a);
        }
    }
    return immediateDominators;
}

map<int, set<int>> findDominators(shared_ptr<BasicBlocks> basicBlocks) {
    auto blocks = basicBlocks->getBlocks();
    map<int, set<int>> dom;
    if (blocks.empty()) {
        return dom;
    }
    int entry = basicBlocks->getMainLabel();

    // dom = {every block -> all blocks}
    set<int> allBlocks;
    for (const auto &block : blocks) {
        allBlocks.insert(block.first);
    }
    for (const auto &block : blocks) {
        int blockLabel = block.first;
        dom[blockLabel] = allBlocks;
    }

    // dom[entry] = {entry}
    dom[entry] = {entry};
    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto &block : blocks) {
            int blockLabel = block.first;
            if (blockLabel == entry) {
                continue;
            }

            // dom[vertex] = {vertex} ∪ ⋂(dom[p] for p in vertex.preds}
            set<int> oldDom = dom[blockLabel];
            vector<int> preds = basicBlocks->getPredecessors(blockLabel);
            set<int> newDom;
            if (!preds.empty()) {
                newDom = dom[preds[0]];
                for (size_t i = 1; i < preds.size(); i++) {
                    set<int> temp;
                    int pred = preds[i];
                    for (int d : dom[pred]) {
                        if (newDom.find(d) != newDom.end()) {
                            temp.insert(d);
                        }
                    }
                    newDom = std::move(temp);
                }
            }
            newDom.insert(blockLabel);
            if (newDom != oldDom) {
                dom[blockLabel] = newDom;
                changed = true;
            }
        }
    }
    return dom;
}

#ifndef DOMINATOR_LIB
int main(int argc, char *argv[]) {
    json j;
    cin >> j;

    shared_ptr<BasicBlocks> basicBlocks = make_shared<BasicBlocks>(j);
    map<int, set<int>> dominators = findDominators(basicBlocks);

    cout << "printing dominators\n";
    printDominators(dominators);

    map<int, set<int>> immediateDominators =
        findImmediateDominators(dominators);
    cout << "\nprinting immediate dominators\n";
    printDominators(immediateDominators);

    map<int, set<int>> dominanceFrontier =
        findDominanceFrontier(basicBlocks, dominators);
    cout << "\nprinting dominance frontier\n";
    printDominaceFrontier(dominanceFrontier);
}
#endif
