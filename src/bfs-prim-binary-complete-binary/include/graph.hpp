#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <fstream>
#include <queue>
#include <set>
#include <sstream>
#include <string>

#include "edge.hpp"

class Graph {
  std::vector<shared_node_ptr> nodes;
  std::vector<shared_edge_ptr> edges;
  int tot_nodes, tot_edges;

  struct Compare {
    bool operator()(const shared_node_ptr& node1, const shared_node_ptr& node2) {
      return node1->get_distance() > node2->get_distance();
    }
  };

  void format_line(std::string& line) {
    if (line.empty()) return;
    if (line.front() == '<') line = line.substr(1);
    if (line.back() == '>') line.pop_back();
    for (auto& ch : line) {
      if (ch == ',') ch = ' ';
    }
  }

  void clear_stream(std::istringstream& stream) {
    stream.clear();
    stream.str("");
  }

public:
  Graph(std::ifstream& input) { load(input); }

  void reset() {
    nodes.clear();
    edges.clear();
    tot_nodes = tot_edges = 0;
  }

  void load(std::ifstream& input) {
    reset();
    input.clear();
    input.seekg(0, std::ios::beg);

    std::string line;
    std::getline(input, line);
    format_line(line);

    std::istringstream iss(line);
    iss >> tot_nodes >> tot_edges;

    for (int i = 0; i < tot_nodes; i++) insert_node(create_node(i));

    clear_stream(iss);
    line.clear();
    while (std::getline(input, line)) {
      format_line(line);
      iss.str(line);

      int src_data, dest_data, weight;
      iss >> src_data >> dest_data >> weight;
      auto src = get_node(src_data), dest = get_node(dest_data);
      insert_edge(create_edge(src, dest, weight));

      clear_stream(iss);
      line.clear();
    }
  }

  void insert_node(const shared_node_ptr& node) {
    nodes.push_back(node);
    if (nodes.size() > tot_nodes) tot_nodes = nodes.size();
  }

  void insert_edge(const shared_edge_ptr& edge) {
    edge->get_source()->add_adjacent(edge->get_destination());
    edge->get_destination()->add_adjacent(edge->get_source());
    edges.push_back(edge);
    if (edges.size() > tot_edges) tot_edges = edges.size();
  }

  shared_node_ptr get_node(const int data) const {
    if (nodes.empty()) {
      std::cerr << "[get_node ERROR] No node in graph" << std::endl;
      return nullptr;
    }

    for (auto& node : nodes) {
      if (node->get_data() == data) return node;
    }

    return nullptr;
  }

  shared_edge_ptr get_edge(const shared_node_ptr& src, const shared_node_ptr& dest) const {
    if (edges.empty()) {
      std::cerr << "[get_edge ERROR] No edge in graph" << std::endl;
      return nullptr;
    }

    for (auto& edge : edges) {
      if ((edge->get_source() == src && edge->get_destination() == dest) ||
          (edge->get_source() == dest && edge->get_destination() == src))
        return edge;
    }

    return nullptr;
  }

  void bfs(const shared_node_ptr& src) {
    for (auto& node : nodes) node->reset();

    src->set_distance(0);
    src->set_predecessor(nullptr);
    src->set_color(Color::gray);

    std::queue<shared_node_ptr> q;
    q.push(src);

    while (!q.empty()) {
      const auto node = q.front();
      q.pop();

      for (auto& adj : node->get_adj_list()) {
        const auto adjacent = adj.lock();

        if (adjacent->get_color() == Color::white) {
          adjacent->set_color(Color::gray);
          adjacent->set_predecessor(node);
          adjacent->set_distance(node->get_distance() + 1);
          q.push(adjacent);
        }
      }

      node->set_color(Color::black);
    }
  }

  void print(std::ostream& out = std::cout, const std::string message = "Graph") {
    out << message << std::endl;

    out << "Nodes" << std::endl;
    for (auto& node : nodes) node->print(out);
    out << std::endl;

    out << "Edges" << std::endl;
    for (auto& edge : edges) edge->print(out);
    out << std::endl;

    out << std::endl;
  }

  void prim(const shared_node_ptr& src) {
    for (auto& node : nodes) {
      node->set_distance(INT_MAX);
      node->set_predecessor(nullptr);
    }

    src->set_distance(0);
    src->set_predecessor(nullptr);

    std::priority_queue<shared_node_ptr, std::vector<shared_node_ptr>, Compare> pq;
    for (auto& node : nodes) pq.push(node);
    std::set<shared_node_ptr> in_mst;

    while (!pq.empty()) {
      const auto node = pq.top();
      pq.pop();
      in_mst.insert(node);

      for (auto& adj : node->get_adj_list()) {
        const auto adjacent = adj.lock();
        const auto edge = get_edge(node, adjacent);

        if (in_mst.find(adjacent) == in_mst.end() && adjacent->get_distance() > edge->get_weight()) {
          adjacent->set_predecessor(node);
          adjacent->set_distance(edge->get_weight());
          pq.push(adjacent);
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
        if (adj.lock() != node->get_predecessor()) children++;
      }

      if (children > 2) return false;
    }

    return true;
  }

  bool is_complete_binary() {
    for (auto& node : nodes) {
      std::vector<shared_node_ptr> children;

      for (auto& adj : node->get_adj_list()) {
        if (adj.lock() != node->get_predecessor()) children.push_back(adj.lock());
      }

      if (!children.empty() && children.size() < 2) return false;
    }

    return true;
  }
};

#endif