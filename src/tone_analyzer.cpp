#include "../include/tone_analyzer.h"

// Constructor
ToneAnalyzer::ToneAnalyzer() {
    initializeLexicons();
}

void ToneAnalyzer::initializeLexicons() {
    // Positive words
    positiveWords = {
        "good", "great", "excellent", "amazing", "wonderful", "fantastic",
        "awesome", "brilliant", "outstanding", "superb", "perfect", "beautiful",
        "love", "happy", "joy", "delighted", "pleased", "excited", "thrilled",
        "grateful", "thankful", "appreciate", "enjoy", "like", "best", "better",
        "success", "successful", "achieve", "accomplish", "win", "winning",
        "positive", "optimistic", "hope", "hopeful", "confident", "proud",
        "impressive", "remarkable", "exceptional", "incredible", "magnificent",
        "favorable", "beneficial", "advantageous", "helpful", "useful",
        "innovative", "creative", "inspiring", "motivated", "enthusiastic",
        "bright", "cheerful", "pleasant", "friendly", "kind", "generous",
        "honest", "trustworthy", "reliable", "dedicated", "committed"
    };
    
    // Negative words
    negativeWords = {
        "bad", "terrible", "awful", "horrible", "poor", "worst", "worse",
        "hate", "dislike", "angry", "sad", "upset", "disappointed", "frustrated",
        "annoyed", "irritated", "furious", "outraged", "disgusted", "miserable",
        "depressed", "anxious", "worried", "concerned", "afraid", "scared",
        "fail", "failure", "failed", "failing", "lose", "lost", "losing",
        "problem", "issue", "trouble", "difficult", "hard", "challenging",
        "negative", "pessimistic", "hopeless", "helpless", "useless", "worthless",
        "wrong", "mistake", "error", "fault", "blame", "guilty", "shame",
        "ugly", "boring", "dull", "stupid", "idiotic", "ridiculous", "pathetic",
        "weak", "lazy", "careless", "reckless", "dangerous", "harmful",
        "dishonest", "unreliable", "untrustworthy", "incompetent", "inadequate"
    };
    
    // Formal words
    formalWords = {
        "therefore", "however", "furthermore", "moreover", "consequently",
        "nevertheless", "accordingly", "thus", "hence", "whereby", "whereas",
        "notwithstanding", "henceforth", "thereby", "herein", "therein",
        "pursuant", "regarding", "concerning", "pertaining", "aforementioned",
        "subsequent", "prior", "preliminary", "comprehensive", "substantial",
        "significant", "considerable", "appropriate", "adequate", "sufficient",
        "demonstrate", "indicate", "illustrate", "establish", "determine",
        "facilitate", "implement", "utilize", "acquire", "obtain", "procure",
        "commence", "conclude", "terminate", "initiate", "constitute",
        "endeavor", "ascertain", "comprehend", "acknowledge", "appreciate"
    };
    
    // Informal words
    informalWords = {
        "gonna", "wanna", "gotta", "kinda", "sorta", "dunno", "lemme",
        "yeah", "yep", "nope", "nah", "yup", "ok", "okay", "hey", "hi",
        "cool", "awesome", "stuff", "things", "guys", "kids", "folks",
        "pretty", "really", "very", "super", "totally", "basically",
        "like", "just", "so", "well", "anyway", "actually", "literally",
        "whatever", "whenever", "wherever", "however", "whichever",
        "lol", "omg", "btw", "idk", "imo", "tbh", "fyi", "asap",
        "gonna", "wanna", "gotta", "ain't", "can't", "won't", "don't"
    };
    
    // Academic words
    academicWords = {
        "analysis", "analyze", "approach", "assessment", "assume", "authority",
        "available", "benefit", "concept", "consistent", "constitutional",
        "context", "contract", "create", "data", "definition", "derived",
        "distribution", "economic", "environment", "established", "estimate",
        "evidence", "export", "factors", "financial", "formula", "function",
        "identified", "income", "indicate", "individual", "interpretation",
        "involved", "issues", "labor", "legal", "legislation", "major",
        "method", "occur", "percent", "period", "policy", "principle",
        "procedure", "process", "required", "research", "response", "role",
        "section", "sector", "significant", "similar", "source", "specific",
        "structure", "theory", "variables", "hypothesis", "methodology"
    };
    
    // Emotional words
    emotionalWords = {
        "love", "hate", "fear", "joy", "anger", "sadness", "happiness",
        "excitement", "anxiety", "depression", "hope", "despair", "pride",
        "shame", "guilt", "jealousy", "envy", "gratitude", "compassion",
        "sympathy", "empathy", "passion", "desire", "longing", "yearning",
        "heartbroken", "devastated", "ecstatic", "elated", "furious",
        "terrified", "horrified", "disgusted", "amazed", "astonished",
        "shocked", "surprised", "confused", "frustrated", "overwhelmed",
        "touched", "moved", "inspired", "motivated", "determined"
    };
    
    // Intensifiers
    intensifiers = {
        "very", "extremely", "incredibly", "absolutely", "totally", "completely",
        "utterly", "highly", "deeply", "strongly", "particularly", "especially",
        "remarkably", "exceptionally", "extraordinarily", "tremendously",
        "immensely", "enormously", "vastly", "greatly", "significantly"
    };
    
    // Negations
    negations = {
        "not", "no", "never", "neither", "nobody", "nothing", "nowhere",
        "none", "nor", "cannot", "can't", "won't", "wouldn't", "shouldn't",
        "couldn't", "doesn't", "don't", "didn't", "isn't", "aren't", "wasn't"
    };
}

