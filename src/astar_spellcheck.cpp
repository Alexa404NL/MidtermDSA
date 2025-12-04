#include "../include/astar_spellcheck.h"
#include <omp.h>

AStarSpellChecker::AStarSpellChecker(Trie* t) : trie(t) {}

int AStarSpellChecker::heuristic(const string& current, int targetIndex, const string& target) {
    // Admissible heuristic: minimum possible remaining edit distance
    // At minimum, we need |remaining target chars - remaining word length| edits
    int remainingTarget = target.length() - targetIndex;
    int currentLen = current.length();
    
    // If we're building a word and have reached a valid word, 
    // the heuristic is the remaining characters we haven't matched
    return abs(remainingTarget - 0); 
}

int AStarSpellChecker::levenshteinDistance(const string& s1, const string& s2) {
    int m = s1.length();
    int n = s2.length();

    if (m == 0) return n;
    if (n == 0) return m;

    vector<vector<int>> dp(m + 1, vector<int>(n + 1));

    for (int i = 0; i <= m; i++) dp[i][0] = i;
    for (int j = 0; j <= n; j++) dp[0][j] = j;

    for (int k = 2; k <= m + n; k++) {
        int start_i = max(1, k - n);
        int end_i = min(m, k - 1);

        #pragma omp parallel for
        for (int i = start_i; i <= end_i; i++) {
            int j = k - i;
            
            if (s1[i - 1] == s2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                dp[i][j] = 1 + min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
            }
        }
    }

    return dp[m][n];
}

vector<pair<int, string>> AStarSpellChecker::findSimilarWords(const string& target, int maxDist) {
    vector<pair<int, string>> results;
    
    if (!trie || !trie->getRoot()) {
        return results;
    }
    
    // Priority queue: min-heap based on f-cost
    priority_queue<AStarState, vector<AStarState>, greater<AStarState>> openSet;
    
    // Track visited states to avoid cycles
    unordered_set<string> visited;
    
    // Initialize with root node
    AStarState initial;
    initial.node = trie->getRoot();
    initial.currentWord = "";
    initial.targetIndex = 0;
    initial.gCost = 0;
    initial.fCost = heuristic("", 0, target);
    
    openSet.push(initial);
    
    while (!openSet.empty()) {
        AStarState current = openSet.top();
        openSet.pop();
        
        // Create state key for visited check
        string stateKey = to_string(reinterpret_cast<uintptr_t>(current.node)) + "_" + to_string(current.targetIndex);
        
        if (visited.count(stateKey)) {
            continue;
        }
        visited.insert(stateKey);
        
        // Pruning: if g-cost already exceeds maxDist, skip
        if (current.gCost > maxDist) {
            continue;
        }
        
        // Check if current node is end of a valid word
        if (current.node->isEndOfWord) {
            // Calculate actual edit distance to verify
            int actualDist = levenshteinDistance(current.node->word, target);
            if (actualDist <= maxDist) {
                // Check if we already have this word
                bool found = false;
                for (const auto& [dist, word] : results) {
                    if (word == current.node->word) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    results.push_back({actualDist, current.node->word});
                }
            }
        }
        
        // Explore children (transitions in the Trie)
        for (const auto& [childChar, childNode] : current.node->children) {
            AStarState next;
            next.node = childNode;
            next.currentWord = current.currentWord + childChar;
            
            // Calculate cost based on Levenshtein operations
            int newGCost = current.gCost;
            
            if (current.targetIndex < target.length()) {
                if (childChar == target[current.targetIndex]) {
                    // Match: no additional cost
                    next.targetIndex = current.targetIndex + 1;
                } else {
                    // Substitution: cost 1
                    next.targetIndex = current.targetIndex + 1;
                    newGCost += 1;
                }
            } else {
                // Insertion in dictionary word (target exhausted): cost 1
                next.targetIndex = current.targetIndex;
                newGCost += 1;
            }
            
            next.gCost = newGCost;
            next.fCost = newGCost + heuristic(next.currentWord, next.targetIndex, target);
            
            // Only add if within bounds
            if (next.gCost <= maxDist) {
                openSet.push(next);
            }
        }
        
        // Handle deletion from target (skip a character in target)
        if (current.targetIndex < target.length()) {
            AStarState deleteState;
            deleteState.node = current.node;
            deleteState.currentWord = current.currentWord;
            deleteState.targetIndex = current.targetIndex + 1;
            deleteState.gCost = current.gCost + 1;  // Deletion cost
            deleteState.fCost = deleteState.gCost + heuristic(deleteState.currentWord, deleteState.targetIndex, target);
            
            if (deleteState.gCost <= maxDist) {
                openSet.push(deleteState);
            }
        }
    }
    
    // Sort results by edit distance
    sort(results.begin(), results.end());
    
    return results;
}

string AStarSpellChecker::findBestMatch(const string& target, int maxDist) {
    vector<pair<int, string>> similar = findSimilarWords(target, maxDist);
    
    if (similar.empty()) {
        return "";
    }
    
    return similar[0].second;  // Return the word with minimum edit distance
}

bool AStarSpellChecker::wordExists(const string& word) {
    return trie && trie->contains(word);
}
