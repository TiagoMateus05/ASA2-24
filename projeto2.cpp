#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <unordered_map>
#include <unordered_set>
#include <fstream>

using namespace std;

struct Graph {
    unordered_map<int, unordered_map<int, unordered_set<int>>> adj;
};

Graph metroGraph;

class Information {
    private:
        int _numEstacoes;
        int _numLigacoes;
        int _numLinhas;
    public:
        Information(int numEstacoes, int numLigacoes, int numLinhas, ifstream& inputFile);
        void addAdj(Graph &graph, int u, int v, int line);
        void graphConstruct(ifstream& inputFile);
        int BFSCalculate(Graph &graph, int u, int v);
        void calculateSolution();
        void printGraph();
};


Information::Information(int numEstacoes, int numLigacoes, int numLinhas, ifstream& inputFile) {
    _numEstacoes = numEstacoes;
    _numLigacoes = numLigacoes;
    _numLinhas = numLinhas;
    graphConstruct(inputFile);
    calculateSolution();
}

void Information::addAdj(Graph &graph, int u, int v, int line) {
    graph.adj[u][v].insert(line);
    graph.adj[v][u].insert(line); //undirected
}

void Information::graphConstruct(ifstream& inputFile) {
    int u, v, line;
    while (inputFile >> u >> v >> line) {
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
    if (metroGraph.adj.size() != _numEstacoes) {
        cout << -1 << endl;
        return;
    }

    int ML = 0;
    int check = 0;
    for (auto out_it = metroGraph.adj.begin(); out_it != metroGraph.adj.end(); ++out_it) {
        int start_station = out_it ->first;
        for (auto inner_it = metroGraph.adj.begin(); inner_it != metroGraph.adj.end(); ++inner_it) {
            int end_station = inner_it->first;
            if (start_station != end_station) {
                check = BFSCalculate(metroGraph, start_station, end_station);
                if (check == - 1) {
                    cout << check << endl;
                    return;
                }
                ML = max(ML, check);
            }
        }
    }
    cout << ML << endl;
}

void Information::printGraph() {
    for (auto it = metroGraph.adj.begin(); it != metroGraph.adj.end(); ++it) {
        int station = it->first;
        const std::unordered_map<int, std::unordered_set<int>>& neighbors = it->second;
        std::cout << "Station " << station << ":\n";
        for (auto it2 = neighbors.begin(); it2 != neighbors.end(); ++it2) {
            int neighbor = it2->first;
            const std::unordered_set<int>& lines = it2->second;
            std::cout << "  -> " << neighbor << " via lines: ";
            for (int line : lines) {
                std::cout << line << " ";
            }
            std::cout << "\n";
        }
    }
}

int main(int argc, char * argv[]) {
    std::ios::sync_with_stdio(0);
    std::cin.tie(0);

    if (argc != 2) {
        cerr << "Error opening file: " << argv[1] << endl;
    }

    std::ifstream inputFile(argv[1]);
        if (!inputFile) {
        std::cerr << "Error opening file: " << argv[1] << std::endl;
        return 1;
    }

    int numEstacoes, numLigacoes, numLinhas;
    inputFile >> numEstacoes >> numLigacoes >> numLinhas;

    Information info(numEstacoes, numLigacoes, numLinhas, inputFile);
    return 0;
}