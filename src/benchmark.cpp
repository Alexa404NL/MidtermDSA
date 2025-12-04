#include "../include/benchmark.h"
#include <cfloat>

// Constructor

Benchmark::Benchmark(SpellChecker* sc, const string& outDir) 
    : checker(sc), outputDir(outDir) {}

// Timing helper template implementation

template<typename Func>
double Benchmark::measureTime(Func&& func, int iterations) {
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) {
        func();
    }
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration<double, milli>(end - start).count() / iterations;
}

// Statistics helpers

double Benchmark::calculateMean(const vector<double>& values) {
    if (values.empty()) return 0.0;
    return accumulate(values.begin(), values.end(), 0.0) / values.size();
}

double Benchmark::calculateStdDev(const vector<double>& values, double mean) {
    if (values.size() < 2) return 0.0;
    double sumSq = 0.0;
    for (double v : values) {
        sumSq += (v - mean) * (v - mean);
    }
    return sqrt(sumSq / (values.size() - 1));
}

// Benchmark methods

void Benchmark::runAllBenchmarks() {
    cout << "\n╔══════════════════════════════════════════════════════════════╗\n";
    cout << "║              RUNNING ALL BENCHMARKS                          ║\n";
    cout << "╚══════════════════════════════════════════════════════════════╝\n\n";
    
    benchmarkSingleWordLookup(100);
    
    vector<string> testWords = {
        "helo", "wrold", "programing", "algoritm", "speling",
        "recieve", "occured", "seperate", "definately", "accomodate",
        "thier", "wierd", "freind", "beleive", "enviroment"
    };
    benchmarkMethodComparison(testWords);
    
    printSummary();
}

void Benchmark::benchmarkSingleWordLookup(int iterations) {
    cout << "Running single word lookup benchmark (" << iterations << " iterations)...\n";
    
    vector<string> testWords = {"hello", "world", "algorithm", "programming"};
    
    for (const string& method : {"trie", "kdtree", "astar"}) {
        vector<double> times;
        
        for (const string& word : testWords) {
            for (int i = 0; i < iterations; i++) {
                auto start = chrono::high_resolution_clock::now();
                
                if (method == "trie") {
                    checker->getSuggestionsTrie(word);
                } else if (method == "kdtree") {
                    checker->getSuggestionsKDTree(word);
                } else {
                    checker->getSuggestionsAStar(word);
                }
                
                auto end = chrono::high_resolution_clock::now();
                times.push_back(chrono::duration<double, milli>(end - start).count());
            }
        }
        
        BenchmarkResult result;
        result.methodName = method;
        result.testName = "single_word_lookup";
        result.inputSize = testWords.size();
        result.iterations = iterations;
        result.avgTimeMs = calculateMean(times);
        result.stdDevMs = calculateStdDev(times, result.avgTimeMs);
        result.minTimeMs = *min_element(times.begin(), times.end());
        result.maxTimeMs = *max_element(times.begin(), times.end());
        result.throughput = 1000.0 / result.avgTimeMs;  // words per second
        
        results.push_back(result);
    }
}

void Benchmark::benchmarkTextProcessing(const vector<string>& testFiles) {
    cout << "Running text processing benchmark...\n";
    
    for (const string& filepath : testFiles) {
        ifstream file(filepath);
        if (!file.is_open()) continue;
        
        stringstream buffer;
        buffer << file.rdbuf();
        string text = buffer.str();
        file.close();
        
        // Count words
        int wordCount = 0;
        istringstream iss(text);
        string word;
        while (iss >> word) wordCount++;
        
        for (const string& method : {"trie", "kdtree", "astar"}) {
            auto start = chrono::high_resolution_clock::now();
            checker->checkText(text, method);
            auto end = chrono::high_resolution_clock::now();
            
            BenchmarkResult result;
            result.methodName = method;
            result.testName = "text_processing_" + filepath;
            result.inputSize = wordCount;
            result.iterations = 1;
            result.avgTimeMs = chrono::duration<double, milli>(end - start).count();
            result.throughput = (wordCount / result.avgTimeMs) * 1000.0;
            
            results.push_back(result);
        }
    }
}

