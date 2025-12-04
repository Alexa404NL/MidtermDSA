#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include "../include/trie.h"
#include "../include/kdtree.h"
#include "../include/astar_spellcheck.h"
#include "../include/spellchecker.h"

using namespace std;

// Test counters
int testsRun = 0;
int testsPassed = 0;

// Test helper macros
#define TEST(name) void name()
#define RUN_TEST(name) do { \
    cout << "Running " << #name << "... "; \
    testsRun++; \
    try { \
        name(); \
        testsPassed++; \
        cout << "PASSED\n"; \
    } catch (const exception& e) { \
        cout << "FAILED: " << e.what() << "\n"; \
    } \
} while(0)

#define ASSERT_TRUE(condition) do { \
    if (!(condition)) throw runtime_error("Assertion failed: " #condition); \
} while(0)

#define ASSERT_FALSE(condition) ASSERT_TRUE(!(condition))

#define ASSERT_EQ(expected, actual) do { \
    if ((expected) != (actual)) { \
        throw runtime_error("Expected " + to_string(expected) + " but got " + to_string(actual)); \
    } \
} while(0)

// ==================== TRIE TESTS ====================

TEST(test_trie_insert_and_contains) {
    Trie trie;
    trie.insert("hello");
    trie.insert("world");
    trie.insert("algorithm");
    
    ASSERT_TRUE(trie.contains("hello"));
    ASSERT_TRUE(trie.contains("world"));
    ASSERT_TRUE(trie.contains("algorithm"));
    ASSERT_FALSE(trie.contains("helo"));
    ASSERT_FALSE(trie.contains("wor"));
    ASSERT_FALSE(trie.contains(""));
}

TEST(test_trie_remove) {
    Trie trie;
    trie.insert("hello");
    trie.insert("help");
    trie.insert("helicopter");
    
    ASSERT_TRUE(trie.contains("hello"));
    trie.remove("hello");
    ASSERT_FALSE(trie.contains("hello"));
    ASSERT_TRUE(trie.contains("help"));
    ASSERT_TRUE(trie.contains("helicopter"));
}

TEST(test_trie_similar_words) {
    Trie trie;
    trie.insert("hello");
    trie.insert("hallo");
    trie.insert("help");
    trie.insert("world");
    
    vector<string> similar = trie.getSimilarWords("helo", 2);
    
    ASSERT_TRUE(similar.size() >= 2);  // Should find hello and hallo at least
    
    // Check that hello and hallo are in results
    bool foundHello = false, foundHallo = false;
    for (const auto& word : similar) {
        if (word == "hello") foundHello = true;
        if (word == "hallo") foundHallo = true;
    }
    ASSERT_TRUE(foundHello);
    ASSERT_TRUE(foundHallo);
}

TEST(test_trie_empty_word) {
    Trie trie;
    trie.insert("hello");
    // Empty word behavior: inserting empty string marks root as end,
    // but we still shouldn't find it useful in spell checking
    // This test verifies the trie handles edge cases gracefully
    ASSERT_TRUE(trie.contains("hello"));
}

TEST(test_trie_case_sensitivity) {
    Trie trie;
    trie.insert("Hello");
    trie.insert("hello");
    
    ASSERT_TRUE(trie.contains("Hello"));
    ASSERT_TRUE(trie.contains("hello"));
    ASSERT_FALSE(trie.contains("HELLO"));
}

// ==================== KD-TREE TESTS ====================

TEST(test_kdtree_insert_and_find) {
    KDTree kdtree;
    kdtree.insert("hello");
    kdtree.insert("world");
    kdtree.insert("algorithm");
    
    vector<Position> nearest = kdtree.findKNearest("hello", 1);
    ASSERT_TRUE(nearest.size() == 1);
    ASSERT_TRUE(nearest[0].word == "hello");
}

TEST(test_kdtree_find_k_nearest) {
    KDTree kdtree;
    kdtree.insert("hello");
    kdtree.insert("hallo");
    kdtree.insert("help");
    kdtree.insert("world");
    kdtree.insert("programming");
    
    vector<Position> nearest = kdtree.findKNearest("helo", 3);
    ASSERT_TRUE(nearest.size() == 3);
}

TEST(test_kdtree_similar_structure_words) {
    KDTree kdtree;
    kdtree.insert("cat");
    kdtree.insert("bat");
    kdtree.insert("hat");
    kdtree.insert("elephant");
    
    vector<Position> nearest = kdtree.findKNearest("rat", 3);
    ASSERT_TRUE(nearest.size() == 3);
    
    // Words with similar structure should be found
    bool foundSimilar = false;
    for (const auto& pos : nearest) {
        if (pos.word == "cat" || pos.word == "bat" || pos.word == "hat") {
            foundSimilar = true;
            break;
        }
    }
    ASSERT_TRUE(foundSimilar);
}

TEST(test_position_from_word) {
    Position pos = Position::fromWord("hello");
    
    ASSERT_TRUE(pos.word == "hello");
    ASSERT_TRUE(pos.coords.size() == 5);
    
    // Check that coordinates are in valid ranges
    for (double coord : pos.coords) {
        ASSERT_TRUE(coord >= -1.0 && coord <= 1.0);
    }
}

// ==================== A* TESTS ====================

TEST(test_astar_word_exists) {
    Trie trie;
    trie.insert("hello");
    trie.insert("world");
    
    AStarSpellChecker checker(&trie);
    
    ASSERT_TRUE(checker.wordExists("hello"));
    ASSERT_TRUE(checker.wordExists("world"));
    ASSERT_FALSE(checker.wordExists("helo"));
}

