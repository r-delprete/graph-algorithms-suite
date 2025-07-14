#ifndef NODE_HPP
#define NODE_HPP

#include <climits>
#include <iostream>
#include <memory>
#include <vector>

enum Color { white, gray, black };

class Node {
  int data, start_visit, end_visit;
  std::weak_ptr<Node> predecessor;
  std::vector<std::weak_ptr<Node>> adj_list;
  Color color;

public:
  Node(int data) : data(data), start_visit(INT_MAX), end_visit(INT_MAX), color(Color::white) {}

  const int get_data() const { return data; }
  const int get_start_visit() const { return start_visit; }
  const int get_end_visit() const { return end_visit; }
  const Color get_color() const { return color; }
  const std::weak_ptr<Node> get_predecessor() const { return predecessor; }
  const std::vector<std::weak_ptr<Node>>& get_adj_list() const { return adj_list; }

  void set_data(int data) { this->data = data; }
  void set_start_visit(int start_visit) { this->start_visit = start_visit; }
  void set_end_visit(int end_visit) { this->end_visit = end_visit; }
  void set_color(Color color) { this->color = color; }
  void set_predecessor(const std::shared_ptr<Node>& predecessor) { this->predecessor = predecessor; }
  void add_adjacent(const std::shared_ptr<Node>& node) { adj_list.push_back(node); }

  void print(std::ostream& out = std::cout) {
    out << "(" << data << ") => start visit: " << start_visit << " - end visit: " << end_visit << " - color: "
        << (color == Color::white  ? "white"
            : color == Color::gray ? "gray"
                                   : "black")
        << " - predecessor (";
    auto pred = predecessor.lock();
    if (pred)
      out << pred->data;
    else
      out << "NULL";

    out << ")" << std::endl;
  }
};

using shared_node_ptr = std::shared_ptr<Node>;
shared_node_ptr create_node(const int data) { return std::make_shared<Node>(data); }

#endif  // NODE_HPP