void Benchmark::benchmarkScalability(const vector<int>& dictionarySizes) {
    cout << "Running scalability benchmark...\n";
    // This would require loading dictionaries of different sizes
    // For now, just log that it would be run
    cout << "  (Scalability benchmark requires multiple dictionary sizes)\n";
}

void Benchmark::benchmarkMethodComparison(const vector<string>& testWords) {
    cout << "Running method comparison benchmark...\n";
    
    for (const string& word : testWords) {
        MethodComparison comp;
        comp.word = word;
        
        // Trie
        auto start = chrono::high_resolution_clock::now();
        comp.trieSuggestions = checker->getSuggestionsTrie(word);
        auto end = chrono::high_resolution_clock::now();
        comp.trieTimeMs = chrono::duration<double, milli>(end - start).count();
        
        // KD-Tree
        start = chrono::high_resolution_clock::now();
        comp.kdtreeSuggestions = checker->getSuggestionsKDTree(word);
        end = chrono::high_resolution_clock::now();
        comp.kdtreeTimeMs = chrono::duration<double, milli>(end - start).count();
        
        // A*
        start = chrono::high_resolution_clock::now();
        comp.astarSuggestions = checker->getSuggestionsAStar(word);
        end = chrono::high_resolution_clock::now();
        comp.astarTimeMs = chrono::duration<double, milli>(end - start).count();
        
        comparisons.push_back(comp);
    }
}

void Benchmark::benchmarkParallelProcessing(const string& text, int numThreads) {
    cout << "Running parallel processing benchmark with " << numThreads << " threads...\n";
    
    // Tokenize text
    vector<string> words;
    istringstream iss(text);
    string word;
    while (iss >> word) words.push_back(word);
    
    // Sequential baseline
    auto startSeq = chrono::high_resolution_clock::now();
    for (const string& w : words) {
        checker->getSuggestionsAStar(w);
    }
    auto endSeq = chrono::high_resolution_clock::now();
    double seqTime = chrono::duration<double, milli>(endSeq - startSeq).count();
    
    // Parallel version
    auto startPar = chrono::high_resolution_clock::now();
    
    vector<thread> threads;
    atomic<size_t> index(0);
    mutex resultMutex;
    
    for (int t = 0; t < numThreads; t++) {
        threads.emplace_back([&]() {
            size_t i;
            while ((i = index.fetch_add(1)) < words.size()) {
                checker->getSuggestionsAStar(words[i]);
            }
        });
    }
    
    for (auto& th : threads) {
        th.join();
    }
    
    auto endPar = chrono::high_resolution_clock::now();
    double parTime = chrono::duration<double, milli>(endPar - startPar).count();
    
    cout << "  Sequential time: " << seqTime << " ms\n";
    cout << "  Parallel time (" << numThreads << " threads): " << parTime << " ms\n";
    cout << "  Speedup: " << (seqTime / parTime) << "x\n";
}

// Export methods

void Benchmark::exportToCSV(const string& filename) {
    ofstream file(outputDir + filename);
    if (!file.is_open()) {
        cerr << "Error: Could not create file " << filename << endl;
        return;
    }
    
    file << "Method,Test,InputSize,AvgTime(ms),MinTime(ms),MaxTime(ms),StdDev(ms),Iterations,Throughput(words/s)\n";
    
    for (const auto& r : results) {
        file << r.methodName << ","
             << r.testName << ","
             << r.inputSize << ","
             << r.avgTimeMs << ","
             << r.minTimeMs << ","
             << r.maxTimeMs << ","
             << r.stdDevMs << ","
             << r.iterations << ","
             << r.throughput << "\n";
    }
    
    file.close();
    cout << "Results exported to: " << outputDir << filename << endl;
}

void Benchmark::exportComparisonToCSV(const string& filename) {
    ofstream file(outputDir + filename);
    if (!file.is_open()) {
        cerr << "Error: Could not create file " << filename << endl;
        return;
    }
    
    file << "Word,TrieTime(ms),KDTreeTime(ms),AStarTime(ms),TrieSuggestions,KDTreeSuggestions,AStarSuggestions\n";
    
    for (const auto& c : comparisons) {
        file << c.word << ","
             << c.trieTimeMs << ","
             << c.kdtreeTimeMs << ","
             << c.astarTimeMs << ",\"";
        
        for (size_t i = 0; i < c.trieSuggestions.size(); i++) {
            if (i > 0) file << ";";
            file << c.trieSuggestions[i];
        }
        file << "\",\"";
        
        for (size_t i = 0; i < c.kdtreeSuggestions.size(); i++) {
            if (i > 0) file << ";";
            file << c.kdtreeSuggestions[i];
        }
        file << "\",\"";
        
        for (size_t i = 0; i < c.astarSuggestions.size(); i++) {
            if (i > 0) file << ";";
            file << c.astarSuggestions[i];
        }
        file << "\"\n";
    }
    
    file.close();
    cout << "Comparison exported to: " << outputDir << filename << endl;
}

