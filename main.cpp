#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <climits>
#include <numeric>
#include <cmath>

using namespace std;

struct TrieNode {
    map<char, TrieNode*> children;
    bool isEndOfWord;
    string word;

    TrieNode() : isEndOfWord(false) {}
};


// position of word in tree (calculates distance)
//TODO add kd node structure and tree
struct Position {
    string word;
    vector<double> coords;

    double distance(const Position& other) const {
        double sum = 0.0;
        for (size_t i = 0; i < coords.size(); ++i) {
            double diff = coords[i] - other.coords[i];
            sum += diff * diff;
        }
        return sqrt(sum);
    }
};

class Trie {
private:
    TrieNode* root;

    void clear(TrieNode* node) {
        for(auto& pair : node->children) {
            clear(pair.second);
        }
        delete node;
    }

    bool remove(TrieNode* curr, const string& word, int depth) { // remove for clean up 
        if (!curr) return false;

        if (depth == word.size()) {
            if (curr->isEndOfWord) {
                curr->isEndOfWord = false;
                return curr->children.empty();
            } // delete word from logical structure
            return false;
        }

        char c = word[depth];
        if (curr->children.find(c) == curr->children.end()) return false; 

        bool shouldDeleteChild = remove(curr->children[c], word, depth + 1);

        if (shouldDeleteChild) {
            delete curr->children[c];
            curr->children.erase(c);
            return !curr->isEndOfWord && curr->children.empty(); //establish new end
        }

        return false;
    }

    void searchRecursive(TrieNode* node, char letter, const string& target, 
                         const vector<int>& prevRow, vector<string>& results, int maxDist) {
        
        int columns = target.size() + 1;
        vector<int> currentRow(columns);
        currentRow[0] = prevRow[0] + 1;

        int minRowCost = currentRow[0];

        for (int i = 1; i < columns; i++) {
            int insertCost = currentRow[i - 1] + 1;
            int deleteCost = prevRow[i] + 1;
            int replaceCost = prevRow[i - 1];
            
            if (target[i - 1] != letter) {
                replaceCost += 1;
            }

            currentRow[i] = min({ insertCost, deleteCost, replaceCost });
            minRowCost = min(minRowCost, currentRow[i]);
        }

        if (minRowCost > maxDist) {
            return;
        }

        if (node->isEndOfWord && currentRow.back() <= maxDist) {
            results.push_back(node->word);
        }

        for (auto const& [key, childNode] : node->children) {
            searchRecursive(childNode, key, target, currentRow, results, maxDist);
        }
    }

public:
    Trie() {
        root = new TrieNode();
    }

    ~Trie() {
        clear(root);
    }

    void insert(const string& word) {
        TrieNode* curr = root;
        for (char c : word) {
            if (curr->children.find(c) == curr->children.end()) {
                curr->children[c] = new TrieNode();
            }
            curr = curr->children[c];
        }
        curr->isEndOfWord = true;
        curr->word = word;
    }

    bool contains(const string& word) {
        TrieNode* curr = root;
        for (char c : word) {
            if (curr->children.find(c) == curr->children.end()) {
                return false;
            }
            curr = curr->children[c];
        }
        return curr != nullptr && curr->isEndOfWord;
    }

    void remove(const string& key) {
        remove(root, key, 0);
    }

    vector<string> getSimilarWords(const string& word, int maxDist) {
        vector<string> results;
        
        vector<int> currentRow(word.size() + 1);
        iota(currentRow.begin(), currentRow.end(), 0);

        for (auto const& [key, childNode] : root->children) {
           searchRecursive(childNode, key, word, currentRow, results, maxDist);
        }

        return results;
    }
};