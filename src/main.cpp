#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "../include/spellchecker.h"
#include "../include/ui.h"
#include "../include/benchmark.h"
#include "../include/parallel_processor.h"
#include "../include/tone_analyzer.h"
#include "../include/visualizer.h"

using namespace std;

void printUsage(const char* programName) {
    cout << "Usage: " << programName << " [options]\n\n";
    cout << "Options:\n";
    cout << "  --ui                  Launch interactive UI (default)\n";
    cout << "  --benchmark           Run benchmarks\n";
    cout << "  --check <word>        Check a single word\n";
    cout << "  --file <path>         Check a file\n";
    cout << "  --dict <path>         Specify dictionary file (default: data/dictionary.txt)\n";
    cout << "  --method <name>       Specify method: astar, trie, kdtree (default: astar)\n";
    cout << "  --parallel <file>     Process file with parallel spell checking\n";
    cout << "  --tone <file>         Analyze tone of a text file\n";
    cout << "  --visualize           Show visualization of benchmark results\n";
    cout << "  --export-html <file>  Export full report to HTML\n";
    cout << "  --threads <n>         Number of threads for parallel processing (default: 4)\n";
    cout << "  --help                Show this help message\n";
}

int main(int argc, char* argv[]) {
    string dictionaryPath = "data/dictionary.txt";
    string method = "astar";
    string mode = "ui";
    string targetWord = "";
    string targetFile = "";
    string exportFile = "";
    int numThreads = 4;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "--ui") {
            mode = "ui";
        } else if (arg == "--benchmark") {
            mode = "benchmark";
        } else if (arg == "--visualize") {
            mode = "visualize";
        } else if (arg == "--check" && i + 1 < argc) {
            mode = "check";
            targetWord = argv[++i];
        } else if (arg == "--file" && i + 1 < argc) {
            mode = "file";
            targetFile = argv[++i];
        } else if (arg == "--parallel" && i + 1 < argc) {
            mode = "parallel";
            targetFile = argv[++i];
        } else if (arg == "--tone" && i + 1 < argc) {
            mode = "tone";
            targetFile = argv[++i];
        } else if (arg == "--export-html" && i + 1 < argc) {
            exportFile = argv[++i];
        } else if (arg == "--dict" && i + 1 < argc) {
            dictionaryPath = argv[++i];
        } else if (arg == "--method" && i + 1 < argc) {
            method = argv[++i];
        } else if (arg == "--threads" && i + 1 < argc) {
            numThreads = stoi(argv[++i]);
        }
    }
    
    if (mode == "ui") {
        // Interactive UI mode
        SpellCheckerUI ui;
        if (ui.initialize(dictionaryPath)) {
            ui.run();
        } else {
            cerr << "Failed to initialize spell checker.\n";
            cerr << "Make sure the dictionary file exists at: " << dictionaryPath << "\n";
            return 1;
        }
    } else if (mode == "benchmark") {
        // Benchmark mode
        cout << "Initializing spell checker for benchmarks...\n";
        SpellChecker checker(2, 5);
        
        if (!checker.loadDictionary(dictionaryPath)) {
            cerr << "Warning: Could not load dictionary. Using empty dictionary.\n";
        }
        
        Benchmark bench(&checker);
        bench.runAllBenchmarks();
        bench.exportToCSV("benchmark_results.csv");
        bench.exportComparisonToCSV("method_comparison.csv");
        bench.generateReport("benchmark_report.md");
        
    } else if (mode == "check") {
        // Single word check mode
        SpellChecker checker(2, 5);
        checker.loadDictionary(dictionaryPath);
        checker.compareMethodsForWord(targetWord);
        
    } else if (mode == "file") {
        // File check mode
        SpellChecker checker(2, 5);
        checker.loadDictionary(dictionaryPath);
        
        SpellCheckResult result = checker.checkFile(targetFile, method);
        
        cout << "\n=== Spell Check Results ===\n";
        cout << "File: " << targetFile << "\n";
        cout << "Method: " << method << "\n";
        cout << "Total words: " << result.totalWords << "\n";
        cout << "Correct: " << result.correctWords << "\n";
        cout << "Errors: " << result.incorrectWords << "\n";
        cout << "Time: " << result.processingTimeMs << " ms\n\n";
        
        for (const auto& error : result.errors) {
            cout << "  Line " << error.lineNumber << ": \"" << error.originalWord << "\"\n";
            cout << "    Suggestions: ";
            for (const auto& s : error.suggestions) cout << s << " ";
            cout << "\n";
        }
    } else if (mode == "parallel") {
        // Parallel processing mode
        cout << "Loading dictionary from: " << dictionaryPath << "\n";
        SpellChecker checker(2, 5);
        checker.loadDictionary(dictionaryPath);
        
        cout << "Processing file with " << numThreads << " threads...\n\n";
        
        ParallelSpellChecker parallelChecker(&checker, numThreads);
        
        // Read file content and run comparison
        ifstream infile(targetFile);
        if (!infile.is_open()) {
            cerr << "Error: Could not open file " << targetFile << "\n";
            return 1;
        }
        stringstream buffer;
        buffer << infile.rdbuf();
        string text = buffer.str();
        infile.close();
        
        parallelChecker.compareSequentialVsParallel(text);
        
    } else if (mode == "tone") {
        // Tone analysis mode
        cout << "Analyzing tone of: " << targetFile << "\n\n";
        
        ToneAnalyzer analyzer;
        ToneAnalysisResult result = analyzer.analyzeFile(targetFile);
        analyzer.printAnalysis(result);
        
        // Export if requested
        if (!exportFile.empty()) {
            analyzer.exportAnalysis(result, exportFile);
        }
        
    } else if (mode == "visualize") {
        // Visualization mode - run benchmarks and visualize
        cout << "Running benchmarks with visualization...\n\n";
        
        SpellChecker checker(2, 5);
        checker.loadDictionary(dictionaryPath);
        
        Benchmark bench(&checker);
        bench.runAllBenchmarks();
        
        // Visualize results
        Visualizer viz;
        const vector<BenchmarkResult>& results = bench.getResults();
        viz.visualizeBenchmarkResults(results);
        
        // Also show method comparisons if available
        const vector<MethodComparison>& comparisons = bench.getComparisons();
        if (!comparisons.empty()) {
            viz.visualizeMethodComparison(comparisons);
        }
        
        // Export if requested
        if (!exportFile.empty()) {
            vector<DataPoint> dataPoints;
            for (const auto& r : results) {
                dataPoints.push_back({r.methodName, r.avgTimeMs, ""});
            }
            ChartConfig config;
            config.title = "Spell Checker Benchmark Results";
            config.width = 600;
            config.height = 400;
            viz.exportToHTML(dataPoints, config, exportFile);
        }
        
        bench.exportToCSV("benchmark_results.csv");
        bench.generateReport("benchmark_report.md");
    }
    
    return 0;
}
