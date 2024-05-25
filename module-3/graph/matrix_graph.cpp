#include "matrix_graph.hpp"

#include <cassert>
#include <cstdint>


namespace graph {

MatrixGraph::MatrixGraph(std::size_t size): adjacency_matrix_(size) {
    for (auto& row: adjacency_matrix_) {
        row.resize(size, false);
    }
}

MatrixGraph::MatrixGraph(const IGraph& graph): adjacency_matrix_(graph.VerticesCount()) {
    for (std::size_t from = 0; from < graph.VerticesCount(); ++from) {
        adjacency_matrix_[from].resize(graph.VerticesCount(), false);
        auto to_vertices = graph.GetNextVertices(from);
        for (const auto& to: to_vertices) {
            adjacency_matrix_[from][to] = true;
        }
    }
}

void MatrixGraph::AddEdge(std::uint64_t from, std::uint64_t to) {
    assert(from < VerticesCount());
    assert(to < VerticesCount());
    adjacency_matrix_[from][to] = true;
}

[[nodiscard]] std::size_t MatrixGraph::VerticesCount() const {
    return adjacency_matrix_.size();
}

[[nodiscard]] std::vector<std::uint64_t> MatrixGraph::GetNextVertices(std::uint64_t vertex) const {
    assert(vertex < VerticesCount());

    std::vector<std::uint64_t> next_vertices;
    for (std::size_t to = 0; to < VerticesCount(); ++to) {
        if (adjacency_matrix_[vertex][to]) {
            next_vertices.push_back(to);
        }
    }

    return next_vertices;
}

[[nodiscard]] std::vector<std::uint64_t> MatrixGraph::GetPrevVertices(std::uint64_t vertex) const {
    assert(vertex < VerticesCount());

    std::vector<std::uint64_t> prev_vertices;
    for (std::size_t from = 0; from < VerticesCount(); ++from) {
        if (adjacency_matrix_[from][vertex]) {
            prev_vertices.push_back(from);
        }
    }

    return prev_vertices;
}

}  // namespace graph