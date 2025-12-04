#include "../include/visualizer.h"

// Constructor
Visualizer::Visualizer(const string& outDir) : outputDir(outDir) {}

// Helper functions
string Visualizer::getColor(int index) {
    vector<string> colors = {RED, GREEN, BLUE, YELLOW, MAGENTA, CYAN};
    return colors[index % colors.size()];
}

string Visualizer::repeatChar(char c, int count) {
    return count > 0 ? string(count, c) : "";
}

double Visualizer::getMaxValue(const vector<DataPoint>& data) {
    double maxVal = 0;
    for (const auto& dp : data) {
        if (dp.value > maxVal) maxVal = dp.value;
    }
    return maxVal > 0 ? maxVal : 1;
}

double Visualizer::getMinValue(const vector<DataPoint>& data) {
    if (data.empty()) return 0;
    double minVal = data[0].value;
    for (const auto& dp : data) {
        if (dp.value < minVal) minVal = dp.value;
    }
    return minVal;
}

void Visualizer::printHorizontalLine(int width, char c) {
    cout << repeatChar(c, width) << "\n";
}

// Clear screen
void Visualizer::clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void Visualizer::printHeader(const string& title) {
    cout << "\n";
    cout << "+" << repeatChar('=', title.length() + 4) << "+\n";
    cout << "|  " << BOLD << title << RESET << "  |\n";
    cout << "+" << repeatChar('=', title.length() + 4) << "+\n";
    cout << "\n";
}

// ASCII Chart generation
void Visualizer::drawBarChartHorizontal(const vector<DataPoint>& data, const ChartConfig& config) {
    if (data.empty()) {
        cout << "No data to display.\n";
        return;
    }
    
    printHeader(config.title);
    
    double maxVal = getMaxValue(data);
    size_t maxLabelLen = 0;
    for (const auto& dp : data) {
        if (dp.label.length() > maxLabelLen) maxLabelLen = dp.label.length();
    }
    
    int barMaxLen = config.width - maxLabelLen - 20;
    if (barMaxLen < 10) barMaxLen = 10;
    
    for (size_t i = 0; i < data.size(); i++) {
        const auto& dp = data[i];
        int barLen = (int)((dp.value / maxVal) * barMaxLen);
        
        cout << getColor(i) << setw(maxLabelLen) << right << dp.label << RESET;
        cout << " |";
        cout << getColor(i) << repeatChar(BLOCK_FULL, barLen) << RESET;
        
        if (config.showValues) {
            cout << " " << fixed << setprecision(4) << dp.value;
        }
        cout << "\n";
    }
    
    // X-axis
    cout << repeatChar(' ', maxLabelLen) << " +" << repeatChar(HORIZONTAL, barMaxLen) << "\n";
    
    // Scale
    cout << repeatChar(' ', maxLabelLen + 2) << "0";
    cout << repeatChar(' ', barMaxLen / 2 - 1) << fixed << setprecision(2) << (maxVal / 2);
    cout << repeatChar(' ', barMaxLen / 2 - 3) << maxVal << "\n";
}

void Visualizer::drawBarChartVertical(const vector<DataPoint>& data, const ChartConfig& config) {
    if (data.empty()) {
        cout << "No data to display.\n";
        return;
    }
    
    printHeader(config.title);
    
    double maxVal = getMaxValue(data);
    int barWidth = (config.width - 10) / max(1, (int)data.size());
    
    // Draw from top to bottom
    for (int row = config.height; row >= 0; row--) {
        double threshold = (row * maxVal) / config.height;
        
        // Y-axis label
        if (row == config.height || row == config.height / 2 || row == 0) {
            cout << setw(8) << fixed << setprecision(2) << threshold << " |";
        } else {
            cout << "         |";
        }
        
        // Bars
        for (size_t i = 0; i < data.size(); i++) {
            if (data[i].value >= threshold && threshold > 0) {
                cout << getColor(i) << repeatChar(BLOCK_FULL, barWidth - 1) << RESET << " ";
            } else {
                cout << repeatChar(' ', barWidth);
            }
        }
        cout << "\n";
    }
    
    // X-axis
    cout << "         +" << repeatChar(HORIZONTAL, barWidth * data.size()) << "\n";
    
    // Labels
    cout << "          ";
    for (const auto& dp : data) {
        cout << setw(barWidth) << left << dp.label.substr(0, barWidth - 1);
    }
    cout << "\n";
}

