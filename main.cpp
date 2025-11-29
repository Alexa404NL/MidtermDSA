#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <climits>
#include <numeric>

using namespace std;

struct TrieNode {
    map<char, TrieNode*> children;
    bool isEndOfWord;
    string word;

    TrieNode() : isEndOfWord(false) {}
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
            //TODO: recursive search through thre trie
        }

        return results;
    }
};