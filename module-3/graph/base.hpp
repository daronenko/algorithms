#pragma once

#include <vector>
#include <cstdint>


namespace graph {

struct IGraph {
    virtual ~IGraph() {}

    // adding an edge from `from` to `to`.
    virtual void AddEdge(std::uint64_t from, std::uint64_t to) = 0;

    [[nodiscard]] virtual std::size_t VerticesCount() const  = 0;

    [[nodiscard]] virtual std::vector<std::uint64_t> GetNextVertices(std::uint64_t vertex) const = 0;
    [[nodiscard]] virtual std::vector<std::uint64_t> GetPrevVertices(std::uint64_t vertex) const = 0;
};

}  // namespace graph