void Visualizer::drawLineChart(const vector<DataPoint>& data, const ChartConfig& config) {
    if (data.empty()) {
        cout << "No data to display.\n";
        return;
    }
    
    printHeader(config.title);
    
    double maxVal = getMaxValue(data);
    double minVal = getMinValue(data);
    double range = maxVal - minVal;
    if (range == 0) range = 1;
    
    // Create grid
    vector<string> grid(config.height, string(config.width, ' '));
    
    int pointSpacing = config.width / max(1, (int)data.size() - 1);
    
    // Plot points and lines
    for (size_t i = 0; i < data.size(); i++) {
        int x = i * pointSpacing;
        int y = config.height - 1 - (int)(((data[i].value - minVal) / range) * (config.height - 1));
        
        if (x >= 0 && x < config.width && y >= 0 && y < config.height) {
            grid[y][x] = DOT;
            
            // Connect to previous point
            if (i > 0) {
                int prevX = (i - 1) * pointSpacing;
                int prevY = config.height - 1 - (int)(((data[i-1].value - minVal) / range) * (config.height - 1));
                
                int steps = abs(x - prevX);
                for (int s = 1; s < steps; s++) {
                    int lineX = prevX + s;
                    int lineY = prevY + (y - prevY) * s / steps;
                    if (lineY >= 0 && lineY < config.height && lineX >= 0 && lineX < config.width) {
                        if (grid[lineY][lineX] == ' ') grid[lineY][lineX] = '-';
                    }
                }
            }
        }
    }
    
    // Print grid with Y-axis
    for (int y = 0; y < config.height; y++) {
        double value = maxVal - (y * range / (config.height - 1));
        cout << setw(8) << fixed << setprecision(2) << value << " |" << grid[y] << "\n";
    }
    
    // X-axis
    cout << "         +" << repeatChar(HORIZONTAL, config.width) << "\n";
    
    // X labels (show first and last)
    if (!data.empty()) {
        cout << "          " << data.front().label;
        cout << repeatChar(' ', config.width - data.front().label.length() - data.back().label.length());
        cout << data.back().label << "\n";
    }
}

void Visualizer::drawPieChart(const vector<DataPoint>& data, const ChartConfig& config) {
    if (data.empty()) {
        cout << "No data to display.\n";
        return;
    }
    
    printHeader(config.title);
    
    // Calculate total
    double total = 0;
    for (const auto& dp : data) {
        total += dp.value;
    }
    if (total == 0) total = 1;
    
    // Display as percentage bars (simplified pie representation)
    cout << "Distribution:\n\n";
    
    for (size_t i = 0; i < data.size(); i++) {
        double percentage = (data[i].value / total) * 100;
        int barLen = (int)(percentage / 2);  // 50 chars = 100%
        
        cout << getColor(i) << setw(15) << left << data[i].label << RESET;
        cout << " [" << repeatChar(BLOCK_FULL, barLen) << repeatChar(' ', 50 - barLen) << "] ";
        cout << fixed << setprecision(1) << percentage << "%\n";
    }
    
    cout << "\nTotal: " << fixed << setprecision(2) << total << "\n";
}

void Visualizer::drawHistogram(const vector<double>& values, int bins, const ChartConfig& config) {
    if (values.empty()) {
        cout << "No data to display.\n";
        return;
    }
    
    printHeader(config.title);
    
    // Find min/max
    double minVal = *min_element(values.begin(), values.end());
    double maxVal = *max_element(values.begin(), values.end());
    double range = maxVal - minVal;
    if (range == 0) range = 1;
    
    // Create bins
    vector<int> binCounts(bins, 0);
    for (double val : values) {
        int bin = (int)((val - minVal) / range * (bins - 1));
        bin = max(0, min(bins - 1, bin));
        binCounts[bin]++;
    }
    
    int maxBin = *max_element(binCounts.begin(), binCounts.end());
    if (maxBin == 0) maxBin = 1;
    
    int barMaxLen = config.width - 25;
    
    for (int i = 0; i < bins; i++) {
        double binStart = minVal + i * range / bins;
        double binEnd = minVal + (i + 1) * range / bins;
        int barLen = binCounts[i] * barMaxLen / maxBin;
        
        cout << setw(6) << fixed << setprecision(1) << binStart;
        cout << "-" << setw(6) << binEnd << " |";
        cout << GREEN << repeatChar(BLOCK_FULL, barLen) << RESET;
        cout << " (" << binCounts[i] << ")\n";
    }
    
    cout << "\nN = " << values.size() << "\n";
}

