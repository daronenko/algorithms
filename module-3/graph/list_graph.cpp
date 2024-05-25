#include "list_graph.hpp"

#include <cassert>
#include <cstdint>


namespace graph {

ListGraph::ListGraph(std::size_t size): adjacency_lists_(size) {
}

ListGraph::ListGraph(const IGraph& graph) {
    for (std::size_t from = 0; from < graph.VerticesCount(); ++from) {
        adjacency_lists_[from] = graph.GetNextVertices(from);
    }
}

void ListGraph::AddEdge(std::uint64_t from, std::uint64_t to) {
    assert(from < VerticesCount());
    assert(to < VerticesCount());
    adjacency_lists_[from].push_back(to);
}

[[nodiscard]] std::size_t ListGraph::VerticesCount() const {
    return adjacency_lists_.size();
}

[[nodiscard]] std::vector<std::uint64_t> ListGraph::GetNextVertices(std::uint64_t vertex) const {
    assert(vertex < VerticesCount());
    return adjacency_lists_[vertex];
}

[[nodiscard]] std::vector<std::uint64_t> ListGraph::GetPrevVertices(std::uint64_t vertex) const {
    assert(vertex < VerticesCount());

    std::vector<std::uint64_t> prev_vertices;
    for (std::size_t from = 0; from < VerticesCount(); ++from) {
        for (std::size_t to: adjacency_lists_[from]) {
            if (to == vertex) {
                prev_vertices.push_back(from);
            }
        }
    }

    return prev_vertices;
}

}  // namespace graph
