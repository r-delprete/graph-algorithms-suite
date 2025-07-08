#ifndef NODE_HPP
#define NODE_HPP

#include <climits>
#include <vector>

enum Color { white, gray, black };

class Node {
private:
  int data;
  int start_discovery;
  int end_visit;
  Node* predecessor;
  std::vector<Node*> adj;
  Color color;

public:
  Node(int data)
      : data(data), start_discovery(INT_MAX), end_visit(INT_MAX), predecessor(nullptr), color(Color::white) {}

  int get_data() { return data; }
  int get_start_discovery() { return start_discovery; }
  int get_end_visit() { return end_visit; }
  Node* get_predecessor() { return predecessor; }
  Color get_color() { return color; }
  const std::vector<Node*>& get_adj_list() const { return adj; }
  std::vector<Node*>& get_adj_list() { return adj; }

  void set_data(int data) { this->data = data; }
  void set_start_discovery(int start_discovery) { this->start_discovery = start_discovery; }
  void set_end_visit(int end_visit) { this->end_visit = end_visit; }
  void set_predecessor(Node* predecessor) { this->predecessor = predecessor; }
  void add_adjacent(Node* node) { adj.push_back(node); }
  void set_color(Color color) { this->color = color; }

  void print(std::ostream& out = std::cout) {
    out << "Node (" << data << ") => start visit: " << start_discovery << " - end visit: " << end_visit
        << " - predecessor: (";
    predecessor ? out << predecessor->get_data() : out << "NULL";
    out << ")" << std::endl;
  }
};

#endif  // NODE_HPP