#ifndef ASTAR_SPELLCHECK_H
#define ASTAR_SPELLCHECK_H

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_set>
#include <functional>
#include "trie.h"

using namespace std;

// State for A* search in the Trie
struct AStarState {
    TrieNode* node;           // Current position in Trie
    string currentWord;       // Word built so far
    int targetIndex;          // Position in target word
    int gCost;                // Actual cost (edit distance so far)
    int fCost;                // f = g + h (total estimated cost)
    
    // For priority queue (min-heap based on fCost)
    bool operator>(const AStarState& other) const {
        return fCost > other.fCost;
    }
};

class AStarSpellChecker {
private:
    Trie* trie;
    
    // Heuristic: estimate remaining edit distance
    // Uses the difference in remaining characters as lower bound
    int heuristic(const string& current, int targetIndex, const string& target);
    
    // Calculate Levenshtein distance between two strings (for verification)
    int levenshteinDistance(const string& s1, const string& s2);

public:
    AStarSpellChecker(Trie* t);
    
    // Find similar words using A* search with Levenshtein distance as cost
    // Returns words within maxDist edit distance, ordered by distance
    vector<pair<int, string>> findSimilarWords(const string& target, int maxDist);
    
    // Find the single best match
    string findBestMatch(const string& target, int maxDist);
    
    // Check if exact word exists
    bool wordExists(const string& word);
};

#endif // ASTAR_SPELLCHECK_H