// Text processing helpers
string ToneAnalyzer::toLowerCase(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(),
              [](unsigned char c) { return tolower(c); });
    return result;
}

string ToneAnalyzer::cleanWord(const string& word) {
    string result;
    for (char c : word) {
        if (isalpha(static_cast<unsigned char>(c)) || c == '\'') {
            result += tolower(static_cast<unsigned char>(c));
        }
    }
    return result;
}

vector<string> ToneAnalyzer::tokenize(const string& text) {
    vector<string> tokens;
    istringstream stream(text);
    string word;
    
    while (stream >> word) {
        string cleaned = cleanWord(word);
        if (!cleaned.empty()) {
            tokens.push_back(cleaned);
        }
    }
    
    return tokens;
}

vector<string> ToneAnalyzer::getSentences(const string& text) {
    vector<string> sentences;
    string current;
    
    for (char c : text) {
        current += c;
        if (c == '.' || c == '!' || c == '?') {
            if (!current.empty()) {
                sentences.push_back(current);
                current.clear();
            }
        }
    }
    
    if (!current.empty()) {
        sentences.push_back(current);
    }
    
    return sentences;
}

int ToneAnalyzer::countSyllables(const string& word) {
    if (word.empty()) return 0;
    
    int count = 0;
    bool prevVowel = false;
    string vowels = "aeiouy";
    
    for (size_t i = 0; i < word.length(); i++) {
        bool isVowel = vowels.find(tolower(word[i])) != string::npos;
        if (isVowel && !prevVowel) {
            count++;
        }
        prevVowel = isVowel;
    }
    
    // Handle silent e
    if (word.length() > 2 && tolower(word.back()) == 'e') {
        count--;
    }
    
    return max(1, count);
}

// Analysis helpers
double ToneAnalyzer::calculateSentiment(const vector<string>& words) {
    double posCount = 0, negCount = 0;
    bool negation = false;
    double intensifierMult = 1.0;
    
    for (size_t i = 0; i < words.size(); i++) {
        const string& word = words[i];
        
        // Check for negation
        if (negations.count(word)) {
            negation = true;
            continue;
        }
        
        // Check for intensifier
        if (intensifiers.count(word)) {
            intensifierMult = 1.5;
            continue;
        }
        
        // Count sentiment
        if (positiveWords.count(word)) {
            if (negation) {
                negCount += intensifierMult;
            } else {
                posCount += intensifierMult;
            }
        } else if (negativeWords.count(word)) {
            if (negation) {
                posCount += intensifierMult;
            } else {
                negCount += intensifierMult;
            }
        }
        
        // Reset modifiers
        negation = false;
        intensifierMult = 1.0;
    }
    
    double total = posCount + negCount;
    if (total == 0) return 0;
    
    return (posCount - negCount) / total;
}

