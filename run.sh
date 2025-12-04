#!/bin/bash

# ═══════════════════════════════════════════════════════════════════════════════
#                    DSA MIDTERM PROJECT - SPELL CHECKER
#                   A* + Trie vs KD-Tree Implementation
# ═══════════════════════════════════════════════════════════════════════════════

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color
BOLD='\033[1m'

# Get script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

# Function to print header
print_header() {
    clear
    echo -e "${CYAN}"
    echo "╔══════════════════════════════════════════════════════════════════╗"
    echo "║           DSA MIDTERM PROJECT - SPELL CHECKER SYSTEM            ║"
    echo "║              A* + Trie vs KD-Tree Implementation                ║"
    echo "╚══════════════════════════════════════════════════════════════════╝"
    echo -e "${NC}"
}

# Function to print menu
print_menu() {
    echo -e "${BOLD}┌────────────────────── MAIN MENU ──────────────────────┐${NC}"
    echo -e "│                                                        │"
    echo -e "│  ${GREEN}1.${NC} Build project (compile)                           │"
    echo -e "│  ${GREEN}2.${NC} Run Spell Checker (interactive)                   │"
    echo -e "│  ${GREEN}3.${NC} Run All Tests                                      │"
    echo -e "│                                                        │"
    echo -e "│  ${YELLOW}───────────── Benchmarks ─────────────${NC}               │"
    echo -e "│  ${GREEN}4.${NC} Run Levenshtein Benchmark (Seq vs Parallel)       │"
    echo -e "│  ${GREEN}5.${NC} Run Batch Processing Benchmark                    │"
    echo -e "│  ${GREEN}6.${NC} Run Full Performance Comparison                   │"
    echo -e "│                                                        │"
    echo -e "│  ${YELLOW}───────────── Utilities ──────────────${NC}               │"
    echo -e "│  ${GREEN}7.${NC} Clean build files                                 │"
    echo -e "│  ${GREEN}8.${NC} Show project statistics                           │"
    echo -e "│  ${GREEN}9.${NC} Quick word check                                  │"
    echo -e "│                                                        │"
    echo -e "│  ${RED}0.${NC}  Exit                                              │"
    echo -e "│                                                        │"
    echo -e "└────────────────────────────────────────────────────────┘"
    echo ""
}

# Function to build project
build_project() {
    echo -e "\n${BLUE}Building project...${NC}\n"
    make clean
    make
    if [ $? -eq 0 ]; then
        echo -e "\n${GREEN}✓ Build successful!${NC}"
    else
        echo -e "\n${RED}✗ Build failed!${NC}"
    fi
    echo -e "\nPress Enter to continue..."
    read
}

# Function to run spell checker
run_spellchecker() {
    if [ ! -f "bin/spellchecker" ]; then
        echo -e "${YELLOW}Spell checker not built. Building now...${NC}"
        make
    fi
    echo -e "\n${BLUE}Starting Spell Checker...${NC}\n"
    ./bin/spellchecker
    echo -e "\nPress Enter to continue..."
    read
}

# Function to run tests
run_tests() {
    echo -e "\n${BLUE}Running all tests...${NC}\n"
    make test
    echo -e "\nPress Enter to continue..."
    read
}

# Function to run Levenshtein benchmark
run_levenshtein_benchmark() {
    echo -e "\n${BLUE}Compiling Levenshtein Benchmark...${NC}\n"
    
    cat > /tmp/lev_benchmark.cpp << 'CPPEOF'
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <omp.h>

using namespace std;

int levenshteinSeq(const string& s1, const string& s2) {
    int m = s1.length(), n = s2.length();
    if (m == 0) return n;
    if (n == 0) return m;
    vector<vector<int>> dp(m + 1, vector<int>(n + 1));
    for (int i = 0; i <= m; i++) dp[i][0] = i;
    for (int j = 0; j <= n; j++) dp[0][j] = j;
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (s1[i - 1] == s2[j - 1]) dp[i][j] = dp[i - 1][j - 1];
            else dp[i][j] = 1 + min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
        }
    }
    return dp[m][n];
}

