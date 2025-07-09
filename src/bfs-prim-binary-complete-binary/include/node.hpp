#ifndef NODE_HPP
#define NODE_HPP

#include <climits>
#include <iostream>
#include <memory>
#include <vector>

enum Color { white, black, gray };

class Node {
private:
  int data;
  Color color;
  Node* predecessor;
  int distance;
  std::vector<Node*> adj;

public:
  Node(int data) : data(data), color(Color::white), distance(INT_MAX) {}

  int get_data() const { return data; }
  Color get_color() const { return color; }
  Node* get_predecessor() const { return predecessor; }
  int get_distance() const { return distance; }
  const std::vector<Node*>& get_adj_list() const { return adj; }
  std::vector<Node*>& get_adj_list() { return adj; }

  void set_data(int data) { this->data = data; }
  void set_color(Color color) { this->color = color; }
  void set_predecessor(Node* predecessor) { this->predecessor = predecessor; }
  void set_distance(int distance) { this->distance = distance; }
  void add_adjacent(Node* node) { adj.push_back(node); }

  void print(std::ostream& out = std::cout) {
    out << "(" << data << ") => distance: " << distance << " - predecessor (";
    if (predecessor)
      out << predecessor->data;
    else
      out << "NULL";

    out << ")" << std::endl;
  }
};

using unique_node_ptr = std::unique_ptr<Node>;
using shared_node_ptr = std::shared_ptr<Node>;

#endif  // NODE_HPP