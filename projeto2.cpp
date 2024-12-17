#include <iostream>
#include <vector>
#include <unordered_set>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <set>

struct Graph {
    std::vector<std::unordered_set<int>> stationsAdj;  // Adjacent lines for stations
    std::vector<std::unordered_set<int>> linesAdj;     // Adjacent stations for lines
};

class Information {
private:
    int numConnections;
    int numStations;
    Graph metroGraph;
    std::set<int> stationSet; // To handle non-sequential station numbers

public:
    Information(int numStations, int numConnections, int numLines);
    void addAdj(int u, int v, int line);
    void buildGraph();
    int BFSCalculate(int start, int target);
    void calculateSolution();
};

Information::Information(int numStations, int numConnections, int numLines)
    : numConnections(numConnections), numStations(numStations) {
    metroGraph.stationsAdj.resize(numStations + 1);
    metroGraph.linesAdj.resize(numLines + 1);
    buildGraph();
    calculateSolution();
}

void Information::addAdj(int u, int v, int line) {
    // Create bidirectional connections between stations and lines
    metroGraph.stationsAdj[u].insert(line);
    metroGraph.stationsAdj[v].insert(line);
    metroGraph.linesAdj[line].insert(u);
    metroGraph.linesAdj[line].insert(v);
    stationSet.insert(u);
    stationSet.insert(v);
}

void Information::buildGraph() {
    int u, v, line;
    for (int i = 0; i < numConnections; i++) {
        std::cin >> u >> v >> line;
        addAdj(u, v, line);
    }
}

int Information::BFSCalculate(int start, int target) {
    if (start == target) return 0;

    std::queue<std::pair<int, int>> q;  // {station, line changes}
    std::unordered_map<int, bool> visitedStations;
    std::unordered_map<int, int> visitedLines;

    q.push({start, 0});
    visitedStations[start] = true;

    while (!q.empty()) {
        int currentStation = q.front().first;
        int lineChanges = q.front().second;
        q.pop();

        for (int line : metroGraph.stationsAdj[currentStation]) {
            if (visitedLines.count(line) == 0 || visitedLines[line] > lineChanges) {
                visitedLines[line] = lineChanges;

                for (int nextStation : metroGraph.linesAdj[line]) {
                    if (nextStation == target) return lineChanges;

                    if (!visitedStations[nextStation]) {
                        visitedStations[nextStation] = true;
                        q.push({nextStation, lineChanges + 1});
                    }
                }
            }
        }
    }

    return -1;  // No path found
}

void Information::calculateSolution() {
    auto bfs = [&](int start) {
        std::unordered_map<int, int> distances;
        std::queue<int> q;

        q.push(start);
        distances[start] = 0;

        int farthestStation = start;
        int maxDistance = 0;

        while (!q.empty()) {
            int station = q.front();
            q.pop();

            for (int line : metroGraph.stationsAdj[station]) {
                for (int nextStation : metroGraph.linesAdj[line]) {
                    if (distances.find(nextStation) == distances.end()) { // Unvisited
                        distances[nextStation] = distances[station] + 1;
                        q.push(nextStation);
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

    // Perform the first BFS from the first station in the set
    std::pair<int, int> result1 = bfs(*stationSet.begin());
    int farthestStation = result1.first;

    // Perform the second BFS from the farthest station found in the first BFS
    std::pair<int, int> result2 = bfs(farthestStation);
    int maxLineChanges = result2.second;

    // Check if all stations are reachable
    std::unordered_map<int, int> distances;
    std::queue<int> q;
    q.push(*stationSet.begin());
    distances[*stationSet.begin()] = 0;

    while (!q.empty()) {
        int station = q.front();
        q.pop();

        for (int line : metroGraph.stationsAdj[station]) {
            for (int nextStation : metroGraph.linesAdj[line]) {
                if (distances.find(nextStation) == distances.end()) { // Unvisited
                    distances[nextStation] = distances[station] + 1;
                    q.push(nextStation);
                }
            }
        }
    }

    int num = 0;
    for (int station : stationSet) {
        num++;
        if (distances.find(station) == distances.end()) {
            std::cout << -1 << std::endl;
            return;
        }
    }
    if (num != numStations) {
        std::cout << -1 << std::endl;
        return;
    }
    std::cout << maxLineChanges - 1 << std::endl;
}

int main() {
    std::ios::sync_with_stdio(0);
    std::cin.tie(0);

    int numStations, numConnections, numLines;
    std::cin >> numStations >> numConnections >> numLines;

    Information info(numStations, numConnections, numLines);
    return 0;
}
