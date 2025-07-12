#ifndef EDGE_HPP
#define EDGE_HPP

#include "node.hpp"

class Edge {
  shared_node_ptr src, dest;
  int weight;

public:
  Edge(const shared_node_ptr& src, const shared_node_ptr& dest, int weight) : src(src), dest(dest), weight(weight) {}

  shared_node_ptr const get_source() const { return src; }
  shared_node_ptr const get_destination() const { return dest; }
  shared_node_ptr get_source_ref() { return src; }
  shared_node_ptr get_destination_ref() { return dest; }
  const int get_weight() const { return weight; }

  void set_source(const shared_node_ptr& src) { this->src = src; }
  void set_destination(const shared_node_ptr& dest) { this->dest = dest; }
  void set_weight(int weight) { this->weight = weight; }

  void print(std::ostream& out) {
    out << "(" << src->get_data() << ") -> (" << dest->get_data() << ") - weight: " << weight << std::endl;
  }
};

using shared_edge_ptr = std::shared_ptr<Edge>;

#endif