#include "../include/ui.h"
#include "../include/parallel_processor.h"
#include "../include/tone_analyzer.h"
#include "../include/visualizer.h"

// Constructor and Destructor

SpellCheckerUI::SpellCheckerUI() : isRunning(false), currentMethod("astar"), numThreads(4) {
    checker = nullptr;
}

SpellCheckerUI::~SpellCheckerUI() {
    if (checker) {
        delete checker;
    }
}

// Private helper methods

void SpellCheckerUI::clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void SpellCheckerUI::printHeader() {
    cout << "\n";
    cout << "╔══════════════════════════════════════════════════════════════╗\n";
    cout << "║                    SPELL CHECKER SYSTEM                      ║\n";
    cout << "║         A* + Trie vs KD-Tree Implementation                  ║\n";
    cout << "║                   DSA Midterm Project                        ║\n";
    cout << "╚══════════════════════════════════════════════════════════════╝\n";
    cout << "\n";
    cout << "Current method: " << currentMethod << "\n";
    cout << "────────────────────────────────────────────────────────────────\n";
}

void SpellCheckerUI::printMenu() {
    cout << "\n";
    cout << "┌─────────────────── MAIN MENU ───────────────────┐\n";
    cout << "│                                                 │\n";
    cout << "│  1. Check a single word                         │\n";
    cout << "│  2. Check text (enter text)                     │\n";
    cout << "│  3. Check file                                  │\n";
    cout << "│  4. Compare methods for a word                  │\n";
    cout << "│  5. Change search method                        │\n";
    cout << "│  6. Load custom dictionary                      │\n";
    cout << "│  7. Run benchmarks                              │\n";
    cout << "│  8. Show statistics                             │\n";
    cout << "│  9. Help                                        │\n";
    cout << "│  ─────────── Advanced Features ───────────      │\n";
    cout << "│  10. Parallel file processing                   │\n";
    cout << "│  11. Tone analysis                              │\n";
    cout << "│  12. Visualization                              │\n";
    cout << "│  13. Export HTML report                         │\n";
    cout << "│  0. Exit                                        │\n";
    cout << "│                                                 │\n";
    cout << "└─────────────────────────────────────────────────┘\n";
    cout << "\n";
    cout << "Enter your choice: ";
}

void SpellCheckerUI::waitForEnter() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// Menu options implementation

void SpellCheckerUI::checkSingleWord() {
    cout << "\n=== Check Single Word ===\n";
    cout << "Enter a word: ";
    string word;
    cin >> word;
    
    if (checker->isValidWord(word)) {
        cout << "\n✓ \"" << word << "\" is spelled correctly!\n";
    } else {
        cout << "\n✗ \"" << word << "\" is not in the dictionary.\n";
        cout << "\nSuggestions (" << currentMethod << "):\n";
        
        vector<string> suggestions;
        if (currentMethod == "trie") {
            suggestions = checker->getSuggestionsTrie(word);
        } else if (currentMethod == "kdtree") {
            suggestions = checker->getSuggestionsKDTree(word);
        } else {
            suggestions = checker->getSuggestionsAStar(word);
        }
        
        if (suggestions.empty()) {
            cout << "  No suggestions found.\n";
        } else {
            for (size_t i = 0; i < suggestions.size(); i++) {
                cout << "  " << (i + 1) << ". " << suggestions[i] << "\n";
            }
        }
    }
}

void SpellCheckerUI::checkText() {
    cout << "\n=== Check Text ===\n";
    cout << "Enter text (press Enter twice to finish):\n";
    cin.ignore();
    
    string text, line;
    while (getline(cin, line) && !line.empty()) {
        text += line + "\n";
    }
    
    if (text.empty()) {
        cout << "No text entered.\n";
        return;
    }
    
    SpellCheckResult result = checker->checkText(text, currentMethod);
    displayResults(result);
}

void SpellCheckerUI::checkFile() {
    cout << "\n=== Check File ===\n";
    cout << "Enter file path: ";
    string filepath;
    cin >> filepath;
    
    SpellCheckResult result = checker->checkFile(filepath, currentMethod);
    
    if (result.totalWords == 0) {
        cout << "Could not read file or file is empty.\n";
        return;
    }
    
    displayResults(result);
}