// Benchmark visualization
void Visualizer::visualizeBenchmarkResults(const vector<BenchmarkResult>& results) {
    printHeader("BENCHMARK RESULTS");
    
    if (results.empty()) {
        cout << "No benchmark results to display.\n";
        return;
    }
    
    // Convert to DataPoints for bar chart
    vector<DataPoint> timeData;
    for (const auto& r : results) {
        timeData.push_back({r.methodName, r.averageTimeMs, ""});
    }
    
    ChartConfig config;
    config.title = "Average Execution Time (ms)";
    config.width = 60;
    config.showValues = true;
    
    drawBarChartHorizontal(timeData, config);
    
    // Detailed table
    cout << "\n+------------------------------------------------------------------+\n";
    cout << "|                    DETAILED STATISTICS                           |\n";
    cout << "+---------------+------------+-----------+----------+-------------+\n";
    cout << "| Method        | Avg (ms)   | Min (ms)  | Max (ms) | Iterations  |\n";
    cout << "+---------------+------------+-----------+----------+-------------+\n";
    
    for (const auto& r : results) {
        cout << "| " << setw(13) << left << r.methodName << " | ";
        cout << setw(10) << fixed << setprecision(4) << r.averageTimeMs << " | ";
        cout << setw(9) << r.minTimeMs << " | ";
        cout << setw(8) << r.maxTimeMs << " | ";
        cout << setw(11) << r.iterations << " |\n";
    }
    
    cout << "+---------------+------------+-----------+----------+-------------+\n";
}

void Visualizer::visualizeMethodComparison(const vector<MethodComparison>& comparisons) {
    printHeader("METHOD COMPARISON");
    
    for (const auto& comp : comparisons) {
        cout << "Word: \"" << comp.word << "\"\n";
        cout << "  Method: " << comp.method << "\n";
        cout << "  Time: " << fixed << setprecision(4) << comp.timeMs << " ms\n";
        cout << "  Suggestions: ";
        for (size_t i = 0; i < min((size_t)5, comp.suggestions.size()); i++) {
            if (i > 0) cout << ", ";
            cout << comp.suggestions[i];
        }
        cout << "\n\n";
    }
}

void Visualizer::visualizeSpeedupAnalysis(double seqTime, double parTime, int threads) {
    printHeader("PARALLEL SPEEDUP ANALYSIS");
    
    double speedup = seqTime / parTime;
    double efficiency = speedup / threads * 100;
    
    cout << "+----------------------------------------------------------+\n";
    cout << "|  Sequential Time:    " << setw(15) << fixed << setprecision(4) << seqTime << " ms             |\n";
    cout << "|  Parallel Time:      " << setw(15) << parTime << " ms             |\n";
    cout << "|  Threads:            " << setw(15) << threads << "                |\n";
    cout << "+----------------------------------------------------------+\n";
    cout << "|  Speedup:            " << setw(15) << setprecision(2) << speedup << "x              |\n";
    cout << "|  Efficiency:         " << setw(15) << efficiency << "%             |\n";
    cout << "+----------------------------------------------------------+\n";
    
    // Speedup visualization
    int barLen = (int)(speedup * 10);
    cout << "\nSpeedup: [" << GREEN << repeatChar(BLOCK_FULL, min(50, barLen)) << RESET;
    cout << repeatChar(' ', max(0, 50 - barLen)) << "] " << speedup << "x\n";
}

