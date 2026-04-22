// Implementation for Problem 126 - Xiaoh's Game of Life
// The OJ will include this header and call the following functions:
//   void Initialize();
//   void Tick();
//   void PrintGame();
//   int GetLiveCell();
// We implement efficient parsing of a run-length encoded pattern, Game of Life step,
// and compact pattern printing per the problem statement.

#include <bits/stdc++.h>
using namespace std;

// Internal state
static int g_rows = 0, g_cols = 0; // rows = number of lines; cols = number of columns
static vector<unsigned char> g_grid;     // current grid, 0 dead, 1 alive
static vector<unsigned char> g_next;     // next grid buffer
static long long g_live = 0;             // current live cells

// Helper: index in 1D array
static inline int idx(int r, int c) { return r * g_cols + c; }

// Helper: clamp add with bounds check
static inline bool in_bounds(int r, int c) {
    return (unsigned)r < (unsigned)g_rows && (unsigned)c < (unsigned)g_cols;
}

// Parse the run-length encoded pattern lines into g_grid.
static void parse_pattern(const string &pattern) {
    int r = 0, c = 0;
    long long cnt = 0; // count accumulator for digits
    auto flush_cells = [&](long long count, unsigned char val) {
        for (long long k = 0; k < count && r < g_rows; ++k) {
            if (c >= g_cols) {
                // end of line implicitly, move to next line
                c = 0; ++r;
                if (r >= g_rows) break;
            }
            g_grid[idx(r, c)] = val;
            if (val) ++g_live;
            ++c;
        }
    };

    for (size_t i = 0; i < pattern.size(); ++i) {
        char ch = pattern[i];
        if (ch >= '0' && ch <= '9') {
            cnt = cnt * 10 + (ch - '0');
        } else if (ch == 'b' || ch == 'o') {
            long long times = (cnt == 0 ? 1 : cnt);
            flush_cells(times, (unsigned char)(ch == 'o'));
            cnt = 0;
        } else if (ch == '$') {
            long long lines = (cnt == 0 ? 1 : cnt);
            // Move to next lines, fill nothing (remain dead). If c < g_cols, trailing cells are dead implicitly.
            for (long long t = 0; t < lines; ++t) {
                c = 0; ++r;
                if (r >= g_rows) break;
            }
            cnt = 0;
        } else if (ch == '!') {
            break;
        } else {
            // ignore other characters (e.g., spaces) just in case
        }
    }
}

void Initialize() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Read first line: cols and rows (note: README states first line two integers: columns then rows)
    if (!(cin >> g_cols >> g_rows)) {
        return;
    }
    string line;
    string pattern;
    // consume endline remainder
    getline(cin, line);
    // Read subsequent lines until a line containing '!'
    while (std::getline(cin, line)) {
        pattern += line;
        if (line.find('!') != string::npos) break;
    }

    g_grid.assign((size_t)g_rows * (size_t)g_cols, 0);
    g_next.assign((size_t)g_rows * (size_t)g_cols, 0);
    g_live = 0;
    parse_pattern(pattern);
}

void Tick() {
    if (g_rows == 0 || g_cols == 0) return;
    // Iterate cells and compute neighbor counts
    long long new_live = 0;
    const int R = g_rows, C = g_cols;
    for (int r = 0; r < R; ++r) {
        for (int c = 0; c < C; ++c) {
            int alive = 0;
            // unrolled neighbor loops for speed
            for (int dr = -1; dr <= 1; ++dr) {
                int rr = r + dr;
                if ((unsigned)rr >= (unsigned)R) continue;
                int base = rr * C;
                int c0 = c - 1; if ((unsigned)c0 < (unsigned)C) alive += g_grid[base + c0];
                if (dr != 0) { // we already accounted left/right for rr rows; center cell different row not counted
                    int c1 = c; if ((unsigned)c1 < (unsigned)C) alive += g_grid[base + c1];
                }
                int c2 = c + 1; if ((unsigned)c2 < (unsigned)C) alive += g_grid[base + c2];
            }
            // subtract self since when dr=0, c1=c got skipped by condition dr!=0, so self not included. For dr=0, we added left and right only.
            unsigned char cur = g_grid[idx(r, c)];
            // Rules
            unsigned char nxt;
            if (cur) {
                // alive survives if 2 or 3 neighbors
                nxt = (alive == 2 || alive == 3) ? 1 : 0;
            } else {
                // dead becomes alive if exactly 3 neighbors
                nxt = (alive == 3) ? 1 : 0;
            }
            g_next[idx(r, c)] = nxt;
            new_live += nxt;
        }
    }
    g_grid.swap(g_next);
    g_live = new_live;
}

// Print the grid in compressed RLE-like format per spec
void PrintGame() {
    cout << g_cols << ' ' << g_rows << '\n';
    // produce a single-line pattern string
    // For each row, emit runs of b/o and a trailing '$'. Use counts when >1.
    for (int r = 0; r < g_rows; ++r) {
        int c = 0;
        while (c < g_cols) {
            unsigned char val = g_grid[idx(r, c)];
            int start = c;
            while (c < g_cols && g_grid[idx(r, c)] == val) ++c;
            int run = c - start;
            if (run > 1) cout << run;
            cout << (val ? 'o' : 'b');
        }
        cout << '$';
    }
    cout << '!';
}

int GetLiveCell() {
    return (int)g_live;
}

