#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <set>
#include <vector>
#pragma once

using json = nlohmann::json;
using namespace std;

vector<vector<json>> splitBlocks(json &j) {
  map<string, vector<json>> basic_blocks;
  vector<string> labels;

  string curLabel = "main";
  labels.push_back(curLabel);

  queue<json> curBlock;
  for (auto instr : j["instrs"]) {
    // end block
    if (!instr["op"].is_null() &&
        (instr["op"] == "br" || instr["op"] == "jmp" || instr["op"] == "ret")) {
      curBlock.push(instr);

      while (curBlock.size() > 0) {
        basic_blocks[curLabel].push_back(curBlock.front());
        curBlock.pop();
      }
    } else if (!instr["label"].is_null()) {
      while (curBlock.size() > 0) {
        basic_blocks[curLabel].push_back(curBlock.front());
        curBlock.pop();
      }
      curBlock.push(instr);
      curLabel = instr["label"];
      labels.push_back(curLabel);
    } else {
      curBlock.push(instr);
    }
  }
  while (curBlock.size() > 0) {
    basic_blocks[curLabel].push_back(curBlock.front());
    curBlock.pop();
  }

  vector<vector<json>> blocks;
  for (auto a : labels) {
    blocks.push_back(basic_blocks[a]);
  }
  return blocks;
}