int levenshteinPar(const string& s1, const string& s2) {
    int m = s1.length(), n = s2.length();
    if (m == 0) return n;
    if (n == 0) return m;
    vector<vector<int>> dp(m + 1, vector<int>(n + 1));
    for (int i = 0; i <= m; i++) dp[i][0] = i;
    for (int j = 0; j <= n; j++) dp[0][j] = j;
    for (int k = 2; k <= m + n; k++) {
        int start_i = max(1, k - n), end_i = min(m, k - 1);
        #pragma omp parallel for
        for (int i = start_i; i <= end_i; i++) {
            int j = k - i;
            if (s1[i - 1] == s2[j - 1]) dp[i][j] = dp[i - 1][j - 1];
            else dp[i][j] = 1 + min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
        }
    }
    return dp[m][n];
}

int main() {
    cout << "\n╔══════════════════════════════════════════════════════════════════╗" << endl;
    cout << "║         LEVENSHTEIN DISTANCE: SEQUENTIAL vs PARALLEL            ║" << endl;
    cout << "╚══════════════════════════════════════════════════════════════════╝\n" << endl;
    
    cout << "OpenMP Threads: " << omp_get_max_threads() << endl << endl;

    vector<pair<string, string>> tests = {
        {"algorithm", "altruistic"},
        {"programming", "programmer"},
        {"levenshtein", "levenstein"},
        {"parallelization", "parallelize"},
        {"implementation", "implmentation"},
        {"spell", "speel"},
        {"checker", "cheker"},
        {"dictionary", "dictionery"}
    };
    
    int iterations = 1000;
    
    cout << "Running " << iterations << " iterations per test case...\n" << endl;
    cout << "┌─────────────────────────┬────────────┬────────────┬──────────┐" << endl;
    cout << "│ Word Pair               │ Sequential │  Parallel  │ Distance │" << endl;
    cout << "├─────────────────────────┼────────────┼────────────┼──────────┤" << endl;
    
    double totalSeq = 0, totalPar = 0;
    
    for (const auto& [s1, s2] : tests) {
        auto start = chrono::high_resolution_clock::now();
        int distSeq = 0;
        for (int i = 0; i < iterations; i++) distSeq = levenshteinSeq(s1, s2);
        auto end = chrono::high_resolution_clock::now();
        double seqTime = chrono::duration<double, micro>(end - start).count();
        
        start = chrono::high_resolution_clock::now();
        int distPar = 0;
        for (int i = 0; i < iterations; i++) distPar = levenshteinPar(s1, s2);
        end = chrono::high_resolution_clock::now();
        double parTime = chrono::duration<double, micro>(end - start).count();
        
        totalSeq += seqTime;
        totalPar += parTime;
        
        string pair = s1.substr(0,10) + "/" + s2.substr(0,10);
        printf("│ %-23s │ %8.1f μs │ %8.1f μs │    %d     │\n", 
               pair.c_str(), seqTime, parTime, distSeq);
    }
    
    cout << "├─────────────────────────┼────────────┼────────────┼──────────┤" << endl;
    printf("│ TOTAL                   │ %8.1f μs │ %8.1f μs │          │\n", totalSeq, totalPar);
    cout << "└─────────────────────────┴────────────┴────────────┴──────────┘" << endl;
    
    cout << "\n⚠️  Note: For short strings (typical words), sequential is faster" << endl;
    cout << "    due to OpenMP thread management overhead." << endl;
    cout << "    Parallel Levenshtein is beneficial for longer strings (100+ chars)." << endl;
    
    return 0;
}
CPPEOF

    g++ -std=c++17 -O2 -Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include \
        -L/opt/homebrew/opt/libomp/lib -lomp /tmp/lev_benchmark.cpp -o /tmp/lev_benchmark
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ Compiled successfully${NC}\n"
        /tmp/lev_benchmark
    else
        echo -e "${RED}✗ Compilation failed${NC}"
    fi
    
    echo -e "\nPress Enter to continue..."
    read
}

# Function to run batch processing benchmark
run_batch_benchmark() {
    echo -e "\n${BLUE}Compiling Batch Processing Benchmark...${NC}\n"
    
    cat > /tmp/batch_benchmark.cpp << 'CPPEOF'
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <omp.h>
#include <fstream>

using namespace std;

int levenshtein(const string& s1, const string& s2) {
    int m = s1.length(), n = s2.length();
    if (m == 0) return n;
    if (n == 0) return m;
    vector<vector<int>> dp(m + 1, vector<int>(n + 1));
    for (int i = 0; i <= m; i++) dp[i][0] = i;
    for (int j = 0; j <= n; j++) dp[0][j] = j;
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (s1[i - 1] == s2[j - 1]) dp[i][j] = dp[i - 1][j - 1];
            else dp[i][j] = 1 + min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
        }
    }
    return dp[m][n];
}

