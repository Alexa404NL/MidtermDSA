#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

using namespace std;

class Trie {
public:
    unordered_map<string, Trie*> child;
    bool isLeaf;

        Trie() {
            isLeaf = false;
        }

    ~Trie() {
        for (auto& pair : child) {
            delete pair.second;
        }
    }

    void insert(const string& key) {
        Trie* curr = this;
        stringstream ss(key);
        string word;
        while (ss >> word) {
            if (curr->child.find(word) == curr->child.end()) {
                curr->child[word] = new Trie();
            }
            curr = curr->child[word];
        }
        curr->isLeaf = true;
    }

    bool contains(const string& key) {
        Trie* curr = this;
        stringstream ss(key);
        string word;
        while (ss >> word) {
            if (curr->child.find(word) == curr->child.end()) {
                return false;
            }
            curr = curr->child[word];
        }
        return curr != nullptr && curr->isLeaf;
    }

    void remove(const string& key) {
        remove(this, key);
    }

private:
    bool remove(Trie* root, const string& key) {
        string ss(key);
        Trie* curr= root; 
        if (curr == nullptr) {
            return false;
        }

        string word;
        if (!(ss.find(' '))) {
            // End of the key
            if (curr->isLeaf) {
                curr->isLeaf = false;
                return curr->child.empty();
            }
        }

        if (curr->child.count(word) && remove(curr->child[word], ss)) {
            delete curr->child[word];
            curr->child.erase(word);
            return !curr->isLeaf && curr->child.empty();
        }
    }
};