void SpellCheckerUI::compareMethods() {
    cout << "\n=== Compare Methods ===\n";
    cout << "Enter a word to compare: ";
    string word;
    cin >> word;
    
    checker->compareMethodsForWord(word);
}

void SpellCheckerUI::changeMethod() {
    cout << "\n=== Change Search Method ===\n";
    cout << "Current method: " << currentMethod << "\n\n";
    cout << "Available methods:\n";
    cout << "  1. astar  - A* search with Levenshtein distance (recommended)\n";
    cout << "  2. trie   - Direct Trie traversal with Levenshtein\n";
    cout << "  3. kdtree - KD-Tree semantic similarity\n";
    cout << "\nEnter method number (1-3): ";
    
    int choice;
    cin >> choice;
    
    switch (choice) {
        case 1:
            currentMethod = "astar";
            cout << "Method changed to: A* Search\n";
            break;
        case 2:
            currentMethod = "trie";
            cout << "Method changed to: Trie (Levenshtein)\n";
            break;
        case 3:
            currentMethod = "kdtree";
            cout << "Method changed to: KD-Tree (Semantic)\n";
            break;
        default:
            cout << "Invalid choice. Method unchanged.\n";
    }
}

void SpellCheckerUI::loadCustomDictionary() {
    cout << "\n=== Load Custom Dictionary ===\n";
    cout << "Enter dictionary file path: ";
    string filepath;
    cin >> filepath;
    
    if (checker->loadDictionary(filepath)) {
        cout << "Dictionary loaded successfully!\n";
    } else {
        cout << "Failed to load dictionary.\n";
    }
}

void SpellCheckerUI::runBenchmarks() {
    cout << "\n=== Run Benchmarks ===\n";
    cout << "This feature runs performance tests on all three methods.\n";
    cout << "Do you want to proceed? (y/n): ";
    
    char choice;
    cin >> choice;
    
    if (choice != 'y' && choice != 'Y') {
        cout << "Benchmark cancelled.\n";
        return;
    }
    
    cout << "\nRunning benchmarks...\n";
    
    // Test words
    vector<string> testWords = {
        "helo", "wrold", "programing", "algoritm", "speling",
        "recieve", "occured", "seperate", "definately", "accomodate"
    };
    
    cout << "\n┌────────────────────────────────────────────────────────────────────┐\n";
    cout << "│                      BENCHMARK RESULTS                              │\n";
    cout << "├──────────────┬──────────────┬──────────────┬──────────────┬─────────┤\n";
    cout << "│    Word      │   Trie (ms)  │ KD-Tree (ms) │  A* (ms)     │ Winner  │\n";
    cout << "├──────────────┼──────────────┼──────────────┼──────────────┼─────────┤\n";
    
    double totalTrie = 0, totalKD = 0, totalAStar = 0;
    
    for (const string& word : testWords) {
        // Trie
        auto start = chrono::high_resolution_clock::now();
        checker->getSuggestionsTrie(word);
        auto end = chrono::high_resolution_clock::now();
        double trieTime = chrono::duration<double, milli>(end - start).count();
        
        // KD-Tree
        start = chrono::high_resolution_clock::now();
        checker->getSuggestionsKDTree(word);
        end = chrono::high_resolution_clock::now();
        double kdTime = chrono::duration<double, milli>(end - start).count();
        
        // A*
        start = chrono::high_resolution_clock::now();
        checker->getSuggestionsAStar(word);
        end = chrono::high_resolution_clock::now();
        double astarTime = chrono::duration<double, milli>(end - start).count();
        
        totalTrie += trieTime;
        totalKD += kdTime;
        totalAStar += astarTime;
        
        string winner = "Trie";
        double minTime = trieTime;
        if (kdTime < minTime) { winner = "KD-Tree"; minTime = kdTime; }
        if (astarTime < minTime) { winner = "A*"; }
        
        cout << "│ " << setw(12) << left << word.substr(0, 12) 
             << " │ " << setw(12) << fixed << setprecision(4) << trieTime
             << " │ " << setw(12) << kdTime
             << " │ " << setw(12) << astarTime
             << " │ " << setw(7) << winner << " │\n";
    }
    
    cout << "├──────────────┼──────────────┼──────────────┼──────────────┼─────────┤\n";
    cout << "│ " << setw(12) << "TOTAL"
         << " │ " << setw(12) << totalTrie
         << " │ " << setw(12) << totalKD
         << " │ " << setw(12) << totalAStar
         << " │         │\n";
    cout << "└──────────────┴──────────────┴──────────────┴──────────────┴─────────┘\n";
    
    cout << "\nSummary:\n";
    cout << "  - Trie average: " << (totalTrie / testWords.size()) << " ms\n";
    cout << "  - KD-Tree average: " << (totalKD / testWords.size()) << " ms\n";
    cout << "  - A* average: " << (totalAStar / testWords.size()) << " ms\n";
}