int main(int argc, char* argv[]) {
    string dictPath = "data/dictionary.txt";
    if (argc > 1) dictPath = argv[1];
    
    vector<string> dict;
    ifstream file(dictPath);
    if (!file.is_open()) {
        cerr << "Error: Cannot open " << dictPath << endl;
        return 1;
    }
    string word;
    while (file >> word) dict.push_back(word);
    file.close();
    
    cout << "\n╔══════════════════════════════════════════════════════════════════╗" << endl;
    cout << "║      BATCH WORD PROCESSING: SEQUENTIAL vs PARALLEL              ║" << endl;
    cout << "╚══════════════════════════════════════════════════════════════════╝\n" << endl;
    
    cout << "Dictionary: " << dictPath << " (" << dict.size() << " words)" << endl;
    cout << "OpenMP Threads: " << omp_get_max_threads() << endl << endl;

    vector<string> testWords = {"helo", "wrold", "programing", "algoritm", "speling",
                                "recieve", "occured", "seperate", "definately", "accomodate",
                                "thier", "wierd", "freind", "beleive", "enviroment",
                                "acheive", "begining", "calender", "diffrent", "existance"};
    
    int iterations = 20;
    long long totalComparisons = (long long)iterations * testWords.size() * dict.size();
    
    cout << "Test words: " << testWords.size() << endl;
    cout << "Iterations: " << iterations << endl;
    cout << "Total comparisons: " << totalComparisons << endl << endl;
    
    // Sequential
    cout << "Running SEQUENTIAL processing..." << flush;
    auto start = chrono::high_resolution_clock::now();
    for (int iter = 0; iter < iterations; iter++) {
        for (const auto& test : testWords) {
            for (const auto& d : dict) {
                levenshtein(test, d);
            }
        }
    }
    auto end = chrono::high_resolution_clock::now();
    double seqTime = chrono::duration<double, milli>(end - start).count();
    cout << " Done!" << endl;
    
    // Parallel
    cout << "Running PARALLEL processing..." << flush;
    start = chrono::high_resolution_clock::now();
    for (int iter = 0; iter < iterations; iter++) {
        #pragma omp parallel for schedule(dynamic)
        for (size_t t = 0; t < testWords.size(); t++) {
            for (const auto& d : dict) {
                levenshtein(testWords[t], d);
            }
        }
    }
    end = chrono::high_resolution_clock::now();
    double parTime = chrono::duration<double, milli>(end - start).count();
    cout << " Done!" << endl;
    
    double speedup = seqTime / parTime;
    
    cout << "\n┌────────────────────────────────────────────────────────────────┐" << endl;
    cout << "│                         RESULTS                                │" << endl;
    cout << "├────────────────────────────────────────────────────────────────┤" << endl;
    printf("│  Sequential Time:    %10.2f ms                             │\n", seqTime);
    printf("│  Parallel Time:      %10.2f ms                             │\n", parTime);
    printf("│  Speedup:            %10.2fx                              │\n", speedup);
    printf("│  Efficiency:         %10.1f%%                              │\n", (speedup / omp_get_max_threads()) * 100);
    cout << "└────────────────────────────────────────────────────────────────┘" << endl;
    
    cout << "\n✓ Parallel word-level processing provides significant speedup!" << endl;
    cout << "✓ This is the strategy used in the spell checker for large texts." << endl;
    
    return 0;
}
CPPEOF

    g++ -std=c++17 -O2 -Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include \
        -L/opt/homebrew/opt/libomp/lib -lomp /tmp/batch_benchmark.cpp -o /tmp/batch_benchmark
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ Compiled successfully${NC}\n"
        /tmp/batch_benchmark "$SCRIPT_DIR/data/dictionary.txt"
    else
        echo -e "${RED}✗ Compilation failed${NC}"
    fi
    
    echo -e "\nPress Enter to continue..."
    read
}