double ToneAnalyzer::calculateFormality(const vector<string>& words) {
    double formalCount = 0, informalCount = 0;
    
    for (const string& word : words) {
        if (formalWords.count(word) || academicWords.count(word)) {
            formalCount++;
        } else if (informalWords.count(word)) {
            informalCount++;
        }
    }
    
    double total = formalCount + informalCount;
    if (total == 0) return 0.5;  // Neutral
    
    return formalCount / total;
}

double ToneAnalyzer::calculateEmotionalIntensity(const vector<string>& words) {
    double emotionalCount = 0;
    double intensifierCount = 0;
    
    for (const string& word : words) {
        if (emotionalWords.count(word)) {
            emotionalCount++;
        }
        if (intensifiers.count(word)) {
            intensifierCount++;
        }
    }
    
    if (words.empty()) return 0;
    
    double ratio = (emotionalCount + intensifierCount * 0.5) / words.size();
    return min(1.0, ratio * 5);  // Scale to 0-1
}

double ToneAnalyzer::calculateReadability(const string& text, int wordCount, int sentenceCount) {
    if (wordCount == 0 || sentenceCount == 0) return 0;
    
    vector<string> words = tokenize(text);
    int syllableCount = 0;
    
    for (const string& word : words) {
        syllableCount += countSyllables(word);
    }
    
    // Flesch-Kincaid Grade Level
    double avgWordsPerSentence = (double)wordCount / sentenceCount;
    double avgSyllablesPerWord = (double)syllableCount / wordCount;
    
    double grade = 0.39 * avgWordsPerSentence + 11.8 * avgSyllablesPerWord - 15.59;
    return max(0.0, min(20.0, grade));  // Clamp to reasonable range
}

map<Tone, double> ToneAnalyzer::calculateToneScores(const vector<string>& words, 
                                                      double formality, double emotion) {
    map<Tone, double> scores;
    
    double sentiment = calculateSentiment(words);
    
    // Calculate individual tone scores
    scores[Tone::POSITIVE] = max(0.0, sentiment);
    scores[Tone::NEGATIVE] = max(0.0, -sentiment);
    scores[Tone::NEUTRAL] = 1.0 - abs(sentiment);
    scores[Tone::FORMAL] = formality;
    scores[Tone::INFORMAL] = 1.0 - formality;
    scores[Tone::EMOTIONAL] = emotion;
    scores[Tone::OBJECTIVE] = 1.0 - emotion;
    
    // Academic tone (formal + objective)
    double academicCount = 0;
    for (const string& word : words) {
        if (academicWords.count(word)) academicCount++;
    }
    scores[Tone::ACADEMIC] = words.empty() ? 0 : min(1.0, (academicCount / words.size()) * 10 + formality * 0.3);
    
    return scores;
}

vector<string> ToneAnalyzer::extractKeywords(const vector<string>& words, int topN) {
    map<string, int> frequency;
    
    // Count meaningful words (skip common ones)
    set<string> stopWords = {"the", "a", "an", "and", "or", "but", "in", "on", "at", "to", "for",
                             "of", "with", "by", "from", "is", "are", "was", "were", "be", "been",
                             "have", "has", "had", "do", "does", "did", "will", "would", "could",
                             "should", "may", "might", "must", "shall", "can", "this", "that",
                             "these", "those", "it", "its", "i", "you", "he", "she", "we", "they"};
    
    for (const string& word : words) {
        if (word.length() > 2 && !stopWords.count(word)) {
            frequency[word]++;
        }
    }
    
    // Sort by frequency
    vector<pair<int, string>> sorted;
    for (const auto& [word, count] : frequency) {
        sorted.push_back({count, word});
    }
    sort(sorted.rbegin(), sorted.rend());
    
    vector<string> keywords;
    for (int i = 0; i < min(topN, (int)sorted.size()); i++) {
        keywords.push_back(sorted[i].second);
    }
    
    return keywords;
}

string ToneAnalyzer::determineDominantTone(const map<Tone, double>& scores) {
    Tone dominant = Tone::NEUTRAL;
    double maxScore = 0;
    
    for (const auto& [tone, score] : scores) {
        if (score > maxScore) {
            maxScore = score;
            dominant = tone;
        }
    }
    
    return toneToString(dominant);
}

