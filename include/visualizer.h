#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include "benchmark.h"
#include "tone_analyzer.h"

using namespace std;

// Chart types
enum class ChartType {
    BAR_HORIZONTAL,
    BAR_VERTICAL,
    LINE,
    SCATTER,
    PIE,
    HISTOGRAM
};

// Data point for plotting
struct DataPoint {
    string label;
    double value;
    string color;  // For terminal colors
};

// Chart configuration
struct ChartConfig {
    string title;
    string xLabel;
    string yLabel;
    int width;
    int height;
    bool showValues;
    bool showGrid;
    string colorScheme;
};

class Visualizer {
private:
    string outputDir;
    
    // Terminal colors (ANSI codes)
    const string RESET = "\033[0m";
    const string RED = "\033[31m";
    const string GREEN = "\033[32m";
    const string YELLOW = "\033[33m";
    const string BLUE = "\033[34m";
    const string MAGENTA = "\033[35m";
    const string CYAN = "\033[36m";
    const string WHITE = "\033[37m";
    const string BOLD = "\033[1m";
    
    // ASCII characters for drawing
    const char BLOCK_FULL = '#';
    const char BLOCK_LIGHT = '-';
    const char VERTICAL = '|';
    const char HORIZONTAL = '-';
    const char CORNER = '+';
    const char DOT = '*';
    
    // Helper functions
    string getColor(int index);
    string repeatChar(char c, int count);
    double getMaxValue(const vector<DataPoint>& data);
    double getMinValue(const vector<DataPoint>& data);
    void printHorizontalLine(int width, char c = '-');
    
public:
    Visualizer(const string& outDir = "benchmarks/results/plots/");
    
    // ASCII Chart generation
    void drawBarChartHorizontal(const vector<DataPoint>& data, const ChartConfig& config);
    void drawBarChartVertical(const vector<DataPoint>& data, const ChartConfig& config);
    void drawLineChart(const vector<DataPoint>& data, const ChartConfig& config);
    void drawPieChart(const vector<DataPoint>& data, const ChartConfig& config);
    void drawHistogram(const vector<double>& values, int bins, const ChartConfig& config);
    
    // Benchmark visualization
    void visualizeBenchmarkResults(const vector<BenchmarkResult>& results);
    void visualizeMethodComparison(const vector<MethodComparison>& comparisons);
    void visualizeSpeedupAnalysis(double seqTime, double parTime, int threads);
    
    // Tone analysis visualization
    void visualizeToneAnalysis(const ToneAnalysisResult& result);
    void visualizeSentiment(const SentimentScore& sentiment);
    
    // Spell check visualization
    void visualizeSpellCheckResults(const SpellCheckResult& result);
    void visualizeErrorDistribution(const vector<SpellingError>& errors);
    
    // Export functions
    void exportToSVG(const vector<DataPoint>& data, const ChartConfig& config, const string& filename);
    void exportToHTML(const vector<DataPoint>& data, const ChartConfig& config, const string& filename);
    void generateDashboard(const vector<BenchmarkResult>& benchmarks, 
                           const ToneAnalysisResult* tone,
                           const SpellCheckResult* spellcheck,
                           const string& filename);
    
    // Utility
    void clearScreen();
    void printHeader(const string& title);
    void printTable(const vector<vector<string>>& data, const vector<string>& headers);
};

#endif // VISUALIZER_H
