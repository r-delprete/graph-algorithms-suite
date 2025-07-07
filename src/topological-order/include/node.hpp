#ifndef NODE_HPP
#define NODE_HPP

#include <climits>
#include <iostream>
#include <vector>

enum Color { white, gray, black };

class Node {
  int data, start_discovery, end_visit, distance;
  Color color;
  Node* predecessor;
  std::vector<Node*> adj_list;

public:
  Node(int data)
      : data(data),
        start_discovery(INT_MAX),
        end_visit(INT_MAX),
        distance(INT_MAX),
        color(Color::white),
        predecessor(nullptr) {}

  int get_data() const { return data; }
  Color get_color() const { return color; }
  Node* get_predecessor() const { return predecessor; }
  std::vector<Node*>& get_adj_list() { return adj_list; }
  const std::vector<Node*>& get_adj_list() const { return adj_list; }
  int get_start_discovery() { return start_discovery; }
  int get_end_visit() const { return end_visit; }
  int get_distance() const { return distance; }

  void set_data(int data) { this->data = data; }
  void set_color(Color color) { this->color = color; }
  void set_predecessor(Node* predecessor) { this->predecessor = predecessor; }
  void add_adjacent(Node* adj) { adj_list.push_back(adj); }
  void set_start_discovery(int start_discovery) { this->start_discovery = start_discovery; }
  void set_end_visit(int end_visit) { this->end_visit = end_visit; }
  void set_distance(int distance) { this->distance = distance; }

  void get_info(std::ostream& out = std::cout) {
    out << "Node (" << data << ") => start visit: " << start_discovery << " - end visit: " << end_visit
        << " - distance: " << distance << " - predecessor: (";
    predecessor ? out << predecessor->get_data() : out << "NULL";
    out << ")" << std::endl;
  }
};

#endif