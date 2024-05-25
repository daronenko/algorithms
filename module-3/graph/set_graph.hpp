#pragma once

#include "base.hpp"

#include <cstdint>
#include <unordered_set>


namespace graph {

class SetGraph: public IGraph {
 public:
    explicit SetGraph(std::size_t size);

    explicit SetGraph(const IGraph& graph);

    void AddEdge(std::uint64_t from, std::uint64_t to) override;

    [[nodiscard]] std::size_t VerticesCount() const override;

    [[nodiscard]] std::vector<std::uint64_t> GetNextVertices(std::uint64_t vertex) const override;

    [[nodiscard]] std::vector<std::uint64_t> GetPrevVertices(std::uint64_t vertex) const override;

 private:
    std::vector<std::unordered_set<std::uint64_t>> adjacency_sets_;
};

}  // namespace graph
