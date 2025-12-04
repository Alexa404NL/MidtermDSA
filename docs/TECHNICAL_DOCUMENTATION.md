# Technical Documentation

## Spell Checker System - DSA Midterm Project

---

## Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [Core Data Structures](#core-data-structures)
3. [Algorithms](#algorithms)
4. [Parallel Processing (OpenMP)](#parallel-processing-openmp)
5. [Tone Analysis](#tone-analysis)
6. [Visualization System](#visualization-system)
7. [API Reference](#api-reference)
8. [Build System](#build-system)

---

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────────────┐
│                          USER INTERFACE                              │
│  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐  │
│  │   CLI   │  │   UI    │  │Parallel │  │  Tone   │  │  Visual │  │
│  │  Mode   │  │  Mode   │  │  Mode   │  │  Mode   │  │  Mode   │  │
│  └────┬────┘  └────┬────┘  └────┬────┘  └────┬────┘  └────┬────┘  │
└───────┼────────────┼────────────┼────────────┼────────────┼────────┘
        │            │            │            │            │
        ▼            ▼            ▼            ▼            ▼
┌─────────────────────────────────────────────────────────────────────┐
│                        PROCESSING LAYER                              │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐              │
│  │ SpellChecker │  │  Parallel    │  │    Tone      │              │
│  │    Core      │  │  Processor   │  │   Analyzer   │              │
│  │              │  │   (OpenMP)   │  │              │              │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘              │
└─────────┼─────────────────┼─────────────────┼──────────────────────┘
          │                 │                 │
          ▼                 ▼                 ▼
┌─────────────────────────────────────────────────────────────────────┐
│                      DATA STRUCTURES LAYER                           │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐              │
│  │     Trie     │  │   KD-Tree    │  │     A*       │              │
│  │  (Prefix)    │  │  (5D Space)  │  │   Search     │              │
│  └──────────────┘  └──────────────┘  └──────────────┘              │
└─────────────────────────────────────────────────────────────────────┘
```

---

## Core Data Structures

### 1. Trie (Prefix Tree)

**File**: `include/trie.h`, `src/trie.cpp`

```cpp
struct TrieNode {
    map<char, TrieNode*> children;  // Child nodes by character
    bool isEndOfWord;                // Marks complete words
    string word;                     // Store complete word at leaf
};

class Trie {
public:
    void insert(const string& word);           // O(L)
    bool contains(const string& word);          // O(L)
    bool remove(const string& word);            // O(L)
    vector<string> getSimilarWords(            // Levenshtein search
        const string& word, 
        int maxDistance
    );
    int size();
};
```

**Complexity**:
- Insert: O(L) where L = word length
- Search: O(L)
- Fuzzy Search: O(26^d × L) where d = max edit distance

### 2. KD-Tree

**File**: `include/kdtree.h`, `src/kdtree.cpp`

```cpp
struct Position {
    string word;
    vector<double> coords;  // 5-dimensional vector
    
    static Position fromWord(const string& word);
};

class KDTree {
public:
    void insert(const string& word);
    bool find(const string& word);
    vector<Position> findKNearest(const string& target, size_t k);
};
```

**5D Word Embedding**:
1. `coords[0]`: Normalized length (word.length() / 20.0)
2. `coords[1]`: Vowel ratio (vowels / total letters)
3. `coords[2]`: Common letter frequency (e,t,a,i,n,o)
4. `coords[3]`: Alphabet balance (a-m vs n-z ratio)
5. `coords[4]`: First letter position (a=0.0, z=1.0)

**Complexity**:
- Insert: O(log N)
- k-NN Search: O(N^(1-1/D) + k) average, O(N) worst case

### 3. A* Search

**File**: `include/astar_spellcheck.h`, `src/astar_spellcheck.cpp`

```cpp
struct AStarState {
    TrieNode* node;
    string currentWord;
    int targetIndex;
    int gCost;  // Actual edit distance
    int fCost;  // g + heuristic
};

class AStarSpellChecker {
public:
    AStarSpellChecker(Trie* trie);
    vector<pair<int, string>> findSimilarWords(
        const string& target, 
        int maxDistance
    );
    string findBestMatch(const string& target, int maxDistance);
};
```

**Heuristic Function**:
```cpp
h(n) = |remaining_target_chars - remaining_trie_depth|
```
This is admissible because we need at least this many operations.

---

## Algorithms

### Levenshtein Distance (Edit Distance)

Used to measure similarity between words:

```cpp
int levenshteinDistance(const string& s1, const string& s2) {
    // Dynamic programming approach
    // Operations: insert, delete, substitute
    // Time: O(m × n), Space: O(min(m, n))
}
```

### A* Search Algorithm

```
function A*(start, goal):
    openSet = priority_queue with start
    gScore[start] = 0
    fScore[start] = heuristic(start, goal)
    
    while openSet not empty:
        current = openSet.pop_lowest_f()
        
        if current is goal:
            return reconstruct_path()
        
        for each neighbor of current:
            tentative_g = gScore[current] + cost(current, neighbor)
            
            if tentative_g < gScore[neighbor]:
                gScore[neighbor] = tentative_g
                fScore[neighbor] = tentative_g + heuristic(neighbor, goal)
                openSet.add(neighbor)
    
    return failure
```

---

## Parallel Processing (OpenMP)

**File**: `include/parallel_processor.h`, `src/parallel_processor.cpp`

### Overview

The system uses OpenMP for multi-threaded spell checking:

```cpp
class ParallelSpellChecker {
private:
    SpellChecker* checker;
    int numThreads;
    
public:
    ParallelSpellChecker(SpellChecker* sc, int threads = 0);
    
    SpellCheckResult checkTextParallel(
        const string& text, 
        const string& method = "astar"
    );
    
    void compareSequentialVsParallel(const string& text);
};
```

### OpenMP Implementation

```cpp
#pragma omp parallel num_threads(numThreads)
{
    int tid = omp_get_thread_num();
    
    #pragma omp for schedule(dynamic, 10)
    for (int i = 0; i < words.size(); i++) {
        // Thread-local processing
        if (checker->isValidWord(words[i])) {
            threadCorrect[tid]++;
        } else {
            // Get suggestions
            SpellingError error;
            error.suggestions = checker->getSuggestionsAStar(words[i]);
            threadErrors[tid].push_back(error);
        }
    }
}
```

### Key Features

| Feature | Description |
|---------|-------------|
| `schedule(dynamic, 10)` | Dynamic work distribution in chunks of 10 |
| Thread-local storage | Avoids race conditions |
| Auto thread detection | Uses `omp_get_max_threads()` |
| Scalability analysis | Tests 1 to N threads |

### Performance Results

```
Text: 267 words
Sequential: 11.04 ms
Parallel (8 threads): 3.13 ms
Speedup: 3.53x
Efficiency: 44.11%
```

---

## Tone Analysis

**File**: `include/tone_analyzer.h`, `src/tone_analyzer.cpp`

### Sentiment Analysis

```cpp
struct SentimentScore {
    double positive;   // 0.0 - 1.0
    double negative;   // 0.0 - 1.0
    double neutral;    // 0.0 - 1.0
    double compound;   // -1.0 to 1.0
};

struct ToneAnalysisResult {
    SentimentScore sentiment;
    map<Tone, double> toneScores;
    string dominantTone;
    vector<string> keywords;
    double readabilityScore;
    int wordCount;
    int sentenceCount;
};
```

### Lexicon-Based Analysis

**Positive Words** (~50):
```
good, great, excellent, amazing, wonderful, fantastic, 
happy, joy, love, beautiful, perfect, brilliant...
```

**Negative Words** (~50):
```
bad, terrible, awful, horrible, poor, disappointing,
sad, angry, hate, ugly, wrong, failure...
```

### Formality Detection

| Indicator | Type |
|-----------|------|
| "therefore", "furthermore" | Formal |
| "gonna", "wanna", "kinda" | Informal |
| Technical jargon | Academic |
| Exclamation marks | Emotional |

---

## Visualization System

**File**: `include/visualizer.h`, `src/visualizer.cpp`

### ASCII Chart Types

```cpp
enum class ChartType {
    BAR_HORIZONTAL,
    BAR_VERTICAL,
    LINE,
    PIE,
    HISTOGRAM
};
```

### Example Output

```
+===============================+
|  Average Execution Time (ms)  |
+===============================+

  trie |████████████████░░░░| 0.0360
kdtree |██████████░░░░░░░░░░| 0.0214
 astar |████████████████████| 0.0540
       +--------------------
        0        0.03      0.06
```

### Export Formats

- **CSV**: Raw benchmark data
- **Markdown**: Human-readable reports
- **HTML**: Interactive web reports
- **SVG**: Vector graphics charts

---

## API Reference

### SpellChecker Class

```cpp
class SpellChecker {
public:
    SpellChecker(int maxDistance = 2, int maxSuggestions = 5);
    
    // Dictionary operations
    bool loadDictionary(const string& filename);
    bool isValidWord(const string& word);
    
    // Suggestions by method
    vector<string> getSuggestionsTrie(const string& word);
    vector<string> getSuggestionsKDTree(const string& word);
    vector<string> getSuggestionsAStar(const string& word);
    
    // Text processing
    SpellCheckResult checkText(const string& text, const string& method);
    SpellCheckResult checkFile(const string& filename, const string& method);
    
    // Comparison
    void compareMethodsForWord(const string& word);
};
```

### ParallelSpellChecker Class

```cpp
class ParallelSpellChecker {
public:
    ParallelSpellChecker(SpellChecker* sc, int threads = 0);
    
    SpellCheckResult checkTextParallel(const string& text, const string& method);
    SpellCheckResult checkFileParallel(const string& filename, const string& method);
    vector<vector<string>> getSuggestionsParallel(const vector<string>& words);
    void compareSequentialVsParallel(const string& text);
    
    int getThreadCount() const;
    void setThreadCount(int threads);
};
```

### ToneAnalyzer Class

```cpp
class ToneAnalyzer {
public:
    ToneAnalysisResult analyze(const string& text);
    ToneAnalysisResult analyzeFile(const string& filename);
    void printAnalysis(const ToneAnalysisResult& result);
    void exportAnalysis(const ToneAnalysisResult& result, const string& filename);
};
```

---

## Build System

### Makefile Configuration

```makefile
# Compiler
CXX = g++

# OpenMP flags (auto-detect OS)
ifeq ($(UNAME_S),Darwin)
    # macOS with Homebrew libomp
    OMP_FLAGS = -Xpreprocessor -fopenmp \
                -I/opt/homebrew/opt/libomp/include \
                -L/opt/homebrew/opt/libomp/lib -lomp
else
    # Linux
    OMP_FLAGS = -fopenmp
endif

CXXFLAGS = -std=c++17 -Wall -Wextra -O2 $(OMP_FLAGS)
```

### Build Targets

| Target | Description |
|--------|-------------|
| `make all` | Build main executable |
| `make test` | Build and run unit tests |
| `make run` | Run the application |
| `make run-ui` | Run interactive UI |
| `make benchmark` | Run benchmarks |
| `make clean` | Remove build files |
| `make help` | Show all targets |

### Dependencies

- **C++17**: Required for structured bindings, if constexpr
- **OpenMP**: For parallel processing
- **libomp** (macOS): `brew install libomp`
- **libgomp** (Linux): Usually included with GCC

---

## Testing

### Unit Tests (18 Total)

| Category | Tests |
|----------|-------|
| Trie | 5 tests (insert, contains, remove, similar, edge cases) |
| KD-Tree | 4 tests (insert, find, k-NN, embeddings) |
| A* Search | 4 tests (exists, similar, best match, no match) |
| SpellChecker | 5 tests (valid, trie suggestions, kdtree, astar, text) |

### Running Tests

```bash
make test
# Output: 18/18 tests passed ✓ ALL PASSED!
```

---

**Document Version**: 1.1  
**Last Updated**: December 2025
