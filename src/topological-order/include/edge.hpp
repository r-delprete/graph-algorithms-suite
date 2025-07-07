#ifndef EDGE_HPP
#define EDGE_HPP

#include "node.hpp"

class Edge {
  Node *src, *dest;
  int weight;

public:
  Edge(Node* src, Node* dest, int weight) : src(src), dest(dest), weight(weight) {}

  Node* get_source() { return src; }
  Node* get_destination() { return dest; }
  int get_weight() { return weight; }

  void set_source(Node* src) { this->src = src; }
  void set_destination(Node* dest) { this->dest = dest; }
  void set_weight(int weight) { this->weight = weight; }

  void get_info(std::ostream& out = std::cout) {
    out << "(" << src->get_data() << ") -> (" << dest->get_data() << ") - weight: " << weight << std::endl;
  }
};

#endif