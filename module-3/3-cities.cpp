/* 
 * Требуется отыскать самый короткий маршрут между городами. Из города может выходить
 * дорога, которая возвращается в этот же город.
 *
 * Требуемое время работы O((N + M)log N), где N – количество городов, M – известных
 * дорог между ними.
 *
 * Формат ввода
 * Первая строка содержит число N – количество городов, 3 ≤ N ≤ 10000.
 * Вторая строка содержит число M - количество дорог, 1 ≤ M ≤ 250000.
 * Каждая следующая строка содержит три числа u, v, w – описание дороги: откуда, куда,
 * время в пути. 0 ≤ u < N, 0 ≤ v < N, 1 ≤ w ≤ 10000. Между любыми двумя городами может
 * быть больше одной дороги.
 * Последняя строка содержит два числа: s и t – маршрут (откуда и куда нужно доехать).
 * 0 ≤ s < N, 0 ≤ t < N.
 *
 * Формат вывода
 * Вывести длину самого короткого маршрута.
 */


#include <iostream>
#include <limits>
#include <sstream>
#include <vector>
#include <cassert>
#include <cstdint>
#include <utility>
#include <set>

// #define DEBUG


using vertex_t = std::uint64_t;


struct Edge {
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

    std::size_t GetDistance(vertex_t from, vertex_t to) {
        std::vector<std::size_t> distances(VerticesCount(), std::numeric_limits<std::size_t>::max());
        std::set<std::pair<std::size_t, vertex_t>> queue;

        distances[from] = 0;
        queue.insert({0, from});

        while (!queue.empty()) {
            auto prev_edge = *queue.begin();
            queue.erase(queue.begin());

            auto curr_vertex = prev_edge.second;
            for (const auto& next_edge: GetNextEdges(curr_vertex)) {
                if (distances[curr_vertex] + next_edge.weight < distances[next_edge.to]) {
                    if (distances[curr_vertex] != std::numeric_limits<std::size_t>::max()) {
                        queue.erase({distances[curr_vertex], curr_vertex});
                    }

                    distances[next_edge.to] = distances[curr_vertex] + next_edge.weight;
                    queue.insert({next_edge.weight, next_edge.to});
                }
            }
        }

        assert(distances[to] != std::numeric_limits<std::size_t>::max());
        return distances[to];
    }
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

        adjacency_lists_[from].push_back(Edge{to, weight});
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
                    prev_vertices.push_back(Edge{from, edge.weight});
                }
            }
        }

        return prev_vertices;
    }

 private:
    std::vector<std::vector<Edge>> adjacency_lists_;
};


void Run(std::istream& input, std::ostream& output) {
    std::size_t vertex_count, edges_count;
    input >> vertex_count >> edges_count;

    vertex_t from, to;
    std::size_t weight;
    ListGraph graph(vertex_count);
    for (std::size_t i = 0; i < edges_count; ++i) {
        input >> from >> to >> weight;
        graph.AddEdge(from, to, weight);
        graph.AddEdge(to, from, weight);
    }

    input >> from >> to;
    auto distance = graph.GetDistance(from, to);
    output << distance << std::endl;
}

#ifdef DEBUG

void TestRun() {
    {
        std::stringstream input, output;

        input << "6\n";
        input << "9\n";
        input << "0 3 1\n";
        input << "0 4 2\n";
        input << "1 2 7\n";
        input << "1 3 2\n";
        input << "1 4 3\n";
        input << "1 5 3\n";
        input << "2 5 3\n";
        input << "3 4 4\n";
        input << "3 5 6\n";
        input << "0 2\n";

        Run(input, output);
        assert(output.str() == "9\n");
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

