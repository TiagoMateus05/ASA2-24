#include <iostream>
#include <vector>
#include <unordered_set>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <set>

// Custom hash function for std::pair<int, int>
struct pair_hash {
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2> &pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

// Custom equality function for std::pair<int, int>
struct pair_equal {
    bool operator() (const std::pair<int, int> &lhs, const std::pair<int, int> &rhs) const {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }
};

struct Graphs {
    std::vector<std::unordered_set<std::pair<int, int>, pair_hash, pair_equal>> metroGraph; // represents the full metro system
    std::vector<std::unordered_set<int>> linesGraph; // represents the full metro system
    std::vector<std::unordered_set<int>> linesByStation;
    std::vector<std::unordered_set<int>> stationsByLine;
    std::vector<bool> lineContainedInAnotherLine;
};

class Information {
private:
    int numConnections;
    int numStations;
    int numLines;
    int solution;
    Graphs graphs;

public:
    Information(int numStations, int numConnections, int numLines);
    void addAdj(int u, int v, int line);
    void buildMetroGraph();
    void printMetroGraph();
    bool isolatedStationsExist();
    bool systemBFS();
    void printLinesGraph();
    void checkContainedLines();
    int resultsBFS();

    int getSolution() const;
};

Information::Information(int numStations, int numConnections, int numLines)
    : numConnections(numConnections), numStations(numStations), numLines(numLines) {
        graphs.metroGraph.resize(numStations + 1);
        graphs.linesGraph.resize(numLines + 1);
        graphs.linesByStation.resize(numStations + 1);
        graphs.stationsByLine.resize(numLines + 1);
        graphs.lineContainedInAnotherLine.resize(numLines + 1, false);
        buildMetroGraph();
        //printMetroGraph();
        if (isolatedStationsExist()){
            solution = -1;
            return;
        }
        checkContainedLines();
        if (!systemBFS()){
            solution = -1;
            return;
        }
        //printLinesGraph();
        solution = resultsBFS();
        return;

}

int Information::getSolution() const{
    return solution;
}

void Information::addAdj(int u, int v, int line) {
    graphs.metroGraph[u].insert({v,line});
    graphs.metroGraph[v].insert({u,line});
}

void Information::buildMetroGraph() {
    int u, v, line;
    for (int i = 0; i < numConnections; i++) {
        std::cin >> u >> v >> line;
        addAdj(u, v, line);
        graphs.linesByStation[u].insert(line);
        graphs.linesByStation[v].insert(line);
        graphs.stationsByLine[line].insert(u);
        graphs.stationsByLine[line].insert(v);
    }
}


void Information::printMetroGraph() {
    std::cout << "Graph representation:\n";
    for (int station = 1; station <= numStations; ++station) {
        std::cout << "Station " << station << " is connected to stations: ";
        for (std::pair<int,int> station : graphs.metroGraph[station]) {
            std::cout << station.first << " ";
        }
        std::cout << "\n";
    }
}

void Information::printLinesGraph() {
    std::cout << "Lines graph representation:\n";
    for (int line = 1; line <= numLines; ++line) {
        std::cout << "Line " << line << " connects to lines: ";
        for (const auto& station : graphs.linesGraph[line]) {
            std::cout << station << " ";
        }
        std::cout << "\n";
    }
}

bool Information::isolatedStationsExist() {
    for (int station = 1; station <= numStations; station++) {
        if (graphs.metroGraph[station].empty()) {
            return true;
        }
    }
    return false;
}

bool Information::systemBFS(){

    std::vector<int> visited(numStations + 1, 0);
    std::queue<int> q;
    q.push(1);
    visited[1] = 1;
    while (!q.empty()){
        int station = q.front();
        q.pop();
        for (std::pair<int,int> adjStation : graphs.metroGraph[station]){
            if (!visited[adjStation.first]){
                visited[adjStation.first] = 1;
                q.push(adjStation.first);
            }
            for (int line : graphs.linesByStation[station]){
                for (int adjLine : graphs.linesByStation[adjStation.first]){
                    if (line != adjLine && !graphs.lineContainedInAnotherLine[adjLine] && !graphs.lineContainedInAnotherLine[line]){
                        graphs.linesGraph[line].insert(adjLine);
                        graphs.linesGraph[adjLine].insert(line);
                    }
                }
            }
        }
    }
    for (int i = 1; i <= numStations; i++){
        if (!visited[i]){
            return false;
        }
    }
    return true;
}


int Information::resultsBFS(){
    int solution = 0;
    for (int i = 1; i <= numLines; i++){
        std::vector<int> visited(numLines + 1, 0);
        std::vector<int> distances(numLines + 1, 0);
        std::queue<int> q;
        q.push(i);
        visited[i] = 1;
        distances[i] = 0;
        int maxDistance;
        maxDistance = 0;
        while (!q.empty()){
            int line = q.front();
            q.pop();
            for (int adjLine : graphs.linesGraph[line]){
                //std::cout << "Line: "<< line << "\n";
                if (!visited[adjLine]){
                    //std::cout << "AdjLine: "<< adjLine << "\n";
                    visited[adjLine] = 1;
                    distances[adjLine] = distances[line] + 1;
                    //std::cout << "Distance: "<< distances[adjLine] << "\n" << "\n";
                    maxDistance = std::max(maxDistance, distances[adjLine]);
                    q.push(adjLine);
                }
            }
        }
        if (maxDistance > solution)
            solution = maxDistance;
        //std::cout << "MaxDistance: "<< maxDistance << "\n";
    }

    return solution;
}

void Information::checkContainedLines(){
    for (int i = 1; i <= numLines; i++){
        for (int j = 1; j <= numLines; j++){
            if (i != j){
                if (graphs.stationsByLine[i].size() < graphs.stationsByLine[j].size()){
                    bool contained = true;
                    for (int station : graphs.stationsByLine[i]){
                        if (graphs.stationsByLine[j].find(station) == graphs.stationsByLine[j].end()){
                            contained = false;
                            break;
                        }
                    }
                    if (contained){
                        graphs.lineContainedInAnotherLine[i] = true;
                        break;
                    }
                }
            }
        }
    }
}

int main() {
    std::ios::sync_with_stdio(0);
    std::cin.tie(0);

    int numStations, numConnections, numLines;
    std::cin >> numStations >> numConnections >> numLines;

    Information info(numStations, numConnections, numLines);

    std::cout << info.getSolution() << "\n";
    return 0;
}
