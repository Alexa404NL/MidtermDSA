#ifndef KDTREE_H
#define KDTREE_H

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

// position of word in tree (calculates distance)
struct Position {
    string word;
    vector<double> coords;

    double distance(const Position& other) const;

    // comparison operator for sorting (used when sorting pairs of <distance, Position>)
    bool operator<(const Position& other) const;

    // convert word to position object
    static Position fromWord(const string& word);
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

    KDTreeNode* insertRecursive(KDTreeNode* node, const Position& pos, size_t depth);
    void clearRecursive(KDTreeNode* node);
    void kNearestRecursive(KDTreeNode* node, const Position& target, size_t depth,
                           vector<pair<double, Position>>& candidates, size_t k);

public:
    KDTree();
    ~KDTree();

    void insert(const string word);
    vector<Position> findKNearest(const string target_word, size_t k);
    
    // Get dimensions count
    size_t getDimensions() const { return dimensions; }
};

#endif // KDTREE_H