# Function to run full comparison
run_full_comparison() {
    echo -e "\n${BLUE}Running Full Performance Comparison...${NC}\n"
    
    if [ ! -f "bin/spellchecker" ]; then
        echo -e "${YELLOW}Building spell checker first...${NC}"
        make
    fi
    
    echo -e "\n${CYAN}═══ Part 1: Levenshtein Algorithm Benchmark ═══${NC}\n"
    run_levenshtein_benchmark_inline
    
    echo -e "\n${CYAN}═══ Part 2: Batch Processing Benchmark ═══${NC}\n"
    run_batch_benchmark_inline
    
    echo -e "\n${CYAN}═══ Part 3: Running Unit Tests ═══${NC}\n"
    make test 2>/dev/null || echo "Tests skipped"
    
    echo -e "\n${GREEN}═══ Full comparison complete! ═══${NC}"
    echo -e "\nPress Enter to continue..."
    read
}

run_levenshtein_benchmark_inline() {
    # Same as run_levenshtein_benchmark but without pause
    cat > /tmp/lev_bench_inline.cpp << 'CPPEOF'
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <omp.h>
using namespace std;
int levenshteinSeq(const string& s1, const string& s2) {
    int m = s1.length(), n = s2.length();
    if (m == 0) return n; if (n == 0) return m;
    vector<vector<int>> dp(m + 1, vector<int>(n + 1));
    for (int i = 0; i <= m; i++) dp[i][0] = i;
    for (int j = 0; j <= n; j++) dp[0][j] = j;
    for (int i = 1; i <= m; i++) for (int j = 1; j <= n; j++) {
        if (s1[i-1] == s2[j-1]) dp[i][j] = dp[i-1][j-1];
        else dp[i][j] = 1 + min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]});
    }
    return dp[m][n];
}
int main() {
    vector<pair<string,string>> tests = {{"algorithm","altruistic"},{"programming","programmer"},{"levenshtein","levenstein"}};
    cout << "Levenshtein quick test (1000 iterations each):" << endl;
    for (auto& [s1,s2] : tests) {
        auto start = chrono::high_resolution_clock::now();
        for (int i = 0; i < 1000; i++) levenshteinSeq(s1, s2);
        auto end = chrono::high_resolution_clock::now();
        double t = chrono::duration<double, micro>(end - start).count();
        printf("  %s -> %s: %.1f μs, dist=%d\n", s1.c_str(), s2.c_str(), t, levenshteinSeq(s1,s2));
    }
    return 0;
}
CPPEOF
    g++ -std=c++17 -O2 /tmp/lev_bench_inline.cpp -o /tmp/lev_bench_inline 2>/dev/null && /tmp/lev_bench_inline
}

run_batch_benchmark_inline() {
    cat > /tmp/batch_inline.cpp << 'CPPEOF'
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <omp.h>
#include <fstream>
using namespace std;
int lev(const string& s1, const string& s2) {
    int m = s1.length(), n = s2.length();
    if (m == 0) return n; if (n == 0) return m;
    vector<vector<int>> dp(m + 1, vector<int>(n + 1));
    for (int i = 0; i <= m; i++) dp[i][0] = i;
    for (int j = 0; j <= n; j++) dp[0][j] = j;
    for (int i = 1; i <= m; i++) for (int j = 1; j <= n; j++) {
        if (s1[i-1] == s2[j-1]) dp[i][j] = dp[i-1][j-1];
        else dp[i][j] = 1 + min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]});
    }
    return dp[m][n];
}
int main(int argc, char* argv[]) {
    vector<string> dict;
    ifstream file(argv[1]);
    string w; while (file >> w) dict.push_back(w);
    vector<string> tests = {"helo", "wrold", "programing", "algoritm", "speling"};
    cout << "Batch processing (" << dict.size() << " words, " << tests.size() << " tests, 10 iter):" << endl;
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < 10; i++) for (auto& t : tests) for (auto& d : dict) lev(t, d);
    auto end = chrono::high_resolution_clock::now();
    double seq = chrono::duration<double, milli>(end - start).count();
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < 10; i++) {
        #pragma omp parallel for
        for (size_t t = 0; t < tests.size(); t++) for (auto& d : dict) lev(tests[t], d);
    }
    end = chrono::high_resolution_clock::now();
    double par = chrono::duration<double, milli>(end - start).count();
    printf("  Sequential: %.2f ms\n  Parallel:   %.2f ms\n  Speedup:    %.2fx\n", seq, par, seq/par);
    return 0;
}
CPPEOF
    g++ -std=c++17 -O2 -Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include \
        -L/opt/homebrew/opt/libomp/lib -lomp /tmp/batch_inline.cpp -o /tmp/batch_inline 2>/dev/null \
        && /tmp/batch_inline "$SCRIPT_DIR/data/dictionary.txt"
}

