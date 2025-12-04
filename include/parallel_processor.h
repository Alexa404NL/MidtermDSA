#ifndef PARALLEL_PROCESSOR_H
#define PARALLEL_PROCESSOR_H

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <omp.h>
#include "spellchecker.h"

using namespace std;

// Parallel spell checker using OpenMP
class ParallelSpellChecker {
private:
    SpellChecker* checker;
    int numThreads;
    
    // Helper to split text into words
    vector<string> tokenize(const string& text);
    
public:
    ParallelSpellChecker(SpellChecker* sc, int threads = 0);  // 0 = auto-detect
    
    // Parallel spell checking
    SpellCheckResult checkTextParallel(const string& text, const string& method = "astar");
    SpellCheckResult checkFileParallel(const string& filename, const string& method = "astar");
    
    // Batch processing with OpenMP
    vector<vector<string>> getSuggestionsParallel(const vector<string>& words, const string& method = "astar");
    
    // Performance comparison
    void compareSequentialVsParallel(const string& text);
    
    // Get thread count
    int getThreadCount() const { return numThreads; }
    
    // Set thread count
    void setThreadCount(int threads);
};

// Results for parallel processing
struct ParallelResult {
    vector<SpellingError> errors;
    int wordCount;
    int correctCount;
    int errorCount;
    double timeMs;
    int threadId;
};

#endif // PARALLEL_PROCESSOR_H