string ToneAnalyzer::toneToString(Tone tone) {
    switch (tone) {
        case Tone::POSITIVE: return "Positive";
        case Tone::NEGATIVE: return "Negative";
        case Tone::NEUTRAL: return "Neutral";
        case Tone::FORMAL: return "Formal";
        case Tone::INFORMAL: return "Informal";
        case Tone::ACADEMIC: return "Academic";
        case Tone::EMOTIONAL: return "Emotional";
        case Tone::OBJECTIVE: return "Objective";
        default: return "Unknown";
    }
}

string ToneAnalyzer::generateSummary(const ToneAnalysisResult& result) {
    stringstream ss;
    
    ss << "The text appears to be ";
    
    // Formality
    if (result.formalityScore > 0.7) {
        ss << "highly formal";
    } else if (result.formalityScore > 0.4) {
        ss << "moderately formal";
    } else {
        ss << "informal";
    }
    
    ss << " with a ";
    
    // Sentiment
    if (result.sentiment.compound > 0.3) {
        ss << "positive";
    } else if (result.sentiment.compound < -0.3) {
        ss << "negative";
    } else {
        ss << "neutral";
    }
    
    ss << " tone. ";
    
    // Emotional intensity
    if (result.emotionalIntensity > 0.6) {
        ss << "It conveys strong emotional content. ";
    } else if (result.emotionalIntensity < 0.2) {
        ss << "It maintains an objective perspective. ";
    }
    
    // Readability
    ss << "Reading level: approximately grade " << (int)result.readabilityScore << ".";
    
    return ss.str();
}

// Main analysis function
ToneAnalysisResult ToneAnalyzer::analyzeText(const string& text) {
    ToneAnalysisResult result;
    
    vector<string> words = tokenize(text);
    vector<string> sentences = getSentences(text);
    
    result.wordCount = words.size();
    result.sentenceCount = sentences.size();
    
    if (result.wordCount == 0) {
        result.summary = "Text is empty or contains no analyzable words.";
        return result;
    }
    
    // Calculate average lengths
    double totalWordLen = 0;
    for (const string& word : words) {
        totalWordLen += word.length();
    }
    result.avgWordLength = totalWordLen / result.wordCount;
    result.avgSentenceLength = (double)result.wordCount / max(1, result.sentenceCount);
    
    // Calculate scores
    double sentimentValue = calculateSentiment(words);
    result.sentiment.compound = sentimentValue;
    result.sentiment.positive = max(0.0, sentimentValue);
    result.sentiment.negative = max(0.0, -sentimentValue);
    result.sentiment.neutral = 1.0 - abs(sentimentValue);
    
    result.formalityScore = calculateFormality(words);
    result.emotionalIntensity = calculateEmotionalIntensity(words);
    result.readabilityScore = calculateReadability(text, result.wordCount, result.sentenceCount);
    
    result.toneScores = calculateToneScores(words, result.formalityScore, result.emotionalIntensity);
    result.keywords = extractKeywords(words);
    result.dominantTone = determineDominantTone(result.toneScores);
    result.summary = generateSummary(result);
    
    return result;
}

ToneAnalysisResult ToneAnalyzer::analyzeFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        ToneAnalysisResult result;
        result.summary = "Error: Could not open file " + filename;
        return result;
    }
    
    stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    return analyzeText(buffer.str());
}

SentimentScore ToneAnalyzer::getSentiment(const string& text) {
    vector<string> words = tokenize(text);
    double value = calculateSentiment(words);
    
    SentimentScore score;
    score.compound = value;
    score.positive = max(0.0, value);
    score.negative = max(0.0, -value);
    score.neutral = 1.0 - abs(value);
    
    return score;
}

double ToneAnalyzer::getFormality(const string& text) {
    vector<string> words = tokenize(text);
    return calculateFormality(words);
}

double ToneAnalyzer::getReadability(const string& text) {
    vector<string> words = tokenize(text);
    vector<string> sentences = getSentences(text);
    return calculateReadability(text, words.size(), sentences.size());
}

