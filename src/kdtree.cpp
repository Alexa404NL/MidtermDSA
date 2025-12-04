#include "../include/kdtree.h"

// Position methods

double Position::distance(const Position& other) const {
    double sum = 0.0;
    for (size_t i = 0; i < coords.size(); ++i) {
        double diff = coords[i] - other.coords[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

bool Position::operator<(const Position& other) const {
    return word < other.word;
}

Position Position::fromWord(const string& word) {
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

// KDTree private methods

KDTreeNode* KDTree::insertRecursive(KDTreeNode* node, const Position& pos, size_t depth) {
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

void KDTree::clearRecursive(KDTreeNode* node) {
    if (!node) return;
    clearRecursive(node->left);
    clearRecursive(node->right);
    delete node;
}

void KDTree::kNearestRecursive(KDTreeNode* node, const Position& target, size_t depth,
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

// KDTree public methods

KDTree::KDTree() : root(nullptr), dimensions(5) {}

KDTree::~KDTree() {
    clearRecursive(root);
}

void KDTree::insert(const string word) {
    Position pos = Position::fromWord(word);

    if (pos.coords.size() != dimensions) {
        cerr << "Position dimensions do not match KD-Tree dimensions" << endl;
        return;
    }
    root = insertRecursive(root, pos, 0);
}

vector<Position> KDTree::findKNearest(const string target_word, size_t k) {
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
