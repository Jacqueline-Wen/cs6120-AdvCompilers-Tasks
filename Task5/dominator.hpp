#ifndef DOMINATOR_ANALYSIS_HPP
#define DOMINATOR_ANALYSIS_HPP

#include "../utility/basicBlocks.hpp" // Assuming this contains BasicBlocks definition
#include <map>
#include <set>
#include <memory>   // For shared_ptr
#include <iostream> // For print functions (optional, but useful for testing)

using namespace std;

// Forward declarations to use BasicBlocks, although its definition is needed for shared_ptr
// If BasicBlocks is always included via "../utility/basicBlocks.hpp", this might be sufficient.

/**
 * @brief Prints the dominator sets for all blocks.
 *
 * @param dom A map where keys are block labels and values are sets of dominator block labels.
 */
void printDominators(const map<int, set<int>> &dom);

/**
 * @brief Prints the dominance frontier for all blocks.
 *
 * @param dom A map where keys are block labels and values are sets of dominance frontier block labels.
 */
void printDominaceFrontier(const map<int, set<int>> &dom);

/**
 * @brief Computes the dominance frontier for each basic block.
 *
 * @param basicBlocks A shared pointer to the BasicBlocks structure of the control flow graph.
 * @param dom The pre-computed dominator map.
 * @return A map where keys are block labels and values are their dominance frontier sets.
 */
map<int, set<int>> findDominanceFrontier(shared_ptr<BasicBlocks> basicBlocks, const map<int, set<int>> &dom);

/**
 * @brief Computes the immediate dominator for each basic block from the full dominator map.
 *
 * @param dom The pre-computed dominator map.
 * @return A map where keys are block labels and values are sets containing their immediate dominator (should be a single element set).
 */
map<int, set<int>> findImmediateDominators(const map<int, set<int>> &dom);

/**
 * @brief Computes the dominator set for each basic block using the iterative algorithm.
 *
 * @param basicBlocks A shared pointer to the BasicBlocks structure of the control flow graph.
 * @return A map where keys are block labels and values are their dominator sets.
 */
map<int, set<int>> findDominators(shared_ptr<BasicBlocks> basicBlocks);

#endif // DOMINATOR_ANALYSIS_HPP