#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <fstream>
#include <queue>
#include <set>
#include <sstream>
#include <string>

#include "edge.hpp"

class Graph {
  std::vector<Node*> nodes;
  std::vector<Edge*> edges;
  int tot_nodes, tot_edges;

  struct Compare {
    bool operator()(Node* node1, Node* node2) { return node1->get_distance() > node2->get_distance(); }
  };

  void format_line(std::string& line) {
    if (line.front() == '<') line = line.substr(1);
    if (line.back() == '>') line.pop_back();
    for (auto& c : line) {
      if (c == ',') c = ' ';
    }
  }

  void clear() {
    for (auto& node : nodes) {
      delete node;
      node = nullptr;
    }
    nodes.clear();

    for (auto& edge : edges) {
      delete edge;
      edge = nullptr;
    }
    edges.clear();
  }

  void clear_stream(std::istringstream& stream) {
    stream.clear();
    stream.str("");
  }

public:
  Graph(std::ifstream& input) { load(input); }

  ~Graph() { clear(); }

  void load(std::ifstream& input) {
    clear();
    input.clear();
    input.seekg(0, std::ios::beg);

    std::string line;
    std::getline(input, line);
    format_line(line);
    std::istringstream iss(line);
    iss >> tot_nodes >> tot_edges;
    clear_stream(iss);

    for (int i = 0; i < tot_nodes; i++) insert_node(new Node(i));

    while (std::getline(input, line)) {
      format_line(line);
      iss.str(line);

      int src_data, dest_data, weight;
      iss >> src_data >> dest_data >> weight;
      Node *src = get_node(src_data), *dest = get_node(dest_data);
      if (src && dest) insert_edge(new Edge(src, dest, weight));

      clear_stream(iss);
    }
  }

  void insert_node(Node* node) {
    nodes.push_back(node);
    if (nodes.size() > tot_nodes) tot_nodes = nodes.size();
  }

  void insert_edge(Edge* edge) {
    edge->get_source()->add_adjacent(edge->get_destination());
    edge->get_destination()->add_adjacent(edge->get_source());

    edges.push_back(edge);
    if (edges.size() > tot_edges) tot_edges = edges.size();
  }

  Node* get_node(int data) {
    for (auto& node : nodes) {
      if (node->get_data() == data) return node;
    }

    std::cerr << "[get_node ERROR] => Node (" << data << ") not found" << std::endl;
    return nullptr;
  }

  Edge* get_edge(Node* src, Node* dest) {
    for (auto& edge : edges) {
      if ((edge->get_source() == src && edge->get_destination() == dest) ||
          (edge->get_source() == dest && edge->get_destination() == src))
        return edge;
    }

    std::cerr << "[get_edge ERROR] => Edge (" << src->get_data() << ") -> (" << dest->get_data() << ") not found"
              << std::endl;
    return nullptr;
  }

  void bfs(Node*& src) {
    for (auto& node : nodes) {
      node->set_color(Color::white);
      node->set_distance(INT_MAX);
      node->set_predecessor(nullptr);
    }

    src->set_color(Color::gray);
    src->set_distance(0);
    src->set_predecessor(nullptr);

    std::queue<Node*> q;
    q.push(src);

    while (!q.empty()) {
      Node* node = q.front();
      q.pop();

      for (auto& adj : node->get_adj_list()) {
        if (adj->get_color() == Color::white) {
          adj->set_predecessor(node);
          adj->set_color(Color::gray);
          adj->set_distance(node->get_distance() + 1);
          q.push(adj);
        }
      }

      node->set_color(Color::black);
    }
  }

  void print(std::string message = "Graph", std::ostream& out = std::cout) {
    out << message << std::endl << "Nodes" << std::endl;
    for (auto& node : nodes) node->print(out);

    out << "Edges" << std::endl;
    for (auto& edge : edges) edge->print(out);
    out << std::endl;
  }

  void prim(Node* src) {
    for (auto& node : nodes) {
      node->set_distance(INT_MAX);
      node->set_predecessor(nullptr);
    }
    src->set_distance(0);
    std::priority_queue<Node*, std::vector<Node*>, Compare> pq;
    pq.push(src);

    std::set<Node*> in_mst;

    while (!pq.empty()) {
      Node* node = pq.top();
      pq.pop();

      in_mst.insert(node);

      for (auto& adj : node->get_adj_list()) {
        Edge* edge = get_edge(node, adj);
        if (!edge) return;

        if ((in_mst.find(adj) == in_mst.end()) && (adj->get_distance() > edge->get_weight())) {
          adj->set_predecessor(node);
          adj->set_distance(edge->get_weight());
          pq.push(adj);
        }
      }
    }
  }

  void print_mst(std::ostream& out = std::cout) {
    out << "Minimum Spanning Tree (MST)" << std::endl;
    for (auto& node : nodes) node->print(out);
    out << std::endl;
  }

  bool is_binary() {
    for (auto& node : nodes) {
      int children = 0;
      for (auto& adj : node->get_adj_list()) {
        if (adj != node->get_predecessor()) children++;
      }

      if (children > 2) return false;
    }

    return true;
  }

  bool is_complete_binary() {
    for (auto& node : nodes) {
      std::vector<Node*> children;
      for (auto& adj : node->get_adj_list())
        if (adj != node->get_predecessor()) {
          children.push_back(adj);
        }

      if (children.size() < 2 && !children.empty()) return false;
    }

    return true;
  }
};

#endif