#ifndef NODE_HPP
#define NODE_HPP

#include <climits>
#include <iostream>
#include <memory>
#include <vector>

enum Color { white, black, gray };

class Node {
private:
  int data, distance;
  Color color;
  std::weak_ptr<Node> predecessor;
  std::vector<std::weak_ptr<Node>> adj;

public:
  Node(int data) : data(data), color(Color::white), distance(INT_MAX) {}

  const int get_data() const { return data; }
  const Color get_color() const { return color; }
  const std::shared_ptr<Node>& get_predecessor() const { return predecessor.lock(); }
  const int get_distance() const { return distance; }
  const std::vector<std::weak_ptr<Node>>& get_adj_list() const { return adj; }

  void set_data(const int data) { this->data = data; }
  void set_color(const Color color) { this->color = color; }
  void set_predecessor(const std::shared_ptr<Node>& predecessor) { this->predecessor = predecessor; }
  void set_distance(const int distance) { this->distance = distance; }
  void add_adjacent(const std::shared_ptr<Node>& node) { adj.push_back(node); }

  void print(std::ostream& out = std::cout) {
    out << "(" << data << ") => distance: " << distance << " - predecessor (";
    auto pred = predecessor.lock();
    if (pred)
      out << pred->data;
    else
      out << "NULL";

    out << ")" << std::endl;
  }

  void reset() {
    color = Color::white;
    this->set_predecessor(nullptr);
    distance = INT_MAX;
  }
};

using shared_node_ptr = std::shared_ptr<Node>;
shared_node_ptr create_node(const int data) { return std::make_shared<Node>(data); }

#endif  // NODE_HPP