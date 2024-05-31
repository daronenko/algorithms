/*
 * Рик и Морти снова бороздят просторы вселенных, но решили ограничиться
 * только теми, номера которых меньше M. Они могут телепортироваться
 * из вселенной с номером z во вселенную (z+1) mod M за a бутылок лимонада
 * или во вселенную (z^2+1) mod M за b бутылок лимонада. Рик и Морти хотят
 * добраться из вселенной с номером x во вселенную с номером y. Сколько
 * бутылок лимонада отдаст Рик за такое путешествие, если он хочет потратить
 * их как можно меньше?
 *
 * Формат ввода
 * В строке подряд даны количество бутылок a за первый тип телепортации,
 * количество бутылок b за второй тип телепортации, количество вселенных M,
 * номер стартовой вселенной x, номер конечной вселенной y (0 ≤ a, b ≤ 100,
 * 1 ≤ M ≤ 10^6, 0 ≤ x, y < M).
 *
 * Формат вывода
 * Выведите одно число — количество бутылок лимонада, которые отдаст Рик
 * за такое путешествие.
 */


#include <iostream>
#include <sstream>
#include <utility>
#include <limits>
#include <vector>
#include <set>
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

    [[nodiscard]] virtual const std::vector<Edge>& GetNextEdges(vertex_t vertex) const = 0;
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

    [[nodiscard]] const std::vector<Edge>& GetNextEdges(vertex_t vertex) const override {
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


std::size_t FindShortestPath(const IGraph& graph, vertex_t from, vertex_t to) {
    std::vector<std::size_t> distances(graph.VerticesCount(), std::numeric_limits<std::size_t>::max());
    std::set<std::pair<std::size_t, vertex_t>> queue;

    distances[from] = 0;
    queue.insert({0, from});

    while (!queue.empty()) {
        auto prev_edge = *queue.begin();
        queue.erase(queue.begin());

        auto curr_vertex = prev_edge.second;
        for (const auto& next_edge: graph.GetNextEdges(curr_vertex)) {
            if (distances[curr_vertex] + next_edge.weight < distances[next_edge.to]) {
                distances[next_edge.to] = distances[curr_vertex] + next_edge.weight;
                queue.insert({distances[next_edge.to], next_edge.to});
            }
        }
    }

    assert(distances[to] != std::numeric_limits<std::size_t>::max());
    return distances[to];
}


void Run(std::istream& input, std::ostream& output) {
    std::size_t a_weight, b_weight;
    input >> a_weight >> b_weight;

    std::size_t universe_count;
    input >> universe_count;

    vertex_t start, finish;
    input >> start >> finish;

    ListGraph graph(universe_count);
    for (vertex_t universe = 0; universe < universe_count; ++universe) {
        graph.AddEdge(universe, (universe + 1) % universe_count, a_weight);
        graph.AddEdge(universe, (universe * universe + 1) % universe_count, b_weight);
    }

    auto path_weight = FindShortestPath(graph, start, finish);
    output << path_weight << std::endl;
}

#ifdef DEBUG

void TestRun() {
    {
        std::stringstream input, output;

        input << "3 14 15 9 9\n";

        Run(input, output);
        const std::string EXPECTED = "0\n";
        if (output.str() != EXPECTED) {
            std::cerr << "EXPECTED:\n" << EXPECTED << std::endl;
            std::cerr << "\nOBTAINED:\n" << output.str() << std::endl;
            throw;
        }
    }
    {
        std::stringstream input, output;

        input << "6 1 5 2 3\n";

        Run(input, output);
        const std::string EXPECTED = "6\n";
        if (output.str() != EXPECTED) {
            std::cerr << "EXPECTED:\n" << EXPECTED << std::endl;
            std::cerr << "\nOBTAINED:\n" << output.str() << std::endl;
            throw;
        }
    }
    {
        std::stringstream input, output;

        input << "6 1 5 2 1\n";

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