void SpellCheckerUI::showHelp() {
    cout << "\n";
    cout << "╔══════════════════════════════════════════════════════════════╗\n";
    cout << "║                         HELP                                 ║\n";
    cout << "╚══════════════════════════════════════════════════════════════╝\n";
    cout << "\n";
    cout << "This spell checker uses three different algorithms:\n\n";
    cout << "1. A* SEARCH (Default)\n";
    cout << "   Uses A* algorithm to traverse the Trie with Levenshtein\n";
    cout << "   distance as the cost function. Provides optimal results\n";
    cout << "   with efficient pruning.\n\n";
    cout << "2. TRIE (Levenshtein)\n";
    cout << "   Direct Trie traversal computing Levenshtein distance\n";
    cout << "   incrementally. Fast for small edit distances.\n\n";
    cout << "3. KD-TREE (Semantic)\n";
    cout << "   Uses word embeddings based on character features to find\n";
    cout << "   semantically similar words. Good for finding words with\n";
    cout << "   similar structure.\n\n";
    cout << "TIPS:\n";
    cout << "  - Use A* for best accuracy\n";
    cout << "  - Use KD-Tree for semantic suggestions\n";
    cout << "  - Compare methods to see differences\n";
    cout << "  - Load a larger dictionary for better results\n";
}

void SpellCheckerUI::showStatistics() {
    cout << "\n=== Dictionary Statistics ===\n";
    cout << "Current search method: " << currentMethod << "\n";
    cout << "Max edit distance: 2\n";
    cout << "Max suggestions: 5\n";
    cout << "Number of threads: " << numThreads << "\n";
    cout << "\nNote: Load a dictionary to see word count.\n";
}

// New feature implementations

void SpellCheckerUI::runParallelProcessing() {
    cout << "\n=== Parallel File Processing ===\n";
    cout << "Enter file path to process: ";
    string filepath;
    cin >> filepath;
    
    cout << "Enter number of threads (1-16, default=" << numThreads << "): ";
    int threads;
    cin >> threads;
    
    if (threads < 1 || threads > 16) {
        threads = numThreads;
        cout << "Using default: " << threads << " threads\n";
    }
    
    cout << "\nProcessing with " << threads << " threads...\n";
    
    // Read file content
    ifstream file(filepath);
    if (!file.is_open()) {
        cout << "Error: Could not open file " << filepath << "\n";
        return;
    }
    
    stringstream buffer;
    buffer << file.rdbuf();
    string text = buffer.str();
    file.close();
    
    ParallelSpellChecker parallelChecker(checker, threads);
    parallelChecker.compareSequentialVsParallel(text);
}

void SpellCheckerUI::runToneAnalysis() {
    cout << "\n=== Tone Analysis ===\n";
    cout << "Options:\n";
    cout << "  1. Analyze text input\n";
    cout << "  2. Analyze file\n";
    cout << "Enter choice: ";
    
    int choice;
    cin >> choice;
    
    ToneAnalyzer analyzer;
    ToneAnalysisResult result;
    
    if (choice == 1) {
        cout << "\nEnter text to analyze (press Enter twice to finish):\n";
        cin.ignore();
        
        string text, line;
        while (getline(cin, line) && !line.empty()) {
            text += line + "\n";
        }
        
        if (text.empty()) {
            cout << "No text entered.\n";
            return;
        }
        
        result = analyzer.analyzeText(text);
    } else {
        cout << "Enter file path: ";
        string filepath;
        cin >> filepath;
        result = analyzer.analyzeFile(filepath);
    }
    
    analyzer.printAnalysis(result);
    
    cout << "\nExport analysis to file? (y/n): ";
    char exportChoice;
    cin >> exportChoice;
    
    if (exportChoice == 'y' || exportChoice == 'Y') {
        cout << "Enter output filename: ";
        string outfile;
        cin >> outfile;
        analyzer.exportAnalysis(result, outfile);
    }
}

