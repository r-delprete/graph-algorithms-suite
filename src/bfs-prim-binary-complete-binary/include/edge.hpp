#ifndef EDGE_HPP
#define EDGE_HPP

#include "./node.hpp"

class Edge {
private:
  Node* src;
  Node* dest;
  int weight;

public:
  Edge(Node* src, Node* dest, int weight) : src(src), dest(dest), weight(weight) {}

  Node* get_source() const { return src; }
  Node* get_destination() const { return dest; }
  int get_weight() const { return weight; }

  void set_source(Node* src) { this->src = src; }
  void set_destination(Node* dest) { this->dest = dest; }
  void set_weight(int weight) { this->weight = weight; }

  void print(std::ostream& out = std::cout) {
    out << "(" << src->get_data() << ") -> (" << dest->get_data() << ") - weight: " << weight << std::endl;
  }
};

using shared_edge_ptr = std::shared_ptr<Edge>;
using unique_edge_ptr = std::unique_ptr<Edge>;

#endif  // EDGE_HPP