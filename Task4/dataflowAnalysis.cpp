#include <iostream>
#include <fstream>
#include <set>
#include <nlohmann/json.hpp>
#include "../utility/seperateBlocks.hpp"

using namespace std;
using json = nlohmann::json;

void liveVariables() {
    
}

int main(int argc, char* argv[]) {
    std::ifstream f(argv[1]);
    json j = json::parse(f);

    auto blocks = splitBlocks(j);
}