void SpellCheckerUI::showVisualization() {
    cout << "\n=== Visualization ===\n";
    cout << "Running benchmark visualization...\n\n";
    
    // Run quick benchmarks
    vector<string> testWords = {
        "helo", "wrold", "programing", "algoritm", "speling"
    };
    
    double totalTrie = 0, totalKD = 0, totalAStar = 0;
    int trieCorrect = 0, kdCorrect = 0, astarCorrect = 0;
    
    for (const string& word : testWords) {
        auto start = chrono::high_resolution_clock::now();
        auto trieSugg = checker->getSuggestionsTrie(word);
        auto end = chrono::high_resolution_clock::now();
        totalTrie += chrono::duration<double, milli>(end - start).count();
        if (!trieSugg.empty()) trieCorrect++;
        
        start = chrono::high_resolution_clock::now();
        auto kdSugg = checker->getSuggestionsKDTree(word);
        end = chrono::high_resolution_clock::now();
        totalKD += chrono::duration<double, milli>(end - start).count();
        if (!kdSugg.empty()) kdCorrect++;
        
        start = chrono::high_resolution_clock::now();
        auto astarSugg = checker->getSuggestionsAStar(word);
        end = chrono::high_resolution_clock::now();
        totalAStar += chrono::duration<double, milli>(end - start).count();
        if (!astarSugg.empty()) astarCorrect++;
    }
    
    // Display ASCII visualization
    Visualizer viz;
    
    vector<DataPoint> timeData = {
        {"A* Search", totalAStar / testWords.size(), ""},
        {"Trie", totalTrie / testWords.size(), ""},
        {"KD-Tree", totalKD / testWords.size(), ""}
    };
    
    ChartConfig config;
    config.title = "Average Execution Time (ms)";
    config.xLabel = "Method";
    config.yLabel = "Time (ms)";
    config.width = 60;
    config.height = 15;
    config.showValues = true;
    config.showGrid = false;
    
    viz.drawBarChartHorizontal(timeData, config);
    
    // Summary
    cout << "\n=== SUMMARY ===\n";
    cout << "A* Search: " << fixed << setprecision(4) << (totalAStar / testWords.size()) << " ms avg\n";
    cout << "Trie:      " << (totalTrie / testWords.size()) << " ms avg\n";
    cout << "KD-Tree:   " << (totalKD / testWords.size()) << " ms avg\n";
}

void SpellCheckerUI::exportReport() {
    cout << "\n=== Export HTML Report ===\n";
    cout << "This will generate a comprehensive HTML report with:\n";
    cout << "  - Benchmark results\n";
    cout << "  - Method comparisons\n";
    cout << "  - Tone analysis (if performed)\n";
    cout << "\nEnter output filename (e.g., report.html): ";
    
    string filename;
    cin >> filename;
    
    // Run benchmarks
    vector<string> testWords = {
        "helo", "wrold", "programing", "algoritm", "speling",
        "recieve", "occured", "seperate", "definately", "accomodate"
    };
    
    double totalTrie = 0, totalKD = 0, totalAStar = 0;
    
    cout << "\nRunning benchmarks for report...\n";
    
    for (const string& word : testWords) {
        auto start = chrono::high_resolution_clock::now();
        checker->getSuggestionsTrie(word);
        auto end = chrono::high_resolution_clock::now();
        totalTrie += chrono::duration<double, milli>(end - start).count();
        
        start = chrono::high_resolution_clock::now();
        checker->getSuggestionsKDTree(word);
        end = chrono::high_resolution_clock::now();
        totalKD += chrono::duration<double, milli>(end - start).count();
        
        start = chrono::high_resolution_clock::now();
        checker->getSuggestionsAStar(word);
        end = chrono::high_resolution_clock::now();
        totalAStar += chrono::duration<double, milli>(end - start).count();
    }
    
    // Create benchmark data
    vector<DataPoint> benchData = {
        {"A* Search", totalAStar / testWords.size(), ""},
        {"Trie", totalTrie / testWords.size(), ""},
        {"KD-Tree", totalKD / testWords.size(), ""}
    };
    
    ChartConfig config;
    config.title = "Spell Checker Benchmark Results";
    config.xLabel = "Method";
    config.yLabel = "Time (ms)";
    config.width = 600;
    config.height = 400;
    config.showValues = true;
    
    Visualizer viz;
    viz.exportToHTML(benchData, config, filename);
    
    cout << "\n✓ Report exported successfully to: " << filename << "\n";
}

