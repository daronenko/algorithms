/* 
 * Дан неориентированный связный граф. Требуется найти вес минимального остовного
 * дерева в этом графе с помощью алгоритма Прима.
 *
 * Формат ввода
 * Первая строка содержит два натуральных числа n и m — количество вершин и ребер
 * графа соответственно (1 ≤ n ≤ 20000, 0 ≤ m ≤ 100000).
 * Следующие m строк содержат описание ребер по одному на строке.
 * Ребро номер i описывается тремя натуральными числами bi, ei и wi — номера концов
 * ребра и его вес соответственно (1 ≤ bi, ei ≤ n, 0 ≤ wi ≤ 100000).
 *
 * Формат вывода
 * Выведите единственное целое число - вес минимального остовного дерева. 
 */


#include <limits>
#include <iostream>
#include <sstream>
#include <cassert>
#include <cstdint>
#include <utility>
#include <functional>
#include <numeric>
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

    std::size_t GetWeightMST() {
        std::vector<std::size_t> min_e(VerticesCount(), std::numeric_limits<std::size_t>::max());
        std::vector<std::size_t> parent(VerticesCount(), std::numeric_limits<std::size_t>::max());
        std::vector<bool> visited(VerticesCount(), false);

        std::set<std::pair<std::size_t, vertex_t>> queue;

        vertex_t start = 0;
        min_e[start] = 0;
        queue.insert({0, start});

        while (!queue.empty()) {
            auto prev_edge = *queue.begin();
            queue.erase(queue.begin());

            auto curr_vertex = prev_edge.second;
            if (visited[curr_vertex]) {
                continue;
            }

            visited[curr_vertex] = true;
            for (const auto& next_edge: GetNextEdges(curr_vertex)) {
                if (!visited[next_edge.to] && min_e[next_edge.to] > next_edge.weight) {
                    if (min_e[curr_vertex] != std::numeric_limits<std::size_t>::max()) {
                        queue.erase({min_e[curr_vertex], curr_vertex});
                    }

                    min_e[next_edge.to] = next_edge.weight;
                    queue.insert({min_e[next_edge.to], next_edge.to});
                    parent[next_edge.to] = curr_vertex;
                }
            }
        }

        std::size_t mst_weight = std::accumulate(
            std::begin(min_e),
            std::end(min_e),
            0,
            std::plus<std::size_t>()
        );

        return mst_weight;
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
        graph.AddEdge(from - 1, to - 1, weight);
        graph.AddEdge(to - 1, from - 1, weight);
    }

    auto distance = graph.GetWeightMST();
    output << distance << std::endl;
}

#ifdef DEBUG

void TestRun() {
    {
        std::stringstream input, output;

        input << "9 15\n";
        input << "1 2 9\n";
        input << "1 3 2\n";
        input << "1 4 6\n";
        input << "3 5 1\n";
        input << "4 5 1\n";
        input << "5 8 6\n";
        input << "2 4 2\n";
        input << "4 6 9\n";
        input << "6 8 5\n";
        input << "2 7 4\n";
        input << "4 7 7\n";
        input << "6 7 1\n";
        input << "6 9 1\n";
        input << "7 9 5\n";

        Run(input, output);
        const std::string EXPECTED = "17\n";
        if (output.str() != EXPECTED) {
            std::cerr << "EXPECTED:\n" << EXPECTED << std::endl;
            std::cerr << "\nOBTAINED:\n" << output.str() << std::endl;
            throw;
        }
    }
    {
        std::stringstream input, output;

        input << "4 4\n";
        input << "1 2 1\n";
        input << "2 3 2\n";
        input << "3 4 5\n";
        input << "4 1 4\n";

        Run(input, output);
        const std::string EXPECTED = "7\n";
        if (output.str() != EXPECTED) {
            std::cerr << "EXPECTED:\n" << EXPECTED << std::endl;
            std::cerr << "\nOBTAINED:\n" << output.str() << std::endl;
            throw;
        }
    }
    {
        std::stringstream input, output;

        input << "5 10\n";
        input << "4 3 3046\n";
        input << "4 5 90110\n";
        input << "5 1 57786\n";
        input << "3 2 28280\n";
        input << "4 3 18010\n";
        input << "4 5 61367\n";
        input << "4 1 18811\n";
        input << "4 2 69898\n";
        input << "3 5 72518\n";
        input << "3 1 85838\n";

        Run(input, output);
        const std::string EXPECTED = "107923\n";
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