// Tone analysis visualization
void Visualizer::visualizeToneAnalysis(const ToneAnalysisResult& result) {
    printHeader("TONE ANALYSIS");
    
    // Text statistics
    cout << "Text Statistics:\n";
    cout << "  Word Count:      " << result.wordCount << "\n";
    cout << "  Sentence Count:  " << result.sentenceCount << "\n";
    cout << "  Avg Word Length: " << fixed << setprecision(1) << result.avgWordLength << "\n";
    cout << "  Reading Level:   Grade " << (int)result.readabilityScore << "\n\n";
    
    // Sentiment
    visualizeSentiment(result.sentiment);
    
    // Tone scores
    cout << "\nTone Breakdown:\n";
    for (const auto& [tone, score] : result.toneScores) {
        int barLen = (int)(score * 30);
        string toneName;
        switch (tone) {
            case Tone::POSITIVE: toneName = "Positive"; break;
            case Tone::NEGATIVE: toneName = "Negative"; break;
            case Tone::NEUTRAL: toneName = "Neutral"; break;
            case Tone::FORMAL: toneName = "Formal"; break;
            case Tone::INFORMAL: toneName = "Informal"; break;
            case Tone::ACADEMIC: toneName = "Academic"; break;
            case Tone::EMOTIONAL: toneName = "Emotional"; break;
            case Tone::OBJECTIVE: toneName = "Objective"; break;
            default: toneName = "Unknown";
        }
        cout << "  " << setw(12) << left << toneName;
        cout << " [" << repeatChar(BLOCK_FULL, barLen) << repeatChar(' ', 30 - barLen) << "] ";
        cout << fixed << setprecision(0) << (score * 100) << "%\n";
    }
    
    // Dominant tone
    cout << "\nDominant Tone: " << BOLD << result.dominantTone << RESET << "\n";
    
    // Keywords
    if (!result.keywords.empty()) {
        cout << "\nKeywords: ";
        for (size_t i = 0; i < result.keywords.size(); i++) {
            if (i > 0) cout << ", ";
            cout << result.keywords[i];
        }
        cout << "\n";
    }
    
    // Summary
    cout << "\nSummary:\n  " << result.summary << "\n";
}

void Visualizer::visualizeSentiment(const SentimentScore& sentiment) {
    cout << "Sentiment Analysis:\n";
    
    // Overall sentiment
    cout << "  Overall: ";
    if (sentiment.compound > 0.3) {
        cout << GREEN << "POSITIVE" << RESET;
    } else if (sentiment.compound < -0.3) {
        cout << RED << "NEGATIVE" << RESET;
    } else {
        cout << YELLOW << "NEUTRAL" << RESET;
    }
    cout << " (compound: " << fixed << setprecision(3) << sentiment.compound << ")\n";
    
    // Breakdown bars
    cout << "  Positive: [" << GREEN << repeatChar(BLOCK_FULL, (int)(sentiment.positive * 30)) << RESET;
    cout << repeatChar(' ', 30 - (int)(sentiment.positive * 30)) << "] " << setprecision(0) << (sentiment.positive * 100) << "%\n";
    
    cout << "  Negative: [" << RED << repeatChar(BLOCK_FULL, (int)(sentiment.negative * 30)) << RESET;
    cout << repeatChar(' ', 30 - (int)(sentiment.negative * 30)) << "] " << (sentiment.negative * 100) << "%\n";
    
    cout << "  Neutral:  [" << YELLOW << repeatChar(BLOCK_FULL, (int)(sentiment.neutral * 30)) << RESET;
    cout << repeatChar(' ', 30 - (int)(sentiment.neutral * 30)) << "] " << (sentiment.neutral * 100) << "%\n";
}

// Spell check visualization
void Visualizer::visualizeSpellCheckResults(const SpellCheckResult& result) {
    printHeader("SPELL CHECK RESULTS");
    
    double accuracy = result.totalWords > 0 ? 
        (100.0 * result.correctWords / result.totalWords) : 0;
    
    cout << "Summary:\n";
    cout << "  Total Words:    " << result.totalWords << "\n";
    cout << "  Correct Words:  " << result.correctWords << " (" << fixed << setprecision(1) << accuracy << "%)\n";
    cout << "  Errors Found:   " << result.incorrectWords << "\n";
    cout << "  Processing Time:" << setprecision(2) << result.processingTimeMs << " ms\n";
    
    // Accuracy bar
    int barLen = (int)(accuracy / 2);
    cout << "\nAccuracy: [" << GREEN << repeatChar(BLOCK_FULL, barLen) << RESET;
    cout << repeatChar(' ', 50 - barLen) << "] " << accuracy << "%\n";
    
    if (!result.errors.empty()) {
        visualizeErrorDistribution(result.errors);
    }
}

void Visualizer::visualizeErrorDistribution(const vector<SpellingError>& errors) {
    if (errors.empty()) return;
    
    cout << "\nErrors Found:\n";
    
    for (const auto& err : errors) {
        cout << "  X \"" << err.originalWord << "\" (line " << err.lineNumber << ")\n";
        if (!err.suggestions.empty()) {
            cout << "    Suggestions: ";
            for (size_t i = 0; i < min((size_t)5, err.suggestions.size()); i++) {
                if (i > 0) cout << ", ";
                cout << err.suggestions[i];
            }
            cout << "\n";
        }
    }
}

