#ifndef TONE_ANALYZER_H
#define TONE_ANALYZER_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>

using namespace std;

// Tone categories
enum class Tone {
    POSITIVE,
    NEGATIVE,
    NEUTRAL,
    FORMAL,
    INFORMAL,
    ACADEMIC,
    EMOTIONAL,
    OBJECTIVE
};

// Sentiment score structure
struct SentimentScore {
    double positive;
    double negative;
    double neutral;
    double compound;  // Overall sentiment (-1 to 1)
    
    SentimentScore() : positive(0), negative(0), neutral(0), compound(0) {}
};

// Tone analysis result
struct ToneAnalysisResult {
    SentimentScore sentiment;
    map<Tone, double> toneScores;
    vector<string> keywords;
    double formalityScore;      // 0 = informal, 1 = formal
    double emotionalIntensity;  // 0 = calm, 1 = intense
    double readabilityScore;    // Flesch-Kincaid grade level approximation
    int wordCount;
    int sentenceCount;
    double avgWordLength;
    double avgSentenceLength;
    string dominantTone;
    string summary;
};

class ToneAnalyzer {
private:
    // Sentiment lexicons
    set<string> positiveWords;
    set<string> negativeWords;
    set<string> formalWords;
    set<string> informalWords;
    set<string> academicWords;
    set<string> emotionalWords;
    
    // Intensifiers and negations
    set<string> intensifiers;
    set<string> negations;
    
    // Initialize lexicons
    void initializeLexicons();
    
    // Text processing
    vector<string> tokenize(const string& text);
    vector<string> getSentences(const string& text);
    string toLowerCase(const string& str);
    string cleanWord(const string& word);
    
    // Analysis helpers
    double calculateSentiment(const vector<string>& words);
    double calculateFormality(const vector<string>& words);
    double calculateEmotionalIntensity(const vector<string>& words);
    double calculateReadability(const string& text, int wordCount, int sentenceCount);
    map<Tone, double> calculateToneScores(const vector<string>& words, double formality, double emotion);
    vector<string> extractKeywords(const vector<string>& words, int topN = 10);
    string determineDominantTone(const map<Tone, double>& scores);
    string generateSummary(const ToneAnalysisResult& result);
    
    // Syllable counting for readability
    int countSyllables(const string& word);
    
public:
    ToneAnalyzer();
    
    // Main analysis function
    ToneAnalysisResult analyzeText(const string& text);
    ToneAnalysisResult analyzeFile(const string& filename);
    
    // Individual metrics
    SentimentScore getSentiment(const string& text);
    double getFormality(const string& text);
    double getReadability(const string& text);
    
    // Display results
    void printAnalysis(const ToneAnalysisResult& result);
    void exportAnalysis(const ToneAnalysisResult& result, const string& filename);
    
    // Tone name conversion
    static string toneToString(Tone tone);
};

#endif // TONE_ANALYZER_H
