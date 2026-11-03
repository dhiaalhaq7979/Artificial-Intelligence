// Search algorithms on a grid map: BFS, DFS, Uniform Cost Search and A*
// Author: Dhia Alhaq Saleh Al-Hindrish
//
// Map legend:  S start   G goal   # wall   ~ mud (cost 3)   . road (cost 1)
#include <algorithm>
#include <climits>
#include <cstdlib>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <stack>
#include <string>
#include <vector>

using namespace std;

const vector<string> MAP = {
    "S..#.......",
    ".#.#.###.#.",
    ".#...~~~.#.",
    ".####~#~.#.",
    "....#~#~...",
    ".##.#~#####",
    "....~~~...G",
};

struct Cell {
    int r, c;
    bool operator<(const Cell& o) const { return r != o.r ? r < o.r : c < o.c; }
    bool operator==(const Cell& o) const { return r == o.r && c == o.c; }
};

struct Result {
    string name;
    vector<Cell> path;
    int cost = 0;
    int expanded = 0;
    explicit Result(const string& n) : name(n) {}
};

int rows() { return static_cast<int>(MAP.size()); }
int cols() { return static_cast<int>(MAP[0].size()); }
int stepCost(Cell c) { return MAP[c.r][c.c] == '~' ? 3 : 1; }

Cell find(char ch) {
    for (int r = 0; r < rows(); r++)
        for (int c = 0; c < cols(); c++)
            if (MAP[r][c] == ch) return {r, c};
    return {-1, -1};
}

// neighbours in a fixed order: up, right, down, left
vector<Cell> neighbours(Cell cur) {
    static const int dr[] = {-1, 0, 1, 0}, dc[] = {0, 1, 0, -1};
    vector<Cell> out;
    for (int k = 0; k < 4; k++) {
        Cell n{cur.r + dr[k], cur.c + dc[k]};
        if (n.r >= 0 && n.r < rows() && n.c >= 0 && n.c < cols() && MAP[n.r][n.c] != '#') out.push_back(n);
    }
    return out;
}

vector<Cell> buildPath(map<Cell, Cell>& parent, Cell start, Cell goal) {
    vector<Cell> path;
    if (!parent.count(goal) && !(start == goal)) return path;
    for (Cell c = goal; !(c == start); c = parent[c]) path.push_back(c);
    path.push_back(start);
    reverse(path.begin(), path.end());
    return path;
}

int pathCost(const vector<Cell>& path) {
    int total = 0;
    for (size_t i = 1; i < path.size(); i++) total += stepCost(path[i]);
    return total;
}

Result bfs(Cell start, Cell goal) {
    Result res("BFS");
    queue<Cell> q;
    map<Cell, Cell> parent;
    map<Cell, bool> seen;
    q.push(start);
    seen[start] = true;
    while (!q.empty()) {
        Cell cur = q.front(); q.pop();
        res.expanded++;
        if (cur == goal) break;
        for (Cell n : neighbours(cur))
            if (!seen[n]) { seen[n] = true; parent[n] = cur; q.push(n); }
    }
    res.path = buildPath(parent, start, goal);
    res.cost = pathCost(res.path);
    return res;
}

Result dfs(Cell start, Cell goal) {
    Result res("DFS");
    stack<Cell> st;
    map<Cell, Cell> parent;
    map<Cell, bool> visited;
    st.push(start);
    while (!st.empty()) {
        Cell cur = st.top(); st.pop();
        if (visited[cur]) continue;
        visited[cur] = true;
        res.expanded++;
        if (cur == goal) break;
        vector<Cell> next = neighbours(cur);
        for (auto it = next.rbegin(); it != next.rend(); ++it)      // keep up/right/down/left order
            if (!visited[*it]) { parent[*it] = cur; st.push(*it); }
    }
    res.path = buildPath(parent, start, goal);
    res.cost = pathCost(res.path);
    return res;
}

int manhattan(Cell a, Cell b) { return abs(a.r - b.r) + abs(a.c - b.c); }

// UCS and A* share the same code: A* just adds the heuristic h(n)
Result bestFirst(Cell start, Cell goal, bool useHeuristic) {
    Result res(useHeuristic ? "A*" : "UCS");
    using Item = pair<int, pair<int, Cell>>;              // (f, (g, cell))
    priority_queue<Item, vector<Item>, greater<Item>> pq;
    map<Cell, int> bestG;
    map<Cell, Cell> parent;
    auto h = [&](Cell c) { return useHeuristic ? manhattan(c, goal) : 0; };
    pq.push({h(start), {0, start}});
    bestG[start] = 0;
    while (!pq.empty()) {
        auto [f, inner] = pq.top(); pq.pop();
        auto [g, cur] = inner;
        if (g > bestG[cur]) continue;                       // outdated entry
        res.expanded++;
        if (cur == goal) break;
        for (Cell n : neighbours(cur)) {
            int ng = g + stepCost(n);
            if (!bestG.count(n) || ng < bestG[n]) {
                bestG[n] = ng;
                parent[n] = cur;
                pq.push({ng + h(n), {ng, n}});
            }
        }
    }
    res.path = buildPath(parent, start, goal);
    res.cost = pathCost(res.path);
    return res;
}

void drawPath(const Result& res) {
    vector<string> grid = MAP;
    for (size_t i = 1; i + 1 < res.path.size(); i++) grid[res.path[i].r][res.path[i].c] = '*';
    for (const string& row : grid) cout << "    " << row << '\n';
}

int main() {
    Cell start = find('S'), goal = find('G');
    cout << "Map (" << rows() << "x" << cols() << "), # = wall, ~ = mud (cost 3), . = road (cost 1)\n";
    for (const string& row : MAP) cout << "    " << row << '\n';
    cout << "Heuristic for A*: Manhattan distance (admissible because every step costs at least 1)\n\n";

    vector<Result> results = {bfs(start, goal), dfs(start, goal), bestFirst(start, goal, false),
                              bestFirst(start, goal, true)};
    for (const Result& r : results) {
        cout << r.name << ": " << r.path.size() - 1 << " steps, cost " << r.cost << ", " << r.expanded
             << " nodes expanded\n";
        drawPath(r);
        cout << '\n';
    }
    cout << "Algorithm | Steps | Cost | Expanded | Optimal cost?\n";
    int best = results[2].cost;                                  // UCS is optimal for costs
    for (const Result& r : results)
        cout << left << setw(9) << r.name << " | " << setw(5) << r.path.size() - 1 << " | " << setw(4) << r.cost
             << " | " << setw(8) << r.expanded << " | " << (r.cost == best ? "yes" : "no") << '\n';
    return 0;
}