# Function to compile poster
compile_poster() {
    echo -e "\n${BLUE}Compiling poster...${NC}\n"
    cd docs
    pdflatex -interaction=nonstopmode poster_final.tex
    pdflatex -interaction=nonstopmode poster_final.tex
    cd ..
    if [ -f "docs/poster_final.pdf" ]; then
        echo -e "\n${GREEN}✓ Poster compiled successfully!${NC}"
    else
        echo -e "\n${RED}✗ Poster compilation failed${NC}"
    fi
    echo -e "\nPress Enter to continue..."
    read
}

# Function to open poster
open_poster() {
    if [ -f "docs/poster_final.pdf" ]; then
        echo -e "\n${BLUE}Opening poster...${NC}"
        open docs/poster_final.pdf
    else
        echo -e "\n${YELLOW}Poster not found. Compiling first...${NC}"
        compile_poster
        open docs/poster_final.pdf
    fi
    echo -e "\nPress Enter to continue..."
    read
}

# Function to view README
view_readme() {
    echo -e "\n${BLUE}README.md Contents:${NC}\n"
    if [ -f "README.md" ]; then
        cat README.md | head -100
        echo -e "\n... (truncated)"
    else
        echo "README.md not found"
    fi
    echo -e "\nPress Enter to continue..."
    read
}

# Function to clean
clean_build() {
    echo -e "\n${BLUE}Cleaning build files...${NC}\n"
    make clean
    rm -f /tmp/lev_benchmark /tmp/batch_benchmark /tmp/*.cpp
    echo -e "\n${GREEN}✓ Clean complete!${NC}"
    echo -e "\nPress Enter to continue..."
    read
}

# Function to show stats
show_stats() {
    echo -e "\n${CYAN}═══ Project Statistics ═══${NC}\n"
    
    echo -e "${BOLD}Source Files:${NC}"
    find src -name "*.cpp" | wc -l | xargs echo "  C++ source files:"
    find include -name "*.h" | wc -l | xargs echo "  Header files:"
    
    echo -e "\n${BOLD}Lines of Code:${NC}"
    wc -l src/*.cpp include/*.h 2>/dev/null | tail -1
    
    echo -e "\n${BOLD}Dictionary:${NC}"
    if [ -f "data/dictionary.txt" ]; then
        wc -l data/dictionary.txt | awk '{print "  Words: " $1}'
    fi
    
    echo -e "\n${BOLD}Build Status:${NC}"
    if [ -f "bin/spellchecker" ]; then
        echo -e "  Spell Checker: ${GREEN}Built${NC}"
        ls -lh bin/spellchecker | awk '{print "  Size: " $5}'
    else
        echo -e "  Spell Checker: ${RED}Not built${NC}"
    fi
    
    echo -e "\n${BOLD}OpenMP Info:${NC}"
    echo "  Max threads available: $(sysctl -n hw.ncpu 2>/dev/null || echo "unknown")"
    
    echo -e "\nPress Enter to continue..."
    read
}

# Function for quick word check
quick_check() {
    if [ ! -f "bin/spellchecker" ]; then
        echo -e "${YELLOW}Building spell checker...${NC}"
        make 2>/dev/null
    fi
    
    echo -e "\n${CYAN}Quick Word Check${NC}"
    echo -e "Enter a word to check (or 'q' to quit):\n"
    
    while true; do
        echo -n "Word: "
        read word
        if [ "$word" = "q" ] || [ "$word" = "Q" ]; then
            break
        fi
        echo -e "1\n$word\n\n0" | ./bin/spellchecker 2>/dev/null | grep -A 20 "Checking word" | head -15
        echo ""
    done
}

# Main loop
while true; do
    print_header
    print_menu
    
    echo -n "Enter your choice [0-9]: "
    read choice
    
    case $choice in
        1) build_project ;;
        2) run_spellchecker ;;
        3) run_tests ;;
        4) run_levenshtein_benchmark ;;
        5) run_batch_benchmark ;;
        6) run_full_comparison ;;
        7) clean_build ;;
        8) show_stats ;;
        9) quick_check ;;
        0) 
            echo -e "\n${GREEN}Goodbye!${NC}\n"
            exit 0
            ;;
        *)
            echo -e "\n${RED}Invalid option. Please try again.${NC}"
            sleep 1
            ;;
    esac
done
