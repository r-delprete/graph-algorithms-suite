#ifndef NODE_HPP
#define NODE_HPP

#include <climits>
#include <iostream>
#include <memory>
#include <vector>

enum Color { white, gray, black };

class Node {
  int data, start_visit, end_visit, distance;
  std::weak_ptr<Node> predecessor;
  std::vector<std::shared_ptr<Node>> adj_list;
  Color color;

  const std::string color_to_string() const {
    return (color == Color::white ? "white" : color == Color::black ? "black" : "gray");
  }

public:
  Node(int data) : data(data), start_visit(INT_MAX), end_visit(INT_MAX), distance(INT_MAX), color(Color::white) {}

  const int get_data() const { return data; }
  const int get_start_visit() const { return start_visit; }
  const int get_end_visit() const { return end_visit; }
  const int get_distance() const { return distance; }
  std::weak_ptr<Node> get_predecessor() const { return predecessor; }
  const std::vector<std::shared_ptr<Node>>& get_adj_list() const { return adj_list; }
  std::vector<std::shared_ptr<Node>>& get_adj_list() { return adj_list; }
  const Color get_color() const { return color; }

  void set_data(int data) { this->data = data; }
  void set_start_visit(int start_visit) { this->start_visit = start_visit; }
  void set_end_visit(int end_visit) { this->end_visit = end_visit; }
  void set_distance(int distance) { this->distance = distance; }
  void set_predecessor(const std::shared_ptr<Node>& predecessor) { this->predecessor = predecessor; }
  void add_adjacent(const std::shared_ptr<Node>& node) { adj_list.push_back(node); }
  void set_color(Color color) { this->color = color; }

  void print(std::ostream& out = std::cout) {
    out << "(" << data << ") => start visit: " << start_visit << " - end visit: " << end_visit
        << " - distance: " << distance << " - color: " << color_to_string() << " - predecessor: (";

    auto pred = predecessor.lock();
    if (pred)
      out << pred->data;
    else
      out << "NULL";
    out << ")" << std::endl;
  }
};

using shared_node_ptr = std::shared_ptr<Node>;

#endif