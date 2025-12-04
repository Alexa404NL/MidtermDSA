#include "../include/spellchecker.h"

// Constructor and Destructor

SpellChecker::SpellChecker(int maxDist, int maxSugg) 
    : maxEditDistance(maxDist), maxSuggestions(maxSugg) {
    trie = new Trie();
    kdtree = new KDTree();
    astarChecker = new AStarSpellChecker(trie);
}

SpellChecker::~SpellChecker() {
    delete astarChecker;
    delete kdtree;
    delete trie;
}

// Private helper methods

string SpellChecker::toLowerCase(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), 
              [](unsigned char c) { return tolower(c); });
    return result;
}

string SpellChecker::cleanWord(const string& word) {
    string result;
    for (char c : word) {
        if (isalpha(static_cast<unsigned char>(c))) {
            result += tolower(static_cast<unsigned char>(c));
        }
    }
    return result;
}

vector<string> SpellChecker::tokenize(const string& text) {
    vector<string> tokens;
    istringstream stream(text);
    string word;
    
    while (stream >> word) {
        string cleaned = cleanWord(word);
        if (!cleaned.empty()) {
            tokens.push_back(cleaned);
        }
    }
    
    return tokens;
}

vector<pair<string, int>> SpellChecker::tokenizeWithLineNumbers(const string& text) {
    vector<pair<string, int>> tokens;
    istringstream stream(text);
    string line;
    int lineNumber = 1;
    
    while (getline(stream, line)) {
        istringstream lineStream(line);
        string word;
        while (lineStream >> word) {
            string cleaned = cleanWord(word);
            if (!cleaned.empty()) {
                tokens.push_back({cleaned, lineNumber});
            }
        }
        lineNumber++;
    }
    
    return tokens;
}

// Dictionary management

bool SpellChecker::loadDictionary(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open dictionary file: " << filename << endl;
        return false;
    }
    
    string word;
    int count = 0;
    
    while (file >> word) {
        string cleaned = cleanWord(word);
        if (!cleaned.empty() && cleaned.length() > 1) {  // Skip single letters
            trie->insert(cleaned);
            kdtree->insert(cleaned);
            count++;
        }
    }
    
    file.close();
    cout << "Loaded " << count << " words into dictionary." << endl;
    return true;
}

void SpellChecker::addWord(const string& word) {
    string cleaned = cleanWord(word);
    if (!cleaned.empty()) {
        trie->insert(cleaned);
        kdtree->insert(cleaned);
    }
}

bool SpellChecker::isValidWord(const string& word) {
    string cleaned = cleanWord(word);
    return trie->contains(cleaned);
}

int SpellChecker::getDictionarySize() const {
    // Note: This would require adding a size() method to Trie
    // For now, return -1 to indicate not implemented
    return -1;
}

// Spell checking methods

SpellCheckResult SpellChecker::checkText(const string& text, const string& method) {
    SpellCheckResult result;
    result.totalWords = 0;
    result.correctWords = 0;
    result.incorrectWords = 0;
    
    auto startTime = chrono::high_resolution_clock::now();
    
    vector<pair<string, int>> tokens = tokenizeWithLineNumbers(text);
    result.totalWords = tokens.size();
    
    int position = 0;
    for (const auto& [word, lineNum] : tokens) {
        if (!isValidWord(word)) {
            SpellingError error;
            error.originalWord = word;
            error.position = position;
            error.lineNumber = lineNum;
            error.method = method;
            
            // Get suggestions based on method
            if (method == "trie") {
                error.suggestions = getSuggestionsTrie(word);
            } else if (method == "kdtree") {
                error.suggestions = getSuggestionsKDTree(word);
            } else {  // default to astar
                error.suggestions = getSuggestionsAStar(word);
            }
            
            result.errors.push_back(error);
            result.incorrectWords++;
        } else {
            result.correctWords++;
        }
        position++;
    }
    
    auto endTime = chrono::high_resolution_clock::now();
    result.processingTimeMs = chrono::duration<double, milli>(endTime - startTime).count();
    
    return result;
}

SpellCheckResult SpellChecker::checkFile(const string& filename, const string& method) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file: " << filename << endl;
        return SpellCheckResult();
    }
    
    stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    return checkText(buffer.str(), method);
}

// Get suggestions for a single word

vector<string> SpellChecker::getSuggestionsTrie(const string& word) {
    vector<string> suggestions = trie->getSimilarWords(word, maxEditDistance);
    
    // Limit to maxSuggestions
    if (suggestions.size() > static_cast<size_t>(maxSuggestions)) {
        suggestions.resize(maxSuggestions);
    }
    
    return suggestions;
}

vector<string> SpellChecker::getSuggestionsKDTree(const string& word) {
    vector<Position> positions = kdtree->findKNearest(word, maxSuggestions);
    
    vector<string> suggestions;
    for (const auto& pos : positions) {
        suggestions.push_back(pos.word);
    }
    
    return suggestions;
}

vector<string> SpellChecker::getSuggestionsAStar(const string& word) {
    vector<pair<int, string>> results = astarChecker->findSimilarWords(word, maxEditDistance);
    
    vector<string> suggestions;
    for (const auto& [dist, w] : results) {
        suggestions.push_back(w);
        if (suggestions.size() >= static_cast<size_t>(maxSuggestions)) {
            break;
        }
    }
    
    return suggestions;
}

// Compare all methods

void SpellChecker::compareMethodsForWord(const string& word) {
    cout << "\n=== Comparison for word: \"" << word << "\" ===" << endl;
    cout << "Valid word: " << (isValidWord(word) ? "Yes" : "No") << endl;
    
    if (!isValidWord(word)) {
        // Trie method
        auto startTrie = chrono::high_resolution_clock::now();
        vector<string> trieSugg = getSuggestionsTrie(word);
        auto endTrie = chrono::high_resolution_clock::now();
        double trieTime = chrono::duration<double, milli>(endTrie - startTrie).count();
        
        cout << "\nTrie (Levenshtein) - Time: " << fixed << setprecision(3) << trieTime << " ms" << endl;
        cout << "  Suggestions: ";
        for (const auto& s : trieSugg) cout << s << " ";
        cout << endl;
        
        // KD-Tree method
        auto startKD = chrono::high_resolution_clock::now();
        vector<string> kdSugg = getSuggestionsKDTree(word);
        auto endKD = chrono::high_resolution_clock::now();
        double kdTime = chrono::duration<double, milli>(endKD - startKD).count();
        
        cout << "\nKD-Tree (Semantic) - Time: " << fixed << setprecision(3) << kdTime << " ms" << endl;
        cout << "  Suggestions: ";
        for (const auto& s : kdSugg) cout << s << " ";
        cout << endl;
        
        // A* method
        auto startAStar = chrono::high_resolution_clock::now();
        vector<string> astarSugg = getSuggestionsAStar(word);
        auto endAStar = chrono::high_resolution_clock::now();
        double astarTime = chrono::duration<double, milli>(endAStar - startAStar).count();
        
        cout << "\nA* Search - Time: " << fixed << setprecision(3) << astarTime << " ms" << endl;
        cout << "  Suggestions: ";
        for (const auto& s : astarSugg) cout << s << " ";
        cout << endl;
    }
}
