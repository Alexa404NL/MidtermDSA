# Spell Checker System: A* + Trie vs KD-Tree

## DSA Midterm Project - Algorithmic Analysis and Comparison

[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![OpenMP](https://img.shields.io/badge/OpenMP-Parallel-orange.svg)](https://www.openmp.org/)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

---

## Table of Contents

1. [Problem Definition](#problem-definition)
2. [Research Question](#research-question)
3. [Algorithmic Justification](#algorithmic-justification)
4. [System Architecture](#system-architecture)
5. [Data Structures and Algorithms](#data-structures-and-algorithms)
6. [Advanced Features](#advanced-features)
7. [Installation and Usage](#installation-and-usage)
8. [Benchmarks and Results](#benchmarks-and-results)
9. [Performance Analysis](#performance-analysis)
10. [Project Structure](#project-structure)
11. [Team Members](#team-members)
12. [References](#references)

---

## Problem Definition

### Context and Motivation

Spelling errors are ubiquitous in digital text communication, affecting everything from casual messaging to professional documentation. The need for efficient spell-checking algorithms is critical in:

- **Text editors and IDEs**: Real-time spell checking while typing
- **Search engines**: Query correction for improved search results
- **Natural Language Processing**: Preprocessing for text analysis
- **Educational software**: Learning tools for language acquisition

The challenge lies in developing a system that can:
1. **Quickly identify** whether a word exists in a dictionary
2. **Efficiently suggest** corrections for misspelled words
3. **Handle large dictionaries** with acceptable memory usage
4. **Scale** with input size (number of words to check)

### Problem Statement

Given a text document and a dictionary of valid words, develop an efficient spell-checking system that:
- Identifies misspelled words in O(W × L) time complexity (W = number of words, L = average word length)
- Provides relevant correction suggestions ranked by similarity
- Compares multiple algorithmic approaches to determine optimal strategy

---

## Research Question

> **How does A* search with Levenshtein distance heuristics in a Trie structure compare to KD-Tree semantic search for spell-checking efficiency and suggestion quality?**

### Hypothesis

We hypothesize that:
1. **A* with Trie** will provide more accurate suggestions based on edit distance
2. **KD-Tree** will offer faster lookup times for finding similar words
3. The combination of both methods can provide a more robust spell-checking system

### Metrics for Evaluation

| Metric | Description |
|--------|-------------|
| **Time Complexity** | Empirical measurement of processing time vs. input size |
| **Suggestion Quality** | Accuracy of corrections (Levenshtein distance to correct word) |
| **Memory Usage** | Space complexity of data structures |
| **Scalability** | Performance with increasing dictionary/text sizes |
| **Throughput** | Words processed per second |

---

## Algorithmic Justification

### Why These Algorithms?

#### 1. Trie with Levenshtein Distance

The Trie (prefix tree) is ideal for dictionary operations because:
- **O(L) lookup time** for exact matches (L = word length)
- **Prefix sharing** reduces memory for words with common prefixes
- **Incremental Levenshtein computation** enables pruning of search space

```
Time Complexity: O(L) for lookup, O(26^d × L) for fuzzy search (d = max edit distance)
Space Complexity: O(N × L) where N = number of words
```

#### 2. A* Search Algorithm

A* enhances the Trie-based search by:
- Using **Levenshtein distance as cost function (g)**
- Employing **heuristic estimation (h)** to prioritize promising paths
- **Pruning** branches that exceed maximum edit distance

```
f(n) = g(n) + h(n)
where:
  g(n) = actual edit distance from start to current node
  h(n) = estimated remaining distance (admissible heuristic)
```

#### 3. KD-Tree for Semantic Similarity

The KD-Tree provides an alternative approach:
- **5-dimensional word embeddings** based on:
  1. Normalized word length
  2. Vowel ratio
  3. Common letter frequency
  4. Alphabet distribution (first half vs. second half)
  5. First letter weight
- **O(log N) average** for k-nearest neighbor search
- **Finds structurally similar words** even with different spellings

```
Time Complexity: O(log N) average, O(N) worst case for k-NN search
Space Complexity: O(N × D) where D = number of dimensions
```

### Integration of Course Topics (Chapters 4-6)

| Topic | Implementation |
|-------|---------------|
| **Computational Geometry** | KD-Tree for multi-dimensional word vectors |
| **Advanced Search (A*)** | Heuristic search in Trie with Levenshtein cost |
| **High-Performance Computing** | OpenMP parallel processing for large texts |
| **Data Visualization** | ASCII charts and HTML report generation |
| **NLP Analysis** | Sentiment and tone analysis of text |

---

## Advanced Features

### 1. OpenMP Parallel Processing

The system uses **OpenMP** for efficient parallel spell checking:

```cpp
#pragma omp parallel num_threads(numThreads)
{
    #pragma omp for schedule(dynamic, 10)
    for (int i = 0; i < words.size(); i++) {
        // Process words in parallel
    }
}
```

**Performance Results**:
- **3.53x speedup** with 8 threads
- **71.66% faster** than sequential processing
- Dynamic scheduling for load balancing

### 2. Tone Analysis

Analyzes text sentiment and formality:

| Metric | Description |
|--------|-------------|
| **Sentiment** | Positive/Negative/Neutral classification |
| **Formality** | Formal vs Informal writing style |
| **Readability** | Grade-level reading score |
| **Emotional Intensity** | Strength of emotional content |

### 3. Visualization System

- **ASCII Bar Charts**: Terminal-based benchmark visualization
- **Scalability Analysis**: Thread performance comparison
- **HTML Reports**: Exportable detailed reports
- **Dashboard Generation**: Combined metrics overview

---

## System Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                        SPELL CHECKER UI                         │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐             │
│  │ Check Word  │  │ Check Text  │  │ Check File  │             │
│  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘             │
└─────────┼────────────────┼────────────────┼─────────────────────┘
          │                │                │
          ▼                ▼                ▼
┌─────────────────────────────────────────────────────────────────┐
│                       SPELL CHECKER CORE                        │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │                    Text Tokenizer                         │  │
│  │         (Preprocessing: lowercase, clean punctuation)     │  │
│  └──────────────────────────────────────────────────────────┘  │
│                              │                                  │
│           ┌──────────────────┼──────────────────┐              │
│           ▼                  ▼                  ▼              │
│  ┌─────────────────┐ ┌─────────────────┐ ┌─────────────────┐  │
│  │   TRIE METHOD   │ │  A* SEARCH      │ │ KD-TREE METHOD  │  │
│  │   (Levenshtein) │ │  (Trie + Heur.) │ │ (Semantic)      │  │
│  └────────┬────────┘ └────────┬────────┘ └────────┬────────┘  │
│           │                   │                   │            │
│           └───────────────────┴───────────────────┘            │
│                              │                                  │
│                              ▼                                  │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │                  Suggestion Ranker                        │  │
│  └──────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
                               │
                               ▼
┌─────────────────────────────────────────────────────────────────┐
│                         BENCHMARK                               │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐             │
│  │ Time Meas.  │  │ Comparison  │  │ CSV Export  │             │
│  └─────────────┘  └─────────────┘  └─────────────┘             │
└─────────────────────────────────────────────────────────────────┘
```

---

## Data Structures and Algorithms

### 1. Trie (Prefix Tree)

```cpp
struct TrieNode {
    map<char, TrieNode*> children;
    bool isEndOfWord;
    string word;
};

class Trie {
    void insert(const string& word);      // O(L)
    bool contains(const string& word);     // O(L)
    vector<string> getSimilarWords(word, maxDist);  // Levenshtein search
};
```

**Key Feature**: Incremental Levenshtein distance computation during Trie traversal.

### 2. KD-Tree

```cpp
struct Position {
    string word;
    vector<double> coords;  // 5 dimensions
    static Position fromWord(const string& word);
};

class KDTree {
    void insert(const string word);
    vector<Position> findKNearest(const string target, size_t k);
};
```

**Dimensions**:
1. `coords[0]`: Normalized length (0-1)
2. `coords[1]`: Vowel ratio
3. `coords[2]`: Common letter frequency (e, t, a, i, n, o)
4. `coords[3]`: Alphabet balance (a-m vs n-z)
5. `coords[4]`: First letter position (a=0, z=1)

### 3. A* Search in Trie

```cpp
struct AStarState {
    TrieNode* node;
    string currentWord;
    int gCost;  // Actual edit distance
    int fCost;  // g + heuristic
};

class AStarSpellChecker {
    vector<pair<int, string>> findSimilarWords(target, maxDist);
    string findBestMatch(target, maxDist);
};
```

**Heuristic**: Remaining character count difference (admissible).

---

## Installation and Usage

### Prerequisites

- C++17 compatible compiler (g++ 7+, clang++ 5+)
- OpenMP support (libomp on macOS, libgomp on Linux)
- Make (optional, for using Makefile)

### Installing OpenMP (macOS)

```bash
# Install libomp via Homebrew
brew install libomp
```

### Building the Project

```bash
# Clone the repository
git clone https://github.com/your-repo/MidtermDSA.git
cd MidtermDSA

# Build using Make (includes OpenMP support)
make all

# Or compile manually with OpenMP
g++ -std=c++17 -O2 -Xpreprocessor -fopenmp \
    -I/opt/homebrew/opt/libomp/include \
    -L/opt/homebrew/opt/libomp/lib -lomp \
    -I include src/*.cpp -o spellchecker
```

### Running the Application

```bash
# Interactive UI mode (default)
./bin/spellchecker

# With custom dictionary
./bin/spellchecker --dict path/to/dictionary.txt

# Check a single word
./bin/spellchecker --check "helo"

# Check a file
./bin/spellchecker --file data/test_texts/sample_errors.txt

# Run benchmarks
./bin/spellchecker --benchmark

# Specify method (astar, trie, kdtree)
./bin/spellchecker --file input.txt --method astar

# Parallel processing with OpenMP
./bin/spellchecker --parallel data/large_test.txt --threads 8

# Tone analysis
./bin/spellchecker --tone data/sample_text.txt

# Visualization mode
./bin/spellchecker --visualize

# Export HTML report
./bin/spellchecker --visualize --export-html report.html
```

### Command Line Options

| Option | Description |
|--------|-------------|
| `--ui` | Launch interactive UI (default) |
| `--benchmark` | Run performance benchmarks |
| `--check <word>` | Check a single word |
| `--file <path>` | Check a file for spelling errors |
| `--dict <path>` | Specify dictionary file |
| `--method <name>` | Choose method: astar, trie, kdtree |
| `--parallel <file>` | Process file with parallel OpenMP |
| `--tone <file>` | Analyze text tone and sentiment |
| `--visualize` | Show benchmark visualizations |
| `--export-html <file>` | Export report to HTML |
| `--threads <n>` | Set thread count for parallel mode |
| `--help` | Show help message |

### Running Tests

```bash
make test
./test_runner
```

---

## Benchmarks and Results

### Test Configuration

| Parameter | Value |
|-----------|-------|
| Dictionary Size | ~1000 words (standard) / ~15000 words (large) |
| Test Words | 15 misspelled words |
| Max Edit Distance | 2 |
| Max Suggestions | 5 |
| Iterations | 100 per test |
| OpenMP Threads | 1-8 (scalability test) |

### Performance Comparison

| Method | Avg Time (ms) | Min Time (ms) | Max Time (ms) | Throughput (words/s) |
|--------|---------------|---------------|---------------|----------------------|
| Trie   | 0.036         | 0.028         | 0.110         | ~27,000              |
| KD-Tree| 0.021         | 0.013         | 0.067         | ~47,000              |
| A*     | 0.054         | 0.042         | 0.103         | ~18,500              |

### Parallel Processing Performance (OpenMP)

| Threads | Time (ms) | Speedup | Efficiency |
|---------|-----------|---------|------------|
| 1       | 3.0       | 1.76x   | 176%       |
| 2       | 2.9       | 1.92x   | 96%        |
| 4       | 2.9       | 2.23x   | 56%        |
| 8       | 2.4       | 3.53x   | 44%        |

**Best Result**: 3.53x speedup with 8 threads, 71.66% faster than sequential!

### Suggestion Quality Comparison

| Word | Trie Suggestions | KD-Tree Suggestions | A* Suggestions |
|------|------------------|---------------------|----------------|
| helo | hello, help      | hello, hallo        | hello, help    |
| wrold| world            | world, word         | world          |

*Full results available in `benchmarks/results/`*

---

## Performance Analysis

### Time Complexity Analysis

| Operation | Trie | KD-Tree | A* |
|-----------|------|---------|-----|
| Exact Lookup | O(L) | O(log N) | O(L) |
| Fuzzy Search | O(26^d × L) | O(N^(1-1/D) + k) | O(b^d) where b = branching factor |

### Space Complexity

| Structure | Complexity | Notes |
|-----------|------------|-------|
| Trie | O(N × L) | Shares prefixes |
| KD-Tree | O(N × D) | D = 5 dimensions |
| A* State | O(V) | V = visited states |

### Scalability

```
Performance vs Dictionary Size:
- Trie: Linear growth with dictionary size
- KD-Tree: Logarithmic growth for search
- A*: Depends on edit distance bound
```

---

## Project Structure

```
MidtermDSA/
├── README.md                    # This file
├── Makefile                     # Build configuration (with OpenMP)
├── main.cpp                     # Original implementation (preserved)
│
├── include/                     # Header files
│   ├── trie.h                   # Trie class declaration
│   ├── kdtree.h                 # KD-Tree class declaration
│   ├── astar_spellcheck.h       # A* spellchecker declaration
│   ├── spellchecker.h           # Main spellchecker interface
│   ├── ui.h                     # User interface declaration
│   ├── benchmark.h              # Benchmarking tools declaration
│   ├── parallel_processor.h     # OpenMP parallel processing
│   ├── tone_analyzer.h          # Sentiment/tone analysis
│   └── visualizer.h             # ASCII charts and reports
│
├── src/                         # Source files
│   ├── main.cpp                 # Application entry point
│   ├── trie.cpp                 # Trie implementation
│   ├── kdtree.cpp               # KD-Tree implementation
│   ├── astar_spellcheck.cpp     # A* algorithm implementation
│   ├── spellchecker.cpp         # Spellchecker implementation
│   ├── ui.cpp                   # User interface (14 menu options)
│   ├── benchmark.cpp            # Benchmarking implementation
│   ├── parallel_processor.cpp   # OpenMP parallel spell checking
│   ├── tone_analyzer.cpp        # NLP tone analysis
│   └── visualizer.cpp           # Chart generation
│
├── tests/                       # Test files
│   └── test_all.cpp             # Unit tests (18 tests)
│
├── benchmarks/                  # Benchmark related files
│   └── results/
│       ├── data/                # Raw benchmark data (CSV, MD)
│       └── plots/               # Generated visualizations
│
├── data/                        # Data files
│   ├── dictionary.txt           # Standard dictionary (~1000 words)
│   ├── dictionary_large.txt     # Extended dictionary (~15000 words)
│   ├── sample_text.txt          # Sample text for testing
│   ├── large_test.txt           # Large test file for benchmarks
│   └── test_texts/              # Additional test files
│
└── docs/                        # Documentation
    └── poster.pdf               # Project poster (for presentation)
```

---

## References

1. **Levenshtein, V.I.** (1966). "Binary codes capable of correcting deletions, insertions, and reversals." *Soviet Physics Doklady*, 10(8): 707-710.

2. **Hart, P.E., Nilsson, N.J., Raphael, B.** (1968). "A Formal Basis for the Heuristic Determination of Minimum Cost Paths." *IEEE Transactions on Systems Science and Cybernetics*, 4(2): 100-107.

3. **Bentley, J.L.** (1975). "Multidimensional binary search trees used for associative searching." *Communications of the ACM*, 18(9): 509-517.

4. **Cormen, T.H., Leiserson, C.E., Rivest, R.L., Stein, C.** (2009). *Introduction to Algorithms* (3rd ed.). MIT Press.

5. **Schulz, K.U., Mihov, S.** (2002). "Fast String Correction with Levenshtein Automata." *International Journal on Document Analysis and Recognition*, 5(1): 67-85.

6. **OpenMP Architecture Review Board** (2020). "OpenMP Application Programming Interface, Version 5.1." [https://www.openmp.org/](https://www.openmp.org/)

---

## License

This project is developed for educational purposes as part of the DSA course midterm evaluation.
