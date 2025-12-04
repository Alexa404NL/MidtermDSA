#include "../include/parallel_processor.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>

// ==================== ParallelSpellChecker Implementation (OpenMP) ====================

ParallelSpellChecker::ParallelSpellChecker(SpellChecker* sc, int threads) 
    : checker(sc) {
    if (threads <= 0) {
        // Auto-detect: use OpenMP's default or hardware concurrency
        #ifdef _OPENMP
        numThreads = omp_get_max_threads();
        #else
        numThreads = 4;  // Fallback default
        #endif
    } else {
        numThreads = threads;
    }
    
    #ifdef _OPENMP
    omp_set_num_threads(numThreads);
    #endif
}

// Set thread count
void ParallelSpellChecker::setThreadCount(int threads) {
    numThreads = threads > 0 ? threads : 4;
    #ifdef _OPENMP
    omp_set_num_threads(numThreads);
    #endif
}

// Tokenize text into words
vector<string> ParallelSpellChecker::tokenize(const string& text) {
    vector<string> words;
    string word;
    
    for (char c : text) {
        if (isalpha(c)) {
            word += tolower(c);
        } else if (!word.empty()) {
            words.push_back(word);
            word.clear();
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }
    
    return words;
}

// Parallel spell check using OpenMP
SpellCheckResult ParallelSpellChecker::checkTextParallel(const string& text, const string& method) {
    auto startTime = chrono::high_resolution_clock::now();
    
    vector<string> words = tokenize(text);
    int totalWords = words.size();
    
    // Thread-local storage for errors
    vector<vector<SpellingError>> threadErrors(numThreads);
    vector<int> threadCorrect(numThreads, 0);
    
    #ifdef _OPENMP
    #pragma omp parallel num_threads(numThreads)
    {
        int tid = omp_get_thread_num();
        
        #pragma omp for schedule(dynamic, 10)
        for (int i = 0; i < (int)words.size(); i++) {
            const string& word = words[i];
            
            if (checker->isValidWord(word)) {
                threadCorrect[tid]++;
            } else {
                SpellingError error;
                error.originalWord = word;
                error.lineNumber = 1;  // Simplified
                error.position = i;
                error.method = method;
                
                // Get suggestions based on method
                if (method == "trie") {
                    error.suggestions = checker->getSuggestionsTrie(word);
                } else if (method == "kdtree") {
                    error.suggestions = checker->getSuggestionsKDTree(word);
                } else {
                    error.suggestions = checker->getSuggestionsAStar(word);
                }
                
                threadErrors[tid].push_back(error);
            }
        }
    }
    #else
    // Fallback: sequential processing if OpenMP not available
    for (size_t i = 0; i < words.size(); i++) {
        const string& word = words[i];
        
        if (checker->isValidWord(word)) {
            threadCorrect[0]++;
        } else {
            SpellingError error;
            error.originalWord = word;
            error.lineNumber = 1;
            error.position = i;
            error.method = method;
            
            if (method == "trie") {
                error.suggestions = checker->getSuggestionsTrie(word);
            } else if (method == "kdtree") {
                error.suggestions = checker->getSuggestionsKDTree(word);
            } else {
                error.suggestions = checker->getSuggestionsAStar(word);
            }
            
            threadErrors[0].push_back(error);
        }
    }
    #endif
    
    // Aggregate results
    SpellCheckResult result;
    result.totalWords = totalWords;
    result.correctWords = 0;
    result.incorrectWords = 0;
    
    for (int t = 0; t < numThreads; t++) {
        result.correctWords += threadCorrect[t];
        for (const auto& err : threadErrors[t]) {
            result.errors.push_back(err);
        }
    }
    result.incorrectWords = result.errors.size();
    
    auto endTime = chrono::high_resolution_clock::now();
    result.processingTimeMs = chrono::duration<double, milli>(endTime - startTime).count();
    
    return result;
}

// Check file with parallel processing
SpellCheckResult ParallelSpellChecker::checkFileParallel(const string& filename, const string& method) {
    ifstream file(filename);
    if (!file.is_open()) {
        SpellCheckResult result;
        result.totalWords = 0;
        result.correctWords = 0;
        result.incorrectWords = 0;
        result.processingTimeMs = 0;
        return result;
    }
    
    stringstream buffer;
    buffer << file.rdbuf();
    return checkTextParallel(buffer.str(), method);
}

// Get suggestions for multiple words in parallel
vector<vector<string>> ParallelSpellChecker::getSuggestionsParallel(const vector<string>& words, const string& method) {
    vector<vector<string>> allSuggestions(words.size());
    
    #ifdef _OPENMP
    #pragma omp parallel for num_threads(numThreads) schedule(dynamic)
    #endif
    for (size_t i = 0; i < words.size(); i++) {
        if (method == "trie") {
            allSuggestions[i] = checker->getSuggestionsTrie(words[i]);
        } else if (method == "kdtree") {
            allSuggestions[i] = checker->getSuggestionsKDTree(words[i]);
        } else {
            allSuggestions[i] = checker->getSuggestionsAStar(words[i]);
        }
    }
    
    return allSuggestions;
}

// Compare sequential vs parallel performance
void ParallelSpellChecker::compareSequentialVsParallel(const string& text) {
    cout << "\n╔══════════════════════════════════════════════════════════════╗\n";
    cout << "║          SEQUENTIAL vs PARALLEL COMPARISON (OpenMP)         ║\n";
    cout << "╚══════════════════════════════════════════════════════════════╝\n\n";
    
    #ifdef _OPENMP
    cout << "OpenMP Version: " << _OPENMP << "\n";
    cout << "Max Threads: " << omp_get_max_threads() << "\n";
    #else
    cout << "OpenMP: Not available (running sequential fallback)\n";
    #endif
    cout << "Configured Threads: " << numThreads << "\n";
    cout << "Text length: " << text.length() << " characters\n\n";
    
    vector<string> words = tokenize(text);
    cout << "Total words to check: " << words.size() << "\n\n";
    
    // Sequential timing
    cout << "Running sequential spell check...\n";
    auto seqStart = chrono::high_resolution_clock::now();
    
    int seqCorrect = 0;
    int seqErrors = 0;
    for (const auto& word : words) {
        if (checker->isValidWord(word)) {
            seqCorrect++;
        } else {
            checker->getSuggestionsAStar(word);  // Use A* as default
            seqErrors++;
        }
    }
    
    auto seqEnd = chrono::high_resolution_clock::now();
    double seqTime = chrono::duration<double, milli>(seqEnd - seqStart).count();
    
    // Parallel timing
    cout << "Running parallel spell check (" << numThreads << " threads)...\n\n";
    auto parStart = chrono::high_resolution_clock::now();
    
    SpellCheckResult parResult = checkTextParallel(text, "astar");
    
    auto parEnd = chrono::high_resolution_clock::now();
    double parTime = chrono::duration<double, milli>(parEnd - parStart).count();
    
    // Calculate metrics
    double speedup = seqTime / parTime;
    double efficiency = (speedup / numThreads) * 100;
    double improvement = ((seqTime - parTime) / seqTime) * 100;
    
    // Display results
    cout << "┌────────────────────────────────────────────────────────────────┐\n";
    cout << "│                        RESULTS                                 │\n";
    cout << "├─────────────────────┬──────────────────┬───────────────────────┤\n";
    cout << "│      Metric         │    Sequential    │       Parallel        │\n";
    cout << "├─────────────────────┼──────────────────┼───────────────────────┤\n";
    cout << "│ Total Words         │ " << setw(16) << words.size() << " │ " << setw(21) << parResult.totalWords << " │\n";
    cout << "│ Errors Found        │ " << setw(16) << seqErrors << " │ " << setw(21) << parResult.incorrectWords << " │\n";
    cout << "│ Time (ms)           │ " << setw(16) << fixed << setprecision(2) << seqTime << " │ " << setw(21) << parTime << " │\n";
    
    double seqThroughput = (words.size() / seqTime) * 1000;
    double parThroughput = (words.size() / parTime) * 1000;
    cout << "│ Throughput (w/s)    │ " << setw(16) << setprecision(2) << seqThroughput << " │ " << setw(21) << parThroughput << " │\n";
    cout << "└─────────────────────┴──────────────────┴───────────────────────┘\n\n";
    
    cout << "  Speedup: " << fixed << setprecision(2) << speedup << "x\n";
    cout << "  Parallel Efficiency: " << setprecision(2) << efficiency << "%\n";
    
    if (improvement > 0) {
        cout << "  ✓ Parallel processing is " << setprecision(2) << improvement << "% faster!\n";
    } else {
        cout << "  ✗ Sequential was faster (overhead too high for small input)\n";
    }
    
    // Scalability analysis
    cout << "\n┌─────────────────── SCALABILITY ANALYSIS ───────────────────┐\n";
    #ifdef _OPENMP
    for (int t = 1; t <= numThreads; t++) {
        omp_set_num_threads(t);
        
        auto start = chrono::high_resolution_clock::now();
        checkTextParallel(text, "astar");
        auto end = chrono::high_resolution_clock::now();
        double time = chrono::duration<double, milli>(end - start).count();
        
        double sp = seqTime / time;
        int barLen = (int)(sp * 10);
        
        cout << "│  " << t << " thread" << (t > 1 ? "s" : " ") << ":  [";
        for (int i = 0; i < 20; i++) {
            cout << (i < barLen ? "█" : "░");
        }
        cout << "] " << fixed << setprecision(2) << sp << "x speedup";
        cout << " (" << setprecision(1) << time << " ms)\n";
    }
    omp_set_num_threads(numThreads);  // Restore
    #else
    cout << "│  Scalability analysis requires OpenMP support              │\n";
    #endif
    cout << "└─────────────────────────────────────────────────────────────┘\n";
}
