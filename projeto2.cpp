#include <iostream>
#include <vector>
#include <unordered_set>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <set>

struct Graphs {
    std::vector<std::unordered_set<int>> metroGraph; // represents the full metro system
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
    bool isolatedStationsExist();
    bool systemBFS();
    void checkContainedLines();
    int resultsBFS();

    //Debugging
    void printLinesGraph();
    void printMetroGraph();
    void printLinesByStation();

    int getSolution() const;
};

Information::Information(int numStations, int numConnections, int numLines)
    : numConnections(numConnections), numStations(numStations), numLines(numLines) {
        if (numStations == 1){
            solution = 0;
            return;
        }
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
    graphs.metroGraph[u].insert(v);
    graphs.metroGraph[v].insert(u);
}

void Information::printLinesByStation(){
    for (int i = 1; i <= numStations; i++){
        std::cout << "Station " << i << " is in lines: ";
        for (int line : graphs.linesByStation[i]){
            std::cout << line << " ";
        }
        std::cout << "\n";
    }
}

void Information::buildMetroGraph() {
    int u, v, line;
    //printMetroGraph();
    for (int i = 0; i < numConnections; i++) {
        std::cin >> u >> v >> line;
        //std::cout << "\nU: " << u << " V: " << v << " Line: " << line << "\n";
        addAdj(u, v, line);
        graphs.linesByStation[u].insert(line);
        graphs.linesByStation[v].insert(line);
        graphs.stationsByLine[line].insert(u);
        graphs.stationsByLine[line].insert(v);
        //printMetroGraph();
    }
}


void Information::printMetroGraph() {
    std::cout << "Graph representation:\n";
    for (int station = 1; station <= numStations; ++station) {
        std::cout << "Station " << station << " is connected to stations: ";
        for (int station : graphs.metroGraph[station]) {
            std::cout << station << " ";
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

    //printLinesByStation();

    std::vector<int> visited(numStations + 1, 0);
    std::queue<int> q;
    q.push(1);
    visited[1] = 1;
    while (!q.empty()){
        int station = q.front();
        q.pop();
        for (int line1 : graphs.linesByStation[station]){
            for (int line2 : graphs.linesByStation[station]){
                if (line1 != line2 && !graphs.lineContainedInAnotherLine[line1] && !graphs.lineContainedInAnotherLine[line2]){
                    graphs.linesGraph[line1].insert(line2);
                    graphs.linesGraph[line2].insert(line1);
                }
            }
        }
        for (int adjStation : graphs.metroGraph[station]){
            if (!visited[adjStation]){
                visited[adjStation] = 1;
                q.push(adjStation);
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
                if (graphs.stationsByLine[i].size() <= graphs.stationsByLine[j].size()){
                    bool contained = true;
                    for (int station : graphs.stationsByLine[i]){
                        if (graphs.stationsByLine[j].find(station) == graphs.stationsByLine[j].end()){
                            contained = false;
                            break;
                        }
                    }
                    if (contained && !graphs.lineContainedInAnotherLine[j]){
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
