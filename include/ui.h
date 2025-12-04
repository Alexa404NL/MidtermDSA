#ifndef UI_H
#define UI_H

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <fstream>
#include "spellchecker.h"

using namespace std;

class SpellCheckerUI {
private:
    SpellChecker* checker;
    bool isRunning;
    string currentMethod;
    int numThreads;
    
    // Menu helpers
    void clearScreen();
    void printHeader();
    void printMenu();
    void waitForEnter();
    
    // Menu options
    void checkSingleWord();
    void checkText();
    void checkFile();
    void compareMethods();
    void changeMethod();
    void loadCustomDictionary();
    void runBenchmarks();
    void showHelp();
    void showStatistics();
    
    // New features
    void runParallelProcessing();
    void runToneAnalysis();
    void showVisualization();
    void exportReport();
    
    // Display helpers
    void displayResults(const SpellCheckResult& result);
    void displayError(const SpellingError& error);
    void printColoredText(const string& text, const vector<SpellingError>& errors);
    
public:
    SpellCheckerUI();
    ~SpellCheckerUI();
    
    // Initialize with dictionary
    bool initialize(const string& dictionaryPath);
    
    // Main run loop
    void run();
    
    // Settings
    void setNumThreads(int n) { numThreads = n; }
};

#endif // UI_H
