/* 
 * Дан неориентированный связный граф. Требуется найти вес минимального остовного
 * дерева в этом графе с помощью алгоритма Крускала.
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


#include <iostream>
#include <sstream>
#include <cassert>
#include <cstdint>
#include <utility>
#include <stack>
#include <vector>
#include <algorithm>
#include <map>

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

class ArcGraph: public IGraph {
    friend std::size_t GetWeightMST(ArcGraph& graph);

 public:
    explicit ArcGraph(std::size_t size): vertices_count_(size) {
    }

    explicit ArcGraph(const IGraph& graph) {
        for (std::size_t from = 0; from < graph.VerticesCount(); ++from) {
            for (const auto& edge: graph.GetNextEdges(from)) {
                edges_.push_back(edge);
            }
        }
    }

    void AddEdge(vertex_t from, vertex_t to, std::size_t weight) override {
        assert(from < VerticesCount());
        assert(to < VerticesCount());
        edges_.push_back({from, to, weight});
    }

    [[nodiscard]] std::size_t VerticesCount() const override {
        return vertices_count_;
    }

    [[nodiscard]] std::vector<Edge> GetNextEdges(vertex_t vertex) const override {
        assert(vertex < VerticesCount());

        std::vector<Edge> next_vertices;
        for (const auto& edge: edges_) {
            if (edge.from == vertex) {
                next_vertices.push_back(edge);
            }
        }

        return next_vertices;
    }

    [[nodiscard]] std::vector<Edge> GetPrevEdges(vertex_t vertex) const override {
        assert(vertex < VerticesCount());

        std::vector<Edge> prev_vertices;
        for (const auto& edge: edges_) {
            if (edge.to == vertex) {
                prev_vertices.push_back(edge);
            }
        }

        return prev_vertices;
    }

 private:
    std::size_t vertices_count_;
    std::vector<Edge> edges_;
};


class DSU {
 public:
    explicit DSU(std::size_t size): parent_(size), rank_(size, 1) {
        for (vertex_t i = 0; i < size; ++i) {
            parent_[i] = i;
        }
    }

    vertex_t FindSet(vertex_t u) {
        std::stack<vertex_t> stack;
        stack.push(u);

        while (parent_[u] != u) {
            stack.push(parent_[u]);
            u = parent_[u];
        }

        vertex_t& root = u;

        while (!stack.empty()) {
            parent_[stack.top()] = root;
            stack.pop();
        }

        return root;
    }

    void UnionSet(vertex_t u, vertex_t v) {
        u = FindSet(u);
        v = FindSet(v);

        if (u != v) {
            if (rank_[u] < rank_[v]) {
                std::swap(u, v);
            }

            parent_[v] = u;
            rank_[u] += rank_[v];
        }
    }

 private:
    std::vector<vertex_t> parent_;
    std::vector<std::size_t> rank_;

    friend std::ostream& operator << (std::ostream& out, DSU& dsu);
};

std::ostream& operator<<(std::ostream& out, DSU& dsu) {
    std::map<size_t, std::vector<size_t>> sets;

    for (std::size_t i = 0; i < dsu.parent_.size(); ++i) {
        sets[dsu.FindSet(i)].push_back(i);
    }

    for (auto& kv: sets) {
        out << kv.first << " [rank = " << dsu.rank_[kv.first] << "]: ";
        for (std::size_t i = 0; i < kv.second.size(); ++i) {
            out << kv.second[i];
            if (i != kv.second.size() - 1) {
                out << ", ";
            }
        }
        out << std::endl;
    }

    return out;
}


std::size_t GetWeightMST(ArcGraph& graph) {
    std::sort(graph.edges_.begin(), graph.edges_.end());

    std::size_t mst_weight = 0;

    DSU dsu(graph.VerticesCount());
    for (const auto& edge: graph.edges_) {
        auto first_set = dsu.FindSet(edge.from);
        auto second_set = dsu.FindSet(edge.to);
        if (first_set != second_set) {
            dsu.UnionSet(first_set, second_set);
            mst_weight += edge.weight;
        }
    }

    return mst_weight;
}


void Run(std::istream& input, std::ostream& output) {
    std::size_t vertex_count, edges_count;
    input >> vertex_count >> edges_count;

    vertex_t from, to;
    std::size_t weight;
    ArcGraph graph(vertex_count);
    for (std::size_t i = 0; i < edges_count; ++i) {
        input >> from >> to >> weight;
        graph.AddEdge(from - 1, to - 1, weight);
        graph.AddEdge(to - 1, from - 1, weight);
    }

    auto distance = GetWeightMST(graph);
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