// Export functions
void Visualizer::exportToSVG(const vector<DataPoint>& data, const ChartConfig& config, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not create file " << filename << endl;
        return;
    }
    
    int width = config.width;
    int height = config.height;
    int margin = 60;
    int chartWidth = width - 2 * margin;
    int chartHeight = height - 2 * margin;
    
    double maxVal = getMaxValue(data);
    
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    file << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << width << "\" height=\"" << height << "\">\n";
    file << "<rect width=\"100%\" height=\"100%\" fill=\"white\"/>\n";
    
    // Title
    file << "<text x=\"" << width/2 << "\" y=\"30\" text-anchor=\"middle\" font-size=\"18\" font-weight=\"bold\">" << config.title << "</text>\n";
    
    // Bars
    int barWidth = chartWidth / data.size() - 10;
    vector<string> colors = {"#4CAF50", "#2196F3", "#FFC107", "#E91E63", "#9C27B0"};
    
    for (size_t i = 0; i < data.size(); i++) {
        int barHeight = (int)((data[i].value / maxVal) * chartHeight);
        int x = margin + i * (barWidth + 10) + 5;
        int y = height - margin - barHeight;
        
        file << "<rect x=\"" << x << "\" y=\"" << y << "\" width=\"" << barWidth << "\" height=\"" << barHeight << "\" fill=\"" << colors[i % colors.size()] << "\" rx=\"3\"/>\n";
        file << "<text x=\"" << (x + barWidth/2) << "\" y=\"" << (y - 5) << "\" text-anchor=\"middle\" font-size=\"12\">" << fixed << setprecision(4) << data[i].value << "</text>\n";
        file << "<text x=\"" << (x + barWidth/2) << "\" y=\"" << (height - margin + 20) << "\" text-anchor=\"middle\" font-size=\"10\">" << data[i].label << "</text>\n";
    }
    
    // Axes
    file << "<line x1=\"" << margin << "\" y1=\"" << (height - margin) << "\" x2=\"" << (width - margin) << "\" y2=\"" << (height - margin) << "\" stroke=\"black\" stroke-width=\"2\"/>\n";
    file << "<line x1=\"" << margin << "\" y1=\"" << margin << "\" x2=\"" << margin << "\" y2=\"" << (height - margin) << "\" stroke=\"black\" stroke-width=\"2\"/>\n";
    
    file << "</svg>\n";
    file.close();
    
    cout << "SVG exported to: " << filename << endl;
}

void Visualizer::exportToHTML(const vector<DataPoint>& data, const ChartConfig& config, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not create file " << filename << endl;
        return;
    }
    
    double maxVal = getMaxValue(data);
    
    file << "<!DOCTYPE html>\n<html>\n<head>\n";
    file << "<title>" << config.title << "</title>\n";
    file << "<style>\n";
    file << "body { font-family: 'Segoe UI', Arial, sans-serif; margin: 40px; background: #f5f5f5; }\n";
    file << ".container { max-width: 800px; margin: 0 auto; background: white; padding: 30px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }\n";
    file << "h1 { color: #333; border-bottom: 3px solid #4CAF50; padding-bottom: 10px; }\n";
    file << "table { width: 100%; border-collapse: collapse; margin: 20px 0; }\n";
    file << "th, td { padding: 12px; text-align: left; border-bottom: 1px solid #ddd; }\n";
    file << "th { background: #4CAF50; color: white; }\n";
    file << ".bar { background: #4CAF50; height: 20px; border-radius: 3px; }\n";
    file << "</style>\n</head>\n<body>\n";
    
    file << "<div class=\"container\">\n";
    file << "<h1>" << config.title << "</h1>\n";
    file << "<p>Generated: " << __DATE__ << " " << __TIME__ << "</p>\n";
    
    file << "<table>\n<tr><th>Method</th><th>Value</th><th>Graph</th></tr>\n";
    
    for (const auto& dp : data) {
        int barWidth = (int)((dp.value / maxVal) * 100);
        file << "<tr>";
        file << "<td><strong>" << dp.label << "</strong></td>";
        file << "<td>" << fixed << setprecision(4) << dp.value << "</td>";
        file << "<td><div class=\"bar\" style=\"width: " << barWidth << "%\"></div></td>";
        file << "</tr>\n";
    }
    
    file << "</table>\n</div>\n</body>\n</html>\n";
    file.close();
    
    cout << "HTML report exported to: " << filename << endl;
}

