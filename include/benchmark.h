#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <functional>
#include <numeric>
#include <cmath>
#include <thread>
#include <mutex>
#include <atomic>
#include "spellchecker.h"

using namespace std;

// Single benchmark result
struct BenchmarkResult {
    string methodName;
    string testName;
    int inputSize;
    double avgTimeMs;
    double minTimeMs;
    double maxTimeMs;
    double stdDevMs;
    int iterations;
    double throughput;  // words per second
    
    // For visualization compatibility
    double averageTimeMs;  // alias for avgTimeMs
    
    BenchmarkResult() : inputSize(0), avgTimeMs(0), minTimeMs(0), maxTimeMs(0), 
                        stdDevMs(0), iterations(0), throughput(0), averageTimeMs(0) {}
};

// Comparison between methods
struct MethodComparison {
    string word;
    string method;
    double timeMs;
    vector<string> suggestions;
    
    // Legacy fields
    double trieTimeMs;
    double kdtreeTimeMs;
    double astarTimeMs;
    vector<string> trieSuggestions;
    vector<string> kdtreeSuggestions;
    vector<string> astarSuggestions;
    
    MethodComparison() : timeMs(0), trieTimeMs(0), kdtreeTimeMs(0), astarTimeMs(0) {}
};

class Benchmark {
private:
    SpellChecker* checker;
    vector<BenchmarkResult> results;
    vector<MethodComparison> comparisons;
    string outputDir;
    
    // Timing helpers
    template<typename Func>
    double measureTime(Func&& func, int iterations = 1);
    
    // Statistics
    double calculateMean(const vector<double>& values);
    double calculateStdDev(const vector<double>& values, double mean);
    
public:
    Benchmark(SpellChecker* sc, const string& outDir = "benchmarks/results/data/");
    
    // Run benchmarks
    void runAllBenchmarks();
    void benchmarkSingleWordLookup(int iterations = 1000);
    void benchmarkTextProcessing(const vector<string>& testFiles);
    void benchmarkScalability(const vector<int>& dictionarySizes);
    void benchmarkMethodComparison(const vector<string>& testWords);
    
    // Parallel benchmarks
    void benchmarkParallelProcessing(const string& text, int numThreads);
    
    // Export results
    void exportToCSV(const string& filename);
    void exportComparisonToCSV(const string& filename);
    void generateReport(const string& filename);
    void printSummary();
    
    // Get results
    const vector<BenchmarkResult>& getResults() const { return results; }
    const vector<MethodComparison>& getComparisons() const { return comparisons; }
};

#endif // BENCHMARK_H
