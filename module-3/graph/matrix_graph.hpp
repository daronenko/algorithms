#pragma once

#include "base.hpp"

#include <cstdint>
#include <vector>


namespace graph {

class MatrixGraph: public IGraph {
 public:
    explicit MatrixGraph(std::size_t size);

    explicit MatrixGraph(const IGraph& graph);

    void AddEdge(std::uint64_t from, std::uint64_t to) override;

    [[nodiscard]] std::size_t VerticesCount() const override;

    [[nodiscard]] std::vector<std::uint64_t> GetNextVertices(std::uint64_t vertex) const override;

    [[nodiscard]] std::vector<std::uint64_t> GetPrevVertices(std::uint64_t vertex) const override;

 private:
    std::vector<std::vector<bool>> adjacency_matrix_;
};

}  // namespace graph
