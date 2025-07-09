#ifndef EDGE_HPP
#define EDGE_HPP

#include <iostream>
#include <string>

#include "node.hpp"

enum EdgeType { tree, backward, forward, cross };

class Edge {
private:
  shared_node_ptr src;
  shared_node_ptr dest;
  int weight;
  EdgeType type;

  std::string print_type() {
    switch (type) {
      case EdgeType::tree:
        return "tree edge";
        break;
      case EdgeType::backward:
        return "backward edge";
        break;
      case EdgeType::cross:
        return "cross edge";
        break;
      case EdgeType::forward:
        return "forward edge";
        break;
      default:
        return "";
        break;
    }
  }

public:
  Edge(shared_node_ptr& src, shared_node_ptr& dest, int weight)
      : src(src), dest(dest), weight(weight), type(EdgeType::tree) {}

  shared_node_ptr get_source() const { return src; }
  shared_node_ptr get_destination() const { return dest; }
  int get_weight() { return weight; }
  EdgeType get_type() { return type; }
  std::string type_string() { return print_type(); }

  void set_source(const shared_node_ptr& src) { this->src = src; }
  void set_destination(const shared_node_ptr& dest) { this->dest = dest; }
  void set_weight(int weight) { this->weight = weight; }
  void set_type(EdgeType type) { this->type = type; }

  void print(std::ostream& out = std::cout) {
    out << "(src: " << src->get_data() << " -> dest: " << dest->get_data() << ") - weight: " << weight
        << " - type: " << print_type() << std::endl;
  }
};

using shared_edge_ptr = std::shared_ptr<Edge>;

#endif  // EDGE_HPP