void Visualizer::generateDashboard(const vector<BenchmarkResult>& benchmarks, 
                                    const ToneAnalysisResult* tone,
                                    const SpellCheckResult* spellcheck,
                                    const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not create file " << filename << endl;
        return;
    }
    
    file << "<!DOCTYPE html>\n<html>\n<head>\n";
    file << "<title>Spell Checker Dashboard</title>\n";
    file << "<style>\n";
    file << "body { font-family: Arial, sans-serif; margin: 20px; background: #f0f0f0; }\n";
    file << ".dashboard { display: grid; grid-template-columns: repeat(2, 1fr); gap: 20px; }\n";
    file << ".card { background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }\n";
    file << "h1 { color: #333; text-align: center; }\n";
    file << "h2 { color: #666; border-bottom: 2px solid #4CAF50; padding-bottom: 5px; }\n";
    file << "</style>\n</head>\n<body>\n";
    
    file << "<h1>Spell Checker Analysis Dashboard</h1>\n";
    file << "<div class=\"dashboard\">\n";
    
    // Benchmarks
    file << "<div class=\"card\">\n<h2>Benchmark Results</h2>\n";
    if (!benchmarks.empty()) {
        file << "<table style=\"width:100%\">\n";
        for (const auto& b : benchmarks) {
            file << "<tr><td>" << b.methodName << "</td><td>" << fixed << setprecision(4) << b.averageTimeMs << " ms</td></tr>\n";
        }
        file << "</table>\n";
    }
    file << "</div>\n";
    
    // Tone
    if (tone) {
        file << "<div class=\"card\">\n<h2>Tone Analysis</h2>\n";
        file << "<p>Dominant Tone: <strong>" << tone->dominantTone << "</strong></p>\n";
        file << "<p>Sentiment: " << (tone->sentiment.compound > 0 ? "Positive" : "Negative/Neutral") << "</p>\n";
        file << "</div>\n";
    }
    
    // Spell check
    if (spellcheck) {
        file << "<div class=\"card\">\n<h2>Spell Check</h2>\n";
        file << "<p>Total Words: " << spellcheck->totalWords << "</p>\n";
        file << "<p>Errors: " << spellcheck->incorrectWords << "</p>\n";
        file << "</div>\n";
    }
    
    file << "</div>\n</body>\n</html>\n";
    file.close();
    
    cout << "Dashboard exported to: " << filename << endl;
}

void Visualizer::printTable(const vector<vector<string>>& data, const vector<string>& headers) {
    if (data.empty()) return;
    
    // Calculate column widths
    vector<size_t> widths(headers.size(), 0);
    for (size_t i = 0; i < headers.size(); i++) {
        widths[i] = headers[i].length();
    }
    for (const auto& row : data) {
        for (size_t i = 0; i < min(row.size(), widths.size()); i++) {
            widths[i] = max(widths[i], row[i].length());
        }
    }
    
    // Print header
    cout << "+";
    for (size_t i = 0; i < widths.size(); i++) {
        cout << repeatChar('-', widths[i] + 2);
        if (i < widths.size() - 1) cout << "+";
    }
    cout << "+\n";
    
    cout << "|";
    for (size_t i = 0; i < headers.size(); i++) {
        cout << " " << setw(widths[i]) << left << headers[i] << " |";
    }
    cout << "\n";
    
    cout << "+";
    for (size_t i = 0; i < widths.size(); i++) {
        cout << repeatChar('-', widths[i] + 2);
        if (i < widths.size() - 1) cout << "+";
    }
    cout << "+\n";
    
    // Print data rows
    for (const auto& row : data) {
        cout << "|";
        for (size_t i = 0; i < widths.size(); i++) {
            string cell = i < row.size() ? row[i] : "";
            cout << " " << setw(widths[i]) << left << cell << " |";
        }
        cout << "\n";
    }
    
    cout << "+";
    for (size_t i = 0; i < widths.size(); i++) {
        cout << repeatChar('-', widths[i] + 2);
        if (i < widths.size() - 1) cout << "+";
    }
    cout << "+\n";
}
