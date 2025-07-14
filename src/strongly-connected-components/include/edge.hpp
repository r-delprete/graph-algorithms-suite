#ifndef EDGE_HPP
#define EDGE_HPP

#include "node.hpp"

class Edge {
  shared_node src, dest;
  int weight;

public:
  Edge(const shared_node& src, const shared_node& dest, const int weight) : src(src), dest(dest), weight(weight) {}

  const shared_node& get_source() const { return src; }
  const shared_node& get_destination() const { return dest; }
  int get_weight() const { return weight; }

  void set_source(const shared_node& src) { this->src = src; }
  void set_destination(const shared_node& dest) { this->dest = dest; }
  void set_weight(const int weight) { this->weight = weight; }

  void print(std::ostream& out = std::cout) const {
    out << "(" << src->get_data() << ") -> (" << dest->get_data() << ") => weight: " << weight << std::endl;
  }
};

using shared_edge = std::shared_ptr<Edge>;
shared_edge create_edge(const shared_node& src, const shared_node& dest, const int weight) {
  return std::make_shared<Edge>(src, dest, weight);
}

#endif