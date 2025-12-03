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

    // comparison operator for sorting (used when sorting pairs of <distance, Position>)
    bool operator<(const Position& other) const {
        return word < other.word;
    }

    // convert word to position object
    static Position fromWord(const string& word) {
        size_t dimensions = 5;
        Position pos;
        pos.word = word;
        pos.coords.assign(dimensions, 0.0);

        size_t total_letters = 0;
        size_t vowel_count = 0;
        double common_letter_freq_sum = 0.0;
        size_t first_half_count = 0;  // a-m
        size_t second_half_count = 0; // n-z

        // calculate frequencies
        for (char ch : word)
        {
            if (isalpha(static_cast<unsigned char>(ch)))
            {
                char lower_ch = tolower(ch);
                total_letters++;

                if (lower_ch == 'a' || lower_ch == 'e' || lower_ch == 'i' || lower_ch == 'o' || lower_ch == 'u')
                {
                    vowel_count++;
                }

                if (lower_ch == 'e' || lower_ch == 't' || lower_ch == 'a' ||
                    lower_ch == 'i' || lower_ch == 'n' || lower_ch == 'o')
                {
                    common_letter_freq_sum += 1.0;
                }

                if (lower_ch >= 'a' && lower_ch <= 'm')
                {
                    first_half_count++;
                }
                else if (lower_ch >= 'n' && lower_ch <= 'z')
                {
                    second_half_count++;
                }
            }
        }

        double len_norm = 1.0; // normalize from word length
        if (total_letters > 0)
        {
            // dimension 1: normalized word length
            pos.coords[0] = min(1.0, (double)total_letters / 20.0);

            // dimension 2: vowel ratio
            pos.coords[1] = (double)vowel_count / total_letters;

            // dimension 3: common letter ratio
            pos.coords[2] = common_letter_freq_sum / total_letters;

            // dimension 4: first half
            pos.coords[3] = (double)(first_half_count - second_half_count) / total_letters;

            // dimension 5: first letter weight
            char first_char = tolower(word[0]);
            pos.coords[4] = (double)(first_char - 'a' + 1) / 26.0;
        }
        return pos;
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

    // find the k nearest words to target word
    void kNearestRecursive(KDTreeNode* node, const Position& target, size_t depth,
                           vector<pair<double, Position>>& candidates, size_t k) {
        if (!node) return;

        double dist = node->pos.distance(target);
        candidates.push_back({dist, node->pos});

        // keep only k best candidates
        sort(candidates.begin(), candidates.end());
        if (candidates.size() > k) {
            candidates.resize(k);
        }

        size_t axis = depth % dimensions;
        double diff = target.coords[axis] - node->pos.coords[axis];

        KDTreeNode* nearSide = (diff < 0) ? node->left : node->right;
        KDTreeNode* farSide = (diff < 0) ? node->right : node->left;

        kNearestRecursive(nearSide, target, depth + 1, candidates, k);

        // check to explore far side
        if (candidates.size() < k || diff * diff < candidates.back().first) {
            kNearestRecursive(farSide, target, depth + 1, candidates, k);
        }
    }

public:
    KDTree() : root(nullptr), dimensions(5) {}

    ~KDTree() {
        clearRecursive(root);
    }

    // insert new word into kd-tree
    void insert(const string word) {
        Position pos = Position::fromWord(word);

        if (pos.coords.size() != dimensions) {
            cerr << "Position dimensions do not match KD-Tree dimensions" << endl;
            return;
        }
        root = insertRecursive(root, pos, 0);
    }

    // returns k nearest words to the target
    vector<Position> findKNearest(const string target_word, size_t k) {
        Position target = Position::fromWord(target_word);
        vector<pair<double, Position>> candidates;

        if (!root) {
            cerr << "No words in KD-Tree." << endl;
            return {};
        }

        kNearestRecursive(root, target, 0, candidates, k);

        // sort by distance and get positions
        sort(candidates.begin(), candidates.end());
        vector<Position> results;
        for (const auto& [dist, pos] : candidates) {
            results.push_back(pos);
        }
        return results;
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

int main () 
{
    // KD-Tree example so yall know how to call it
    // Use 5 dimensions to represent words more accurately
    KDTree kdtree;

    // Insert words
    kdtree.insert("hello");
    kdtree.insert("hallo");
    kdtree.insert("world");

    // Find nearest word to a typo's coordinates
    string typo = "halo";
    vector<Position> suggestion = kdtree.findKNearest(typo, 1);
    cout << "Suggested word: " << suggestion[0].word << endl;

    cout << endl;
    // Or find top 3 suggestions for typo
    vector<Position> topSuggestions = kdtree.findKNearest(typo, 3);
    for (const auto& pos : topSuggestions) {
        cout << "Suggestion: " << pos.word << endl;
    }
    
    return 0;
}