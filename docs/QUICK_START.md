# Quick Start Guide

## Spell Checker System - DSA Midterm Project

---

## 🚀 Getting Started

### 1. Build the Project

```bash
cd MidtermDSA
make all
```

### 2. Run Tests

```bash
make test
# Expected: 18/18 tests passed ✓
```

### 3. Launch Interactive UI

```bash
./bin/spellchecker --ui
```

---

## 📖 Usage Examples

### Check a Single Word

```bash
./bin/spellchecker --check "helo"
```

Output:
```
Checking word: "helo"
Method: A* Search
Suggestions: hello, help, held, hero
Time: 0.05 ms
```

### Check a Text File

```bash
./bin/spellchecker --file data/sample_text.txt --method astar
```

### Parallel Processing (OpenMP)

```bash
./bin/spellchecker --parallel data/large_test.txt --threads 8
```

Output:
```
╔══════════════════════════════════════════════════════════════╗
║          SEQUENTIAL vs PARALLEL COMPARISON (OpenMP)         ║
╚══════════════════════════════════════════════════════════════╝

OpenMP Version: 202011
Configured Threads: 8

┌─────────────────────┬──────────────────┬───────────────────────┤
│      Metric         │    Sequential    │       Parallel        │
├─────────────────────┼──────────────────┼───────────────────────┤
│ Time (ms)           │            11.04 │                  3.13 │
│ Throughput (w/s)    │         24182.04 │              85333.04 │
└─────────────────────┴──────────────────┴───────────────────────┘

  Speedup: 3.53x ✓
```

### Tone Analysis

```bash
./bin/spellchecker --tone data/sample_text.txt
```

Output:
```
╔══════════════════════════════════════════════════════════════╗
║                    TONE ANALYSIS RESULTS                     ║
╚══════════════════════════════════════════════════════════════╝

┌─────────────────── SENTIMENT ANALYSIS ─────────────────┐
│  Overall Sentiment:                      POSITIVE ✓ │
│  Compound Score:                                1.000 │
└─────────────────────────────────────────────────────────┘

  Dominant Tone: Positive
```

### Run Benchmarks with Visualization

```bash
./bin/spellchecker --visualize
```

### Export HTML Report

```bash
./bin/spellchecker --visualize --export-html report.html
```

---

## 🎮 Interactive UI Menu

```
╔══════════════════════════════════════════════════════════════╗
║                    SPELL CHECKER MENU                        ║
╠══════════════════════════════════════════════════════════════╣
║  1. Check single word                                        ║
║  2. Check text input                                         ║
║  3. Check file                                               ║
║  4. Compare methods for word                                 ║
║  5. Run benchmarks                                           ║
║  6. Switch dictionary                                        ║
║  7. View statistics                                          ║
║  8. Run accuracy test                                        ║
║  9. Demo mode                                                ║
║ 10. Parallel processing                                      ║
║ 11. Tone analysis                                            ║
║ 12. Visualization                                            ║
║ 13. Export report                                            ║
║  0. Exit                                                     ║
╚══════════════════════════════════════════════════════════════╝
```

---

## 📊 Method Comparison

| Method | Best For | Speed | Quality |
|--------|----------|-------|---------|
| **Trie** | Prefix matching | ⭐⭐⭐ | ⭐⭐ |
| **KD-Tree** | Semantic search | ⭐⭐⭐⭐ | ⭐⭐ |
| **A\*** | Accurate suggestions | ⭐⭐ | ⭐⭐⭐⭐ |

---

## 🔧 Troubleshooting

### OpenMP Not Found (macOS)

```bash
brew install libomp
```

### Build Errors

```bash
make clean
make all
```

### Permission Denied

```bash
chmod +x bin/spellchecker
```

---

## 📁 Important Files

| File | Description |
|------|-------------|
| `data/dictionary.txt` | Standard dictionary (~1000 words) |
| `data/dictionary_large.txt` | Extended dictionary (~15000 words) |
| `benchmarks/results/data/` | Benchmark results (CSV, MD) |

---

**Happy Spell Checking!** 🎉
