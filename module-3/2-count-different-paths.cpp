/*
 * Дан невзвешенный неориентированный граф. В графе может быть несколько кратчайших путей
 * между какими-то вершинами. Найдите количество различных кратчай их путей между заданными
 * вершинами. Требуемая сложность O(V+E).
 *
 * Формат ввода
 * v: кол-во вершин (макс. 50000),
 * n: кол-во ребер(макс. 200000),
 * n пар реберных вершин,
 * пара вершин (u, w) для запроса.
 *
 * Формат вывода
 * Количество кратчайших путей от v к w.
 */

#include <iostream>
#include <limits>
#include <sstream>
#include <vector>
#include <queue>
#include <cassert>
#include <cstdint>

// #define DEBUG


using vertex_t = std::uint64_t;

struct IGraph {
    virtual ~IGraph() {}

    // adding an edge from `from` to `to`.
    virtual void AddEdge(vertex_t from, vertex_t to) = 0;

    [[nodiscard]] virtual std::size_t VerticesCount() const  = 0;

    [[nodiscard]] virtual std::vector<vertex_t> GetNextVertices(vertex_t vertex) const = 0;
    [[nodiscard]] virtual std::vector<vertex_t> GetPrevVertices(vertex_t vertex) const = 0;

    std::size_t PathsCount(vertex_t from, vertex_t to) const {
        std::vector<std::size_t> min_path_distance(VerticesCount(), std::numeric_limits<std::size_t>::max());
        std::vector<std::size_t> min_path_count(VerticesCount(), 0);

        min_path_distance[from] = 0;
        min_path_count[from] = 1;

        std::queue<vertex_t> queue;
        queue.push(from);
        while (!queue.empty()) {
            auto vertex = queue.front();
            queue.pop();

            for (const auto& next_vertex: GetNextVertices(vertex)) {
                if (min_path_distance[vertex] + 1 < min_path_distance[next_vertex]) {
                    min_path_distance[next_vertex] = min_path_distance[vertex] + 1;
                    min_path_count[next_vertex] = min_path_count[vertex];
                    queue.push(next_vertex);
                } else if (min_path_distance[vertex] + 1 == min_path_distance[next_vertex]) {
                    min_path_count[next_vertex] += min_path_count[vertex];
                }
            }
        }

        return min_path_count[to];
    }
};


class ListGraph: public IGraph {
 public:
    explicit ListGraph(std::size_t size): adjacency_lists_(size) {
    }

    explicit ListGraph(const IGraph& graph): adjacency_lists_(graph.VerticesCount()) {
        for (vertex_t from = 0; from < graph.VerticesCount(); ++from) {
            adjacency_lists_[from] = graph.GetNextVertices(from);
        }
    }

    void AddEdge(vertex_t from, vertex_t to) override {
        assert(from < VerticesCount());
        assert(to < VerticesCount());
        adjacency_lists_[from].push_back(to);
    }

    [[nodiscard]] std::size_t VerticesCount() const override {
        return adjacency_lists_.size();
    }

    [[nodiscard]] std::vector<vertex_t> GetNextVertices(vertex_t vertex) const override {
        assert(vertex < VerticesCount());
        return adjacency_lists_[vertex];
    }

    [[nodiscard]] std::vector<vertex_t> GetPrevVertices(vertex_t vertex) const override {
        assert(vertex < VerticesCount());

        std::vector<vertex_t> prev_vertices;
        for (vertex_t from = 0; from < VerticesCount(); ++from) {
            for (auto to: adjacency_lists_[from]) {
                if (to == vertex) {
                    prev_vertices.push_back(from);
                }
            }
        }

        return prev_vertices;
    }

 private:
    std::vector<std::vector<vertex_t>> adjacency_lists_;
};


void Run(std::istream& input, std::ostream& output) {
    std::size_t vertex_count, edges_count;
    input >> vertex_count >> edges_count;

    vertex_t from, to;
    ListGraph graph(vertex_count);
    for (std::size_t i = 0; i < edges_count; ++i) {
        input >> from >> to;
        graph.AddEdge(from, to);
        graph.AddEdge(to, from);
    }

    input >> from >> to;
    output << graph.PathsCount(from, to) << std::endl;
}

#ifdef DEBUG

void TestRun() {
    {
        std::stringstream input, output;

        input << "4\n";
        input << "5\n";
        input << "0 1\n";
        input << "0 2\n";
        input << "1 2\n";
        input << "1 3\n";
        input << "2 3\n";
        input << "0 3\n";

        Run(input, output);
        assert(output.str() == "2\n");
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
