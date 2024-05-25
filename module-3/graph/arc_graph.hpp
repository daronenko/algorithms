#pragma once

#include "base.hpp"

#include <cstdint>
#include <utility>


namespace graph {

class ArcGraph: public IGraph {
    using Edge = std::pair<std::uint64_t, std::uint64_t>;

 public:
    explicit ArcGraph(std::size_t size);

    explicit ArcGraph(const IGraph& graph);

    void AddEdge(std::uint64_t from, std::uint64_t to) override;

    [[nodiscard]] std::size_t VerticesCount() const override;

    [[nodiscard]] std::vector<std::uint64_t> GetNextVertices(std::uint64_t vertex) const override;

    [[nodiscard]] std::vector<std::uint64_t> GetPrevVertices(std::uint64_t vertex) const override;

 private:
    std::size_t vertices_count_;
    std::vector<Edge> edges_;
};

}  // namespace graph