void Benchmark::generateReport(const string& filename) {
    ofstream file(outputDir + filename);
    if (!file.is_open()) {
        cerr << "Error: Could not create file " << filename << endl;
        return;
    }
    
    file << "# Benchmark Report\n\n";
    file << "Generated: " << __DATE__ << " " << __TIME__ << "\n\n";
    
    file << "## Summary\n\n";
    file << "| Method | Avg Time (ms) | Throughput (words/s) |\n";
    file << "|--------|---------------|----------------------|\n";
    
    map<string, pair<double, double>> methodSummary;
    for (const auto& r : results) {
        if (r.testName == "single_word_lookup") {
            methodSummary[r.methodName] = {r.avgTimeMs, r.throughput};
        }
    }
    
    for (const auto& [method, stats] : methodSummary) {
        file << "| " << method << " | " << stats.first << " | " << stats.second << " |\n";
    }
    
    file << "\n## Method Comparison\n\n";
    file << "| Word | Best Method | Best Time (ms) |\n";
    file << "|------|-------------|----------------|\n";
    
    for (const auto& c : comparisons) {
        string best = "Trie";
        double bestTime = c.trieTimeMs;
        if (c.kdtreeTimeMs < bestTime) { best = "KD-Tree"; bestTime = c.kdtreeTimeMs; }
        if (c.astarTimeMs < bestTime) { best = "A*"; bestTime = c.astarTimeMs; }
        
        file << "| " << c.word << " | " << best << " | " << bestTime << " |\n";
    }
    
    file.close();
    cout << "Report generated: " << outputDir << filename << endl;
}

void Benchmark::printSummary() {
    cout << "\n";
    cout << "╔══════════════════════════════════════════════════════════════╗\n";
    cout << "║                    BENCHMARK SUMMARY                         ║\n";
    cout << "╚══════════════════════════════════════════════════════════════╝\n\n";
    
    // Group by method
    map<string, vector<BenchmarkResult>> byMethod;
    for (const auto& r : results) {
        byMethod[r.methodName].push_back(r);
    }
    
    cout << "┌──────────────┬──────────────┬──────────────┬──────────────┐\n";
    cout << "│    Method    │  Avg (ms)    │  Min (ms)    │  Max (ms)    │\n";
    cout << "├──────────────┼──────────────┼──────────────┼──────────────┤\n";
    
    for (const auto& [method, resultList] : byMethod) {
        double avgSum = 0, minVal = DBL_MAX, maxVal = 0;
        for (const auto& r : resultList) {
            avgSum += r.avgTimeMs;
            minVal = min(minVal, r.minTimeMs);
            maxVal = max(maxVal, r.maxTimeMs);
        }
        double avg = avgSum / resultList.size();
        
        cout << "│ " << setw(12) << left << method
             << " │ " << setw(12) << fixed << setprecision(4) << avg
             << " │ " << setw(12) << minVal
             << " │ " << setw(12) << maxVal << " │\n";
    }
    
    cout << "└──────────────┴──────────────┴──────────────┴──────────────┘\n";
    
    // Comparison summary
    if (!comparisons.empty()) {
        int trieWins = 0, kdWins = 0, astarWins = 0;
        for (const auto& c : comparisons) {
            double minTime = min({c.trieTimeMs, c.kdtreeTimeMs, c.astarTimeMs});
            if (c.trieTimeMs == minTime) trieWins++;
            else if (c.kdtreeTimeMs == minTime) kdWins++;
            else astarWins++;
        }
        
        cout << "\nMethod wins: Trie=" << trieWins 
             << ", KD-Tree=" << kdWins 
             << ", A*=" << astarWins << "\n";
    }
}