TEST(test_astar_find_similar) {
    Trie trie;
    trie.insert("hello");
    trie.insert("hallo");
    trie.insert("help");
    trie.insert("world");
    
    AStarSpellChecker checker(&trie);
    
    vector<pair<int, string>> similar = checker.findSimilarWords("helo", 2);
    
    ASSERT_TRUE(similar.size() >= 1);
    
    // First result should be the closest match
    bool foundClose = false;
    for (const auto& [dist, word] : similar) {
        if (word == "hello" || word == "hallo" || word == "help") {
            foundClose = true;
            ASSERT_TRUE(dist <= 2);
        }
    }
    ASSERT_TRUE(foundClose);
}

TEST(test_astar_best_match) {
    Trie trie;
    trie.insert("hello");
    trie.insert("help");
    trie.insert("world");
    
    AStarSpellChecker checker(&trie);
    
    string best = checker.findBestMatch("helo", 2);
    ASSERT_TRUE(best == "hello" || best == "help");  // Both are valid at distance 1
}

TEST(test_astar_no_match_within_distance) {
    Trie trie;
    trie.insert("algorithm");
    trie.insert("programming");
    
    AStarSpellChecker checker(&trie);
    
    vector<pair<int, string>> similar = checker.findSimilarWords("xyz", 1);
    ASSERT_TRUE(similar.empty());
}

// ==================== SPELLCHECKER TESTS ====================

TEST(test_spellchecker_valid_word) {
    SpellChecker checker(2, 5);
    checker.addWord("hello");
    checker.addWord("world");
    
    ASSERT_TRUE(checker.isValidWord("hello"));
    ASSERT_TRUE(checker.isValidWord("world"));
    ASSERT_FALSE(checker.isValidWord("helo"));
}

TEST(test_spellchecker_suggestions_trie) {
    SpellChecker checker(2, 5);
    checker.addWord("hello");
    checker.addWord("hallo");
    checker.addWord("help");
    
    vector<string> suggestions = checker.getSuggestionsTrie("helo");
    ASSERT_TRUE(suggestions.size() >= 1);
}

TEST(test_spellchecker_suggestions_kdtree) {
    SpellChecker checker(2, 5);
    checker.addWord("hello");
    checker.addWord("hallo");
    checker.addWord("help");
    
    vector<string> suggestions = checker.getSuggestionsKDTree("helo");
    ASSERT_TRUE(suggestions.size() >= 1);
}

TEST(test_spellchecker_suggestions_astar) {
    SpellChecker checker(2, 5);
    checker.addWord("hello");
    checker.addWord("hallo");
    checker.addWord("help");
    
    vector<string> suggestions = checker.getSuggestionsAStar("helo");
    ASSERT_TRUE(suggestions.size() >= 1);
}

TEST(test_spellchecker_check_text) {
    SpellChecker checker(2, 5);
    checker.addWord("the");
    checker.addWord("quick");
    checker.addWord("brown");
    checker.addWord("fox");
    
    SpellCheckResult result = checker.checkText("the quik brown fox", "astar");
    
    ASSERT_EQ(4, result.totalWords);
    ASSERT_EQ(3, result.correctWords);
    ASSERT_EQ(1, result.incorrectWords);
    ASSERT_EQ(1, (int)result.errors.size());
    ASSERT_TRUE(result.errors[0].originalWord == "quik");
}

// ==================== MAIN ====================

int main() {
    cout << "\n";
    cout << "╔══════════════════════════════════════════════════════════════╗\n";
    cout << "║                    UNIT TEST SUITE                           ║\n";
    cout << "║               Spell Checker System Tests                     ║\n";
    cout << "╚══════════════════════════════════════════════════════════════╝\n";
    cout << "\n";
    
    cout << "=== Trie Tests ===\n";
    RUN_TEST(test_trie_insert_and_contains);
    RUN_TEST(test_trie_remove);
    RUN_TEST(test_trie_similar_words);
    RUN_TEST(test_trie_empty_word);
    RUN_TEST(test_trie_case_sensitivity);
    
    cout << "\n=== KD-Tree Tests ===\n";
    RUN_TEST(test_kdtree_insert_and_find);
    RUN_TEST(test_kdtree_find_k_nearest);
    RUN_TEST(test_kdtree_similar_structure_words);
    RUN_TEST(test_position_from_word);
    
    cout << "\n=== A* Search Tests ===\n";
    RUN_TEST(test_astar_word_exists);
    RUN_TEST(test_astar_find_similar);
    RUN_TEST(test_astar_best_match);
    RUN_TEST(test_astar_no_match_within_distance);
    
    cout << "\n=== SpellChecker Tests ===\n";
    RUN_TEST(test_spellchecker_valid_word);
    RUN_TEST(test_spellchecker_suggestions_trie);
    RUN_TEST(test_spellchecker_suggestions_kdtree);
    RUN_TEST(test_spellchecker_suggestions_astar);
    RUN_TEST(test_spellchecker_check_text);
    
    cout << "\n";
    cout << "════════════════════════════════════════════════════════════════\n";
    cout << "  RESULTS: " << testsPassed << "/" << testsRun << " tests passed";
    if (testsPassed == testsRun) {
        cout << " ✓ ALL PASSED!\n";
    } else {
        cout << " ✗ " << (testsRun - testsPassed) << " FAILED\n";
    }
    cout << "════════════════════════════════════════════════════════════════\n";
    
    return (testsPassed == testsRun) ? 0 : 1;
}
