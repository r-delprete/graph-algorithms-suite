#ifndef NODE_HPP
#define NODE_HPP

#include <climits>
#include <iostream>
#include <memory>
#include <vector>

enum Color { white, gray, black };

class Node {
  int data, distance, start_visit, end_visit;
  Color color;
  std::weak_ptr<Node> predecessor;
  std::vector<std::weak_ptr<Node>> adj_list;

public:
  Node(const int data) : data(data), distance(INT_MAX), start_visit(INT_MAX), end_visit(INT_MAX), color(Color::white) {}

  const int get_data() const { return data; }
  const int get_start_visit() const { return start_visit; }
  const int get_end_visit() const { return end_visit; }
  const int get_distance() const { return distance; }
  const std::weak_ptr<Node>& get_predecessor() const { return predecessor; }
  const std::vector<std::weak_ptr<Node>>& get_adj_list() const { return adj_list; }
  const Color get_color() const { return color; }

  void set_data(const int data) { this->data = data; }
  void set_start_visit(const int start_visit) { this->start_visit = start_visit; }
  void set_end_visit(const int end_visit) { this->end_visit = end_visit; }
  void set_distance(const int distance) { this->distance = distance; }
  void set_color(const Color color) { this->color = color; }
  void set_predecessor(const std::shared_ptr<Node>& node) { this->predecessor = node; }
  void add_adjacent(const std::shared_ptr<Node>& node) { adj_list.push_back(node); }

  void print(std::ostream& out = std::cout) {
    out << "(" << data << ") => color: "
        << (color == Color::black  ? "black"
            : color == Color::gray ? "gray"
                                   : "white")
        << " - start visit: " << start_visit << " - end visit: " << end_visit << " - predecessor: (";
    auto pred = predecessor.lock();
    if (pred)
      out << pred->data;
    else
      out << "NULL";
    out << ")" << std::endl;
  }
};

using shared_node_ptr = std::shared_ptr<Node>;
shared_node_ptr shared_node_factory(const int data) { return std::make_shared<Node>(data); }

#endif