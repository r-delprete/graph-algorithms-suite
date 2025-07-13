#ifndef EDGE_HPP
#define EDGE_HPP

#include "./node.hpp"

class Edge {
private:
  shared_node_ptr src, dest;
  int weight;

public:
  Edge(const shared_node_ptr& src, const shared_node_ptr& dest, const int weight)
      : src(src), dest(dest), weight(weight) {}

  const shared_node_ptr& get_source() const { return src; }
  const shared_node_ptr& get_destination() const { return dest; }
  const int get_weight() const { return weight; }

  void set_source(const shared_node_ptr& src) { this->src = src; }
  void set_destination(const shared_node_ptr& dest) { this->dest = dest; }
  void set_weight(const int weight) { this->weight = weight; }

  void print(std::ostream& out = std::cout) {
    out << "(" << src->get_data() << ") -> (" << dest->get_data() << ") => weight: " << weight << std::endl;
  }
};

using shared_edge_ptr = std::shared_ptr<Edge>;
shared_edge_ptr edge_factory_shared(const shared_node_ptr& src, const shared_node_ptr& dest, const int weight) {
  return std::make_shared<Edge>(src, dest, weight);
}

#endif  // EDGE_HPP