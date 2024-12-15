#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <limits>
#include <algorithm>

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
    // Inserts the adjacencies in the graph, in both corresponding places.
    graph.adj[u][v].insert(line);
    graph.adj[v][u].insert(line); // undirected
}

void Information::graphConstruct() {
    int u, v, line;
    for (int i = 0; i < _numConnections; i++) {
        // u and v are stations, line is the line connecting those stations
        std::cin >> u >> v >> line;
        // adds this adjacency to the graph
        addAdj(metroGraph, u, v, line);
    }
    //metroGraph.printGraph();
}

int Information::BFSCalculate(Graph &graph, int u, int v) {
    // State is an alias for a group of 3 numbers
    using State = std::tuple<int, int, int>; // (station, currentLine, lineChanges)

    // cmp is gonna be used to compare States, based on the number of lineChanges
    auto cmp = [](const State& a, const State& b) {
        return std::get<2>(a) > std::get<2>(b); // Min-heap based on lineChanges
    };

    // queue is a list of states by order of line changes (station, currentLine, lineChanges)
    std::priority_queue<State, std::vector<State>, decltype(cmp)> queue(cmp);

    // visited is an unordered map that tells if a station was visited
    std::unordered_map<int, std::unordered_map<int, int>> visited;

    // iterates through the neighbours of u
    for (auto it = graph.adj[u].begin(); it != graph.adj[u].end(); ++it) {
        // neighbour station (it->first is the key)
        int neighbour = it->first; 

        // lines connecting u to neighbour (it->second brings the value)
        const std::unordered_set<int>& lines = it->second;
        for (int line : lines) {

            // We add a State to the queue
            queue.push(std::make_tuple(neighbour, line, 0));

            // Set visited for this station through this line
            visited[u][line] = 0;
        }
    }

    while (!queue.empty()) {
        int station, currentLine, lineChanges;

        // Puts the top of the queue values into the 3 variables (station, currentLine, lineChanges)
        std::tie(station, currentLine, lineChanges) = queue.top();

        // Removes the already seen station from the queue
        queue.pop();

        // Verify if we already got to the station, and if yes, returns the lineChanges
        if (station == v) {
            return lineChanges;
        }

        for (auto it = graph.adj[station].begin(); it != graph.adj[station].end(); ++it) {
            int neighbour = it->first;
            const std::unordered_set<int>& lines = it->second;
            for (int line : lines) {
                int newLineChanges = lineChanges + (line != currentLine ? 1 : 0);
                if (!visited[neighbour].count(line) || visited[neighbour][line] > newLineChanges) {
                    visited[neighbour][line] = newLineChanges;
                    queue.push(std::make_tuple(neighbour, line, newLineChanges));
                }
            }
        }
    }

    return -1; // Return a value in all control paths
}

void Information::calculateSolution() {
    int maxLineChanges = 0;

    // Avoids double calculation by skipping what's already calculates
    int calculated [_numStations + 1][_numStations + 1];

    for (int i = 0; i <= _numStations ; i++){
        for (int j = 0; j <= _numStations; j++)
            calculated[i][j] = 0;
    }

    if (static_cast<int>(metroGraph.adj.size()) != _numStations) {
        std::cout << -1 << std::endl;
        return;
    }

    for (auto out_it = metroGraph.adj.begin(); out_it != metroGraph.adj.end(); ++out_it) {
        int start_station = out_it->first;

        for (auto in_it = metroGraph.adj.begin(); in_it != metroGraph.adj.end(); ++in_it) {
            int end_station = in_it->first;
            if ((start_station != end_station) && (calculated[start_station][end_station] == 0)) {
                int lineChanges = BFSCalculate(metroGraph, start_station, end_station);
                if (lineChanges == -1) {
                    std::cout << -1  << std::endl;
                    return;
                }
                calculated[start_station][end_station] = 1;
                calculated[end_station][start_station] = 1;
                maxLineChanges = std::max(maxLineChanges, lineChanges);
            }
        }
    }
    std::cout << maxLineChanges << std::endl;
}

void Information::printGraph() {
    for (auto it = metroGraph.adj.begin(); it != metroGraph.adj.end(); ++it) {
        int station = it->first;
        const auto &neighbours = it->second;
        std::cout << "Station " << station << ":\n";
        for (auto it2 = neighbours.begin(); it2 != neighbours.end(); ++it2) {
            int neighbour = it2->first;
            const auto &lines = it2->second;
            std::cout << "  -> " << neighbour << " via lines: ";
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
