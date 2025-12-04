#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <chrono>
#include "trie.h"
#include "kdtree.h"
#include "astar_spellcheck.h"

using namespace std;

// Represents a detected spelling error
struct SpellingError {
    string originalWord;
    int position;               // Position in text (word index)
    int lineNumber;             // Line number in text
    vector<string> suggestions; // Suggested corrections
    string method;              // "trie", "kdtree", or "astar"
};

// Result of spell checking a text
struct SpellCheckResult {
    vector<SpellingError> errors;
    int totalWords;
    int correctWords;
    int incorrectWords;
    double processingTimeMs;
};

class SpellChecker {
private:
    Trie* trie;
    KDTree* kdtree;
    AStarSpellChecker* astarChecker;
    int maxEditDistance;
    int maxSuggestions;
    
    // Text processing helpers
    string toLowerCase(const string& str);
    string cleanWord(const string& word);
    vector<string> tokenize(const string& text);
    vector<pair<string, int>> tokenizeWithLineNumbers(const string& text);
    
public:
    SpellChecker(int maxDist = 2, int maxSugg = 5);
    ~SpellChecker();
    
    // Dictionary management
    bool loadDictionary(const string& filename);
    void addWord(const string& word);
    bool isValidWord(const string& word);
    int getDictionarySize() const;
    
    // Spell checking methods
    SpellCheckResult checkText(const string& text, const string& method = "astar");
    SpellCheckResult checkFile(const string& filename, const string& method = "astar");
    
    // Get suggestions for a single word
    vector<string> getSuggestionsTrie(const string& word);
    vector<string> getSuggestionsKDTree(const string& word);
    vector<string> getSuggestionsAStar(const string& word);
    
    // Compare all methods
    void compareMethodsForWord(const string& word);
    
    // Accessors for benchmarking
    Trie* getTriePtr() { return trie; }
    KDTree* getKDTreePtr() { return kdtree; }
    AStarSpellChecker* getAStarPtr() { return astarChecker; }
};

#endif // SPELLCHECKER_H
