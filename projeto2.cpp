#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <limits>

struct Graph {
    std::unordered_map<int, std::unordered_map<int, std::unordered_set<int>>> adj;
};

Graph metroGraph;

class Information {
private:
    int _numConnections;
    int _numStations;

public:
    Information(int numEstacoes, int numLigacoes, int numLinhas);
    void addAdj(Graph &graph, int u, int v, int line);
    void graphConstruct();
    int BFSCalculate(Graph &graph, int start, int target);
    void calculateSolution();
    void printGraph();
};

Information::Information(int numEstacoes, int numLigacoes, int numLinhas) {
    _numStations = numEstacoes;
    _numConnections = numLigacoes;
    graphConstruct();
    calculateSolution();
}
void Information::addAdj(Graph &graph, int u, int v, int line) {
    graph.adj[u][v].insert(line);
    graph.adj[v][u].insert(line); // undirected
}

void Information::graphConstruct() {
    int u, v, line;
    for (int i = 0; i < _numConnections; i++) {
        std::cin >> u >> v >> line;
        addAdj(metroGraph, u, v, line);
    }
}

int Information::BFSCalculate(Graph &graph, int u, int v) {
    using State = std::tuple<int, int, int>; // (station, currentLine, lineChanges)
    auto cmp = [](const State& a, const State& b) {
        return std::get<2>(a) > std::get<2>(b); // Min-heap based on lineChanges
    };
    std::priority_queue<State, std::vector<State>, decltype(cmp)> queue(cmp);
    std::unordered_map<int, std::unordered_map<int, int>> visited;

    for (auto it = graph.adj[u].begin(); it != graph.adj[u].end(); ++it) {
        int neighbor = it->first;
        const std::unordered_set<int>& lines = it->second;
        for (int line : lines) {
            queue.push(std::make_tuple(neighbor, line, 0));
            visited[u][line] = 0;
        }
    }

    while (!queue.empty()) {
        int station, currentLine, lineChanges;
        std::tie(station, currentLine, lineChanges) = queue.top();
        queue.pop();

        if (station == v) {
            return lineChanges;
        }

        for (auto it = graph.adj[station].begin(); it != graph.adj[station].end(); ++it) {
            int neighbor = it->first;
            const std::unordered_set<int>& lines = it->second;
            for (int line : lines) {
                int newLineChanges = lineChanges + (line != currentLine ? 1 : 0);
                if (!visited[neighbor].count(line) || visited[neighbor][line] > newLineChanges) {
                    visited[neighbor][line] = newLineChanges;
                    queue.push(std::make_tuple(neighbor, line, newLineChanges));
                }
            }
        }
    }

    return -1; // Return a value in all control paths
}

void Information::calculateSolution() {
    int maxLineChanges = 0;

    if (static_cast<int>(metroGraph.adj.size()) != _numStations) {
        std::cout << -1 << std::endl;
        return;
    }

    for (auto out_it = metroGraph.adj.begin(); out_it != metroGraph.adj.end(); ++out_it) {
        int start_station = out_it->first;

        for (auto in_it = metroGraph.adj.begin(); in_it != metroGraph.adj.end(); ++in_it) {
            int end_station = in_it->first;
            if (start_station != end_station) {
                int lineChanges = BFSCalculate(metroGraph, start_station, end_station);
                if (lineChanges == -1) {
                    std::cout << -1 << std::endl;
                    return;
                }
                maxLineChanges = std::max(maxLineChanges, lineChanges);
            }
        }
    }
    std::cout << maxLineChanges << std::endl;
}

void Information::printGraph() {
    for (auto it = metroGraph.adj.begin(); it != metroGraph.adj.end(); ++it) {
        int station = it->first;
        const auto &neighbors = it->second;
        std::cout << "Station " << station << ":\n";
        for (auto it2 = neighbors.begin(); it2 != neighbors.end(); ++it2) {
            int neighbor = it2->first;
            const auto &lines = it2->second;
            std::cout << "  -> " << neighbor << " via lines: ";
            for (int line : lines) {
                std::cout << line << " ";
            }
            std::cout << "\n";
        }
    }
}

int main() {
    std::ios::sync_with_stdio(0);
    std::cin.tie(0);

    int numEstacoes, numLigacoes, numLinhas;
    std::cin >> numEstacoes >> numLigacoes >> numLinhas;

    Information info(numEstacoes, numLigacoes, numLinhas);
    return 0;
}
