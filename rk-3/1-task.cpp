/*
 * Посчитать кол-во компонент связности в неориентированном графе.
 *
 * Формат ввода
 * Первая строка содержит число N – количество вершин.
 * Вторая строка содержит число M - количество ребер.
 * Каждая следующая строка содержит ребро (откуда, куда).
 *
 * Формат вывода
 * Целое число - кол-во компонент связности в графе.
 */


#include <iostream>
#include <sstream>
#include <stack>
#include <vector>
#include <cassert>
#include <cstdint>

// #define DEBUG


using vertex_t = std::uint64_t;

struct Edge {
    vertex_t from;
    vertex_t to;
    std::size_t weight;
};

bool operator < (const Edge& lhs, const Edge& rhs) {
    return lhs.weight < rhs.weight;
}

struct IGraph {
    virtual ~IGraph() {}

    virtual void AddEdge(vertex_t from, vertex_t to, std::size_t weight) = 0;

    [[nodiscard]] virtual std::size_t VerticesCount() const  = 0;

    [[nodiscard]] virtual std::vector<Edge> GetNextEdges(vertex_t vertex) const = 0;
    [[nodiscard]] virtual std::vector<Edge> GetPrevEdges(vertex_t vertex) const = 0; 
};

class ListGraph: public IGraph {
 public:
    explicit ListGraph(std::size_t size): adjacency_lists_(size) {
    }

    explicit ListGraph(const IGraph& graph): adjacency_lists_(graph.VerticesCount()) {
        for (vertex_t from = 0; from < graph.VerticesCount(); ++from) {
            adjacency_lists_[from] = graph.GetNextEdges(from);
        }
    }

    void AddEdge(vertex_t from, vertex_t to, std::size_t weight) override {
        assert(from < VerticesCount());
        assert(to < VerticesCount());

        adjacency_lists_[from].push_back(Edge{from, to, weight});
    }

    [[nodiscard]] std::size_t VerticesCount() const override {
        return adjacency_lists_.size();
    }

    [[nodiscard]] std::vector<Edge> GetNextEdges(vertex_t vertex) const override {
        assert(vertex < VerticesCount());
        return adjacency_lists_[vertex];
    }

    [[nodiscard]] std::vector<Edge> GetPrevEdges(vertex_t vertex) const override {
        assert(vertex < VerticesCount());

        std::vector<Edge> prev_vertices;
        for (vertex_t from = 0; from < VerticesCount(); ++from) {
            for (const auto& edge: adjacency_lists_[from]) {
                if (edge.to == vertex) {
                    prev_vertices.push_back(edge);
                }
            }
        }

        return prev_vertices;
    }

 private:
    std::vector<std::vector<Edge>> adjacency_lists_;
};


void DFS(const IGraph& graph, vertex_t start, std::vector<bool>& visited) {
    std::stack<vertex_t> stack;
    stack.push(start);

    while (!stack.empty()) {
        auto vertex = stack.top();
        stack.pop();

        visited[vertex] = true;
        for (const auto edge: graph.GetNextEdges(vertex)) {
            if (!visited[edge.to]) {
                stack.push(edge.to);
            }
        }
    }
}

std::size_t CountConnectivityComponents(const IGraph& graph) {
    std::vector<bool> visited(graph.VerticesCount(), false);

    std::size_t component_count = 0;
    for (std::size_t i = 0; i < graph.VerticesCount(); ++i) {
        if (!visited[i]) {
            ++component_count;
            DFS(graph, i, visited);
        }
    }

    return component_count;
}


void Run(std::istream& input, std::ostream& output) {
    std::size_t vertex_count, edge_count;
    input >> vertex_count >> edge_count;

    vertex_t from, to;
    ListGraph graph(vertex_count);
    for (std::size_t i = 0; i < edge_count; ++i) {
        input >> from >> to;
        graph.AddEdge(from, to, 0);
        graph.AddEdge(to, from, 0);
    }

    auto connectivity_components = CountConnectivityComponents(graph);
    output << connectivity_components << std::endl;
}

#ifdef DEBUG

void TestRun() {
    {
        std::stringstream input, output;

        input << "3\n";
        input << "2\n";
        input << "0 1\n";
        input << "0 2\n";

        Run(input, output);
        const std::string EXPECTED = "1\n";
        if (output.str() != EXPECTED) {
            std::cerr << "EXPECTED:\n" << EXPECTED << std::endl;
            std::cerr << "\nOBTAINED:\n" << output.str() << std::endl;
            throw;
        }
    }
    {
        std::stringstream input, output;

        input << "4\n";
        input << "2\n";
        input << "0 1\n";
        input << "0 2\n";

        Run(input, output);
        const std::string EXPECTED = "2\n";
        if (output.str() != EXPECTED) {
            std::cerr << "EXPECTED:\n" << EXPECTED << std::endl;
            std::cerr << "\nOBTAINED:\n" << output.str() << std::endl;
            throw;
        }
    }
}

#endif  // DEBUG


int main() {
    #ifdef DEBUG
        TestRun();
    #else
        Run(std::cin, std::cout);
    #endif  // DEBUG
}

