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

// kd-tree node
struct KDTreeNode {
    Position pos;
    KDTreeNode* left;
    KDTreeNode* right;

    KDTreeNode(const Position& p) : pos(p), left(nullptr), right(nullptr) {}
};

class KDTree {
private:
    KDTreeNode* root;
    size_t dimensions;

    KDTreeNode* insertRecursive(KDTreeNode* node, const Position& pos, size_t depth) {
        // Base case: found insertion spot
        if (!node) {
            return new KDTreeNode(pos);
        }

        // Select axis based on depth so that all dimensions are cycled through
        size_t axis = depth % dimensions;

        // Compare position at current dimension and go left or right
        if (pos.coords[axis] < node->pos.coords[axis]) {
            node->left = insertRecursive(node->left, pos, depth + 1);
        } else {
            node->right = insertRecursive(node->right, pos, depth + 1);
        }

        return node;
    }

    // deletes nodes recursively (clear the kd-tree)
    void clearRecursive(KDTreeNode* node) {
        if (!node) return;
        clearRecursive(node->left);
        clearRecursive(node->right);
        delete node;
    }

    // find the nearet word to the target word (position object)
    void nearestNeighborRecursive(KDTreeNode* node, const Position& target, 
                                   size_t depth, Position& best, double& bestDist) {
        if (!node) return;

        // calculate distance to current node
        double dist = node->pos.distance(target);
        if (dist < bestDist) {
            bestDist = dist;
            best = node->pos;
        }

        // choose axis based on depth
        size_t axis = depth % dimensions;
        double diff = target.coords[axis] - node->pos.coords[axis];

        // pick a side to explore first
        KDTreeNode* nearSide = (diff < 0) ? node->left : node->right;
        KDTreeNode* farSide = (diff < 0) ? node->right : node->left;

        // visit near side
        nearestNeighborRecursive(nearSide, target, depth + 1, best, bestDist);

        // check to visit far side
        if (diff * diff < bestDist) {
            nearestNeighborRecursive(farSide, target, depth + 1, best, bestDist);
        }
    }

public:
    KDTree(size_t dims = 2) : root(nullptr), dimensions(dims) {}

    ~KDTree() {
        clearRecursive(root);
    }

    // insert new word into kd-tree
    void insert(const Position& pos) {
        if (pos.coords.size() != dimensions) {
            cerr << "Position dimensions do not match KD-Tree dimensions" << endl;
            return;
        }
        root = insertRecursive(root, pos, 0);
    }

    // returns the nearest word to the target
    Position findNearest(const Position& target) {
        if (!root) {
            cerr << "No words in KD-Tree." << endl;
            return Position{"", {}};
        }

        Position best = root->pos;
        double bestDist = numeric_limits<double>::max();
        nearestNeighborRecursive(root, target, 0, best, bestDist);
        return best;
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