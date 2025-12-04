#ifndef TRIE_H
#define TRIE_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
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

    void clear(TrieNode* node);
    bool remove(TrieNode* curr, const string& word, int depth);
    void searchRecursive(TrieNode* node, char letter, const string& target, 
                         const vector<int>& prevRow, vector<string>& results, int maxDist);

public:
    Trie();
    ~Trie();

    void insert(const string& word);
    bool contains(const string& word);
    void remove(const string& key);
    vector<string> getSimilarWords(const string& word, int maxDist);
    
    // Accessor for A* search
    TrieNode* getRoot() const { return root; }
};

#endif // TRIE_H
