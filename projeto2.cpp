#include <iostream>
#include <vector>
#include <queue>
#include <utility>
#include <algorithm>

struct Graph {
    std::vector<std::vector<int>> _stationsAdj; // Lines at each station
    std::vector<std::vector<int>> _linesAdj;    // Stations on each line
    std::vector<std::vector<int>> _lineToLine;  // Precomputed line-to-line adjacency
};

class Information {
private:
    int _numConnections;
    int _numStations;
    int _numLines;
    Graph _metroGraph;

public:
    Information(int numStations, int numConnections, int numLines);
    void addAdj(int u, int v, int line);
    void graphConstruct();
    void precomputeLineAdj();
    int BFSCalculate(int start, int target);
    void calculateSolution();
};

Information::Information(int numStations, int numConnections, int numLines)
    : _numConnections(numConnections), _numStations(numStations), _numLines(numLines) {
    _metroGraph._linesAdj.resize(numLines + 1);
    _metroGraph._stationsAdj.resize(numStations + 1);
    _metroGraph._lineToLine.resize(numLines + 1);
    graphConstruct();
    precomputeLineAdj();
    calculateSolution();
}

void Information::addAdj(int u, int v, int line) {
    _metroGraph._stationsAdj[u].push_back(line);
    _metroGraph._stationsAdj[v].push_back(line);
    _metroGraph._linesAdj[line].push_back(u);
    _metroGraph._linesAdj[line].push_back(v);
}

void Information::graphConstruct() {
    int u, v, line;
    for (int i = 0; i < _numConnections; i++) {
        std::cin >> u >> v >> line;
        addAdj(u, v, line);
    }
}

void Information::precomputeLineAdj() {
    // Create line-to-line adjacency by traversing each station's lines
    for (int station = 1; station <= _numStations; ++station) {
        const auto& lines = _metroGraph._stationsAdj[station];
        for (size_t i = 0; i < lines.size(); ++i) {
            for (size_t j = i + 1; j < lines.size(); ++j) {
                int line1 = lines[i];
                int line2 = lines[j];
                _metroGraph._lineToLine[line1].push_back(line2);
                _metroGraph._lineToLine[line2].push_back(line1);
            }
        }
    }

    // Remove duplicates in line-to-line adjacency
    for (int line = 1; line <= _numLines; ++line) {
        std::vector<int>& adjLines = _metroGraph._lineToLine[line];
        std::sort(adjLines.begin(), adjLines.end());
        adjLines.erase(std::unique(adjLines.begin(), adjLines.end()), adjLines.end());
    }
}

int Information::BFSCalculate(int start, int target) {
    if (start == target) return 0;

    std::vector<bool> visitedStations(_numStations + 1, false);
    std::vector<bool> visitedLines(_numLines + 1, false);
    std::queue<std::pair<int, int>> queue; // {station, line changes}

    queue.push(std::make_pair(start, 0));
    visitedStations[start] = true;

    while (!queue.empty()) {
        std::pair<int, int> current = queue.front();
        queue.pop();

        int currentStation = current.first;
        int lineChanges = current.second;

        for (int line : _metroGraph._stationsAdj[currentStation]) {
            if (visitedLines[line]) continue;
            visitedLines[line] = true;

            for (int nextStation : _metroGraph._linesAdj[line]) {
                if (nextStation == target) return lineChanges;
                if (!visitedStations[nextStation]) {
                    visitedStations[nextStation] = true;
                    queue.push(std::make_pair(nextStation, lineChanges + 1));
                }
            }
        }
    }
    return -1; // No path found
}

void Information::calculateSolution() {
    auto bfs = [&](int start) {
        std::vector<int> distances(_numStations + 1, -1);
        std::queue<int> queue;
        queue.push(start);
        distances[start] = 0;

        int farthestStation = start;
        int maxDistance = 0;

        while (!queue.empty()) {
            int station = queue.front();
            queue.pop();

            for (int line : _metroGraph._stationsAdj[station]) {
                for (int nextStation : _metroGraph._linesAdj[line]) {
                    if (distances[nextStation] == -1) { // Unvisited
                        distances[nextStation] = distances[station] + 1;
                        queue.push(nextStation);
                        if (distances[nextStation] > maxDistance) {
                            maxDistance = distances[nextStation];
                            farthestStation = nextStation;
                        }
                    }
                }
            }
        }

        return std::make_pair(farthestStation, maxDistance);
    };

    // Perform two BFS traversals
    std::pair<int, int> result1 = bfs(1);
    int farthestStation = result1.first;

    std::pair<int, int> result2 = bfs(farthestStation);
    int maxLength = result2.second;

    std::cout << maxLength - 1 << std::endl;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int numStations, numConnections, numLines;
    std::cin >> numStations >> numConnections >> numLines;

    Information info(numStations, numConnections, numLines);

    return 0;
}