// Display helpers

void SpellCheckerUI::displayResults(const SpellCheckResult& result) {
    cout << "\n";
    cout << "╔══════════════════════════════════════════════════════════════╗\n";
    cout << "║                    SPELL CHECK RESULTS                       ║\n";
    cout << "╚══════════════════════════════════════════════════════════════╝\n";
    cout << "\n";
    cout << "  Total words:     " << result.totalWords << "\n";
    cout << "  Correct words:   " << result.correctWords << " (" 
         << fixed << setprecision(1) 
         << (result.totalWords > 0 ? (100.0 * result.correctWords / result.totalWords) : 0) 
         << "%)\n";
    cout << "  Spelling errors: " << result.incorrectWords << "\n";
    cout << "  Processing time: " << fixed << setprecision(2) << result.processingTimeMs << " ms\n";
    cout << "\n";
    
    if (!result.errors.empty()) {
        cout << "────────────────── ERRORS FOUND ──────────────────\n\n";
        for (const auto& error : result.errors) {
            displayError(error);
        }
    } else {
        cout << "✓ No spelling errors found!\n";
    }
}

void SpellCheckerUI::displayError(const SpellingError& error) {
    cout << "  ✗ \"" << error.originalWord << "\" (line " << error.lineNumber << ")\n";
    cout << "    Suggestions: ";
    
    if (error.suggestions.empty()) {
        cout << "(none)";
    } else {
        for (size_t i = 0; i < error.suggestions.size(); i++) {
            if (i > 0) cout << ", ";
            cout << error.suggestions[i];
        }
    }
    cout << "\n\n";
}

void SpellCheckerUI::printColoredText(const string& text, const vector<SpellingError>& errors) {
    // This would require ANSI escape codes for colored output
    // For simplicity, just print the text with markers
    cout << text << "\n";
}

// Public methods

bool SpellCheckerUI::initialize(const string& dictionaryPath) {
    checker = new SpellChecker(2, 5);  // maxDist=2, maxSuggestions=5
    
    if (!dictionaryPath.empty()) {
        return checker->loadDictionary(dictionaryPath);
    }
    
    return true;
}

void SpellCheckerUI::run() {
    isRunning = true;
    
    while (isRunning) {
        clearScreen();
        printHeader();
        printMenu();
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1:
                checkSingleWord();
                waitForEnter();
                break;
            case 2:
                checkText();
                waitForEnter();
                break;
            case 3:
                checkFile();
                waitForEnter();
                break;
            case 4:
                compareMethods();
                waitForEnter();
                break;
            case 5:
                changeMethod();
                waitForEnter();
                break;
            case 6:
                loadCustomDictionary();
                waitForEnter();
                break;
            case 7:
                runBenchmarks();
                waitForEnter();
                break;
            case 8:
                showStatistics();
                waitForEnter();
                break;
            case 9:
                showHelp();
                waitForEnter();
                break;
            case 10:
                runParallelProcessing();
                waitForEnter();
                break;
            case 11:
                runToneAnalysis();
                waitForEnter();
                break;
            case 12:
                showVisualization();
                waitForEnter();
                break;
            case 13:
                exportReport();
                waitForEnter();
                break;
            case 0:
                isRunning = false;
                cout << "\nThank you for using Spell Checker!\n";
                break;
            default:
                cout << "\nInvalid choice. Please try again.\n";
                waitForEnter();
        }
    }
}