void ToneAnalyzer::printAnalysis(const ToneAnalysisResult& result) {
    cout << "\n";
    cout << "╔══════════════════════════════════════════════════════════════╗\n";
    cout << "║                    TONE ANALYSIS RESULTS                     ║\n";
    cout << "╚══════════════════════════════════════════════════════════════╝\n\n";
    
    // Basic stats
    cout << "┌─────────────────── TEXT STATISTICS ───────────────────┐\n";
    cout << "│  Word Count:           " << setw(28) << result.wordCount << "  │\n";
    cout << "│  Sentence Count:       " << setw(28) << result.sentenceCount << "  │\n";
    cout << "│  Avg Word Length:      " << setw(28) << fixed << setprecision(1) << result.avgWordLength << "  │\n";
    cout << "│  Avg Sentence Length:  " << setw(28) << result.avgSentenceLength << "  │\n";
    cout << "│  Reading Level:        " << setw(23) << "Grade " << (int)result.readabilityScore << "  │\n";
    cout << "└────────────────────────────────────────────────────────┘\n\n";
    
    // Sentiment
    cout << "┌─────────────────── SENTIMENT ANALYSIS ─────────────────┐\n";
    cout << "│  Overall Sentiment:  ";
    if (result.sentiment.compound > 0.3) cout << setw(32) << "POSITIVE ✓";
    else if (result.sentiment.compound < -0.3) cout << setw(32) << "NEGATIVE ✗";
    else cout << setw(32) << "NEUTRAL ○";
    cout << " │\n";
    cout << "│  Compound Score:     " << setw(32) << fixed << setprecision(3) << result.sentiment.compound << " │\n";
    cout << "│  Positive:           " << setw(32) << result.sentiment.positive << " │\n";
    cout << "│  Negative:           " << setw(32) << result.sentiment.negative << " │\n";
    cout << "│  Neutral:            " << setw(32) << result.sentiment.neutral << " │\n";
    cout << "└─────────────────────────────────────────────────────────┘\n\n";
    
    // Tone scores
    cout << "┌─────────────────── TONE BREAKDOWN ──────────────────────┐\n";
    for (const auto& [tone, score] : result.toneScores) {
        string toneName = toneToString(tone);
        int barLength = (int)(score * 30);
        cout << "│  " << setw(12) << left << toneName << " ";
        cout << "[";
        for (int i = 0; i < 30; i++) {
            cout << (i < barLength ? "█" : "░");
        }
        cout << "] " << setw(5) << right << fixed << setprecision(2) << (score * 100) << "% │\n";
    }
    cout << "└──────────────────────────────────────────────────────────┘\n\n";
    
    // Dominant tone
    cout << "  Dominant Tone: " << result.dominantTone << "\n\n";
    
    // Keywords
    if (!result.keywords.empty()) {
        cout << "  Keywords: ";
        for (size_t i = 0; i < result.keywords.size(); i++) {
            if (i > 0) cout << ", ";
            cout << result.keywords[i];
        }
        cout << "\n\n";
    }
    
    // Summary
    cout << "  Summary:\n";
    cout << "  " << result.summary << "\n";
}

void ToneAnalyzer::exportAnalysis(const ToneAnalysisResult& result, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not create file " << filename << endl;
        return;
    }
    
    file << "# Tone Analysis Report\n\n";
    file << "## Text Statistics\n";
    file << "- Word Count: " << result.wordCount << "\n";
    file << "- Sentence Count: " << result.sentenceCount << "\n";
    file << "- Average Word Length: " << result.avgWordLength << "\n";
    file << "- Average Sentence Length: " << result.avgSentenceLength << "\n";
    file << "- Reading Level: Grade " << (int)result.readabilityScore << "\n\n";
    
    file << "## Sentiment Analysis\n";
    file << "- Compound Score: " << result.sentiment.compound << "\n";
    file << "- Positive: " << result.sentiment.positive << "\n";
    file << "- Negative: " << result.sentiment.negative << "\n";
    file << "- Neutral: " << result.sentiment.neutral << "\n\n";
    
    file << "## Tone Scores\n";
    for (const auto& [tone, score] : result.toneScores) {
        file << "- " << toneToString(tone) << ": " << (score * 100) << "%\n";
    }
    file << "\n## Dominant Tone: " << result.dominantTone << "\n\n";
    
    file << "## Keywords\n";
    for (const string& kw : result.keywords) {
        file << "- " << kw << "\n";
    }
    
    file << "\n## Summary\n" << result.summary << "\n";
    
    file.close();
    cout << "Analysis exported to: " << filename << endl;
}
