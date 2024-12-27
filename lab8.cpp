#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <queue>
#include <stack>
#include <algorithm>
#include <unordered_map>
#include <chrono>

using namespace std;
using namespace chrono;

struct Edge {
    int src;
    int dest1;
    int dest2;
    double weight1;
    double weight2;
};

vector<Edge> parseGraph(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Error: Cannot open file: " << filePath << endl;
        exit(EXIT_FAILURE);
    }

    vector<Edge> edges;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        istringstream stream(line);
        Edge edge;
        char delimiter;

        stream >> edge.src >> delimiter
            >> edge.dest1 >> delimiter
            >> edge.weight1 >> delimiter
            >> edge.dest2 >> delimiter
            >> edge.weight2;

        edges.push_back(edge);
    }

    return edges;
}

vector<vector<pair<int, double>>> buildGraph(int numNodes, const vector<Edge>& edges) {
    vector<vector<pair<int, double>>> adjList(numNodes);
    for (const auto& edge : edges) {
        if (edge.dest1 != -1) {
            adjList[edge.src - 1].emplace_back(edge.dest1 - 1, edge.weight1);
        }
        if (edge.dest2 != -1) {
            adjList[edge.src - 1].emplace_back(edge.dest2 - 1, edge.weight2);
        }
    }
    return adjList;
}

void makeUndirected(vector<vector<pair<int, double>>>& graph) {
    for (int node = 0; node < graph.size(); ++node) {
        for (const auto& neighbor : graph[node]) {
            int targetNode = neighbor.first;
            double weight = neighbor.second;

            auto it = find_if(graph[targetNode].begin(), graph[targetNode].end(),
                [node](const pair<int, double>& p) { return p.first == node; });

            if (it == graph[targetNode].end()) {
                graph[targetNode].emplace_back(node, weight);
            }
        }
    }
}

vector<int> bfs(int numNodes, int start, int end, const vector<vector<pair<int, double>>>& graph) {
    vector<int> distance(numNodes, INT_MAX);
    vector<int> parent(numNodes, -1);
    queue<int> q;

    distance[start] = 0;
    q.push(start);

    while (!q.empty()) {
        int current = q.front();
        q.pop();

        for (const auto& neighbor : graph[current]) {
            int nextNode = neighbor.first;

            if (distance[current] + 1 < distance[nextNode]) {
                distance[nextNode] = distance[current] + 1;
                parent[nextNode] = current;
                q.push(nextNode);
            }
        }
    }

    if (distance[end] == INT_MAX) return {}; // Path not found

    vector<int> path;
    for (int v = end; v != -1; v = parent[v]) {
        path.push_back(v);
    }
    reverse(path.begin(), path.end());

    return path;
}

vector<int> dfs(int start, int end, const vector<vector<pair<int, double>>>& graph) {
    stack<int> s;
    vector<bool> visited(graph.size(), false);
    vector<int> parent(graph.size(), -1);

    s.push(start);
    visited[start] = true;

    while (!s.empty()) {
        int current = s.top();
        s.pop();

        if (current == end) break;

        for (const auto& neighbor : graph[current]) {
            int nextNode = neighbor.first;
            if (!visited[nextNode]) {
                visited[nextNode] = true;
                parent[nextNode] = current;
                s.push(nextNode);
            }
        }
    }

    if (!visited[end]) return {}; // Path not found

    vector<int> path;
    for (int v = end; v != -1; v = parent[v]) {
        path.push_back(v);
    }
    reverse(path.begin(), path.end());

    return path;
}

vector<int> dijkstra(int numNodes, int start, int end, const vector<vector<pair<int, double>>>& graph) {
    vector<double> distance(numNodes, INT_MAX);
    vector<int> parent(numNodes, -1);
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<>> pq;

    distance[start] = 0;
    pq.emplace(0, start);

    while (!pq.empty()) {
        auto current = pq.top();
        pq.pop();

        int currentNode = current.second;
        double currentDist = current.first;

        if (currentDist > distance[currentNode]) continue;

        for (const auto& neighbor : graph[currentNode]) {
            int nextNode = neighbor.first;
            double edgeWeight = neighbor.second;

            if (currentDist + edgeWeight < distance[nextNode]) {
                distance[nextNode] = currentDist + edgeWeight;
                parent[nextNode] = currentNode;
                pq.emplace(distance[nextNode], nextNode);
            }
        }
    }

    if (distance[end] == INT_MAX) return {}; // Path not found

    vector<int> path;
    for (int v = end; v != -1; v = parent[v]) {
        path.push_back(v);
    }
    reverse(path.begin(), path.end());

    return path;
}

int main() {
    string filePath = "new_graph_output.txt";
    vector<Edge> edges = parseGraph(filePath);
    int numNodes = 350881;

    auto graph = buildGraph(numNodes, edges);
    makeUndirected(graph);

    int startNode = 19379, endNode = 63065;
    cout << "Graph processing completed.\n";

    // BFS
    auto startTime = high_resolution_clock::now();
    vector<int> bfsPath = bfs(numNodes, startNode, endNode, graph);
    auto endTime = high_resolution_clock::now();
    cout << "BFS Time: " << duration_cast<milliseconds>(endTime - startTime).count() << " ms\n";

    if (bfsPath.empty()) {
        cout << "BFS: No path found.\n";
    }
    else {
        cout << "BFS Path: ";
        for (int node : bfsPath) cout << node << " ";
        cout << endl;
    }

    /*
    // DFS
    startTime = high_resolution_clock::now();
    vector<int> dfsPath = dfs(startNode, endNode, graph);
    endTime = high_resolution_clock::now();
    cout << "DFS Time: " << duration_cast<milliseconds>(endTime - startTime).count() << " ms\n";

    if (dfsPath.empty()) {
        cout << "DFS: No path found.\n";
    }
    else {
        cout << "DFS Path: ";
        for (int node : dfsPath) cout << node << " ";
        cout << endl;
    }
    */

    // Dijkstra
    startTime = high_resolution_clock::now();
    vector<int> dijkstraPath = dijkstra(numNodes, startNode, endNode, graph);
    endTime = high_resolution_clock::now();
    cout << "Dijkstra Time: " << duration_cast<milliseconds>(endTime - startTime).count() << " ms\n";

    if (dijkstraPath.empty()) {
        cout << "Dijkstra: No path found.\n";
    }
    else {
        cout << "Dijkstra Path: ";
        for (int node : dijkstraPath) cout << node << " ";
        cout << endl;
    }

    return 0;
}
