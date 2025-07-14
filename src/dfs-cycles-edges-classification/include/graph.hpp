#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <fstream>
#include <sstream>

#include "edge.hpp"

class Graph {
  std::vector<shared_node_ptr> nodes;
  std::vector<shared_edge_ptr> edges;
  int time = 0, tot_nodes, tot_edges, cycles;

  void format_line(std::string& line) {
    if (line.front() == '<') line = line.substr(1);
    if (line.back() == '>') line.pop_back();
    for (auto& c : line) c = c == ',' ? ' ' : c;
  }

  void clear_stream(std::istringstream& stream) {
    stream.clear();
    stream.str("");
  }

  void dfs_visit(const shared_node_ptr& node, std::vector<shared_node_ptr>& path) {
    node->set_color(Color::gray);
    node->set_start_visit(++time);
    path.push_back(node);

    for (auto& adj : node->get_adj_list()) {
      const auto adjacent = adj.lock();
      const auto edge = get_edge(node, adjacent);

      if (adjacent->get_color() == Color::white) {
        adjacent->set_predecessor(node);
        edge->set_type(EdgeType::tree);
        dfs_visit(adjacent, path);
      } else if (adjacent->get_color() == Color::gray) {
        std::cout << "Cycle found" << std::endl;
        cycles++;
        edge->set_type(EdgeType::backward);

        int start = -1;
        for (int i = 0; i < path.size(); i++) {
          if (path[i] == adjacent) {
            start = i;
            break;
          }
        }

        if (start != -1) {
          for (int i = start; i < path.size(); i++) std::cout << "(" << path[i]->get_data() << ") -> ";
          std::cout << "(" << path[start]->get_data() << ")" << std::endl;
          std::cout << std::endl;
        }
      } else if (adjacent->get_color() == Color::black) {
        if (node->get_start_visit() < adjacent->get_start_visit())
          edge->set_type(EdgeType::forward);
        else if (node->get_start_visit() > adjacent->get_start_visit())
          edge->set_type(EdgeType::cross);
      }
    }

    node->set_color(Color::black);
    node->set_end_visit(++time);

    path.pop_back();
  }

public:
  Graph(std::ifstream& input) { load(input); }

  void reset() {
    nodes.clear();
    edges.clear();
    time = tot_nodes = tot_edges = cycles = 0;
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
    clear_stream(iss);
    line.clear();

    for (int i = 0; i < tot_nodes; i++) insert_node(create_node(i));

    while (std::getline(input, line)) {
      format_line(line);
      iss.str(line);

      int src_data, dest_data, weight;
      iss >> src_data >> dest_data >> weight;

      const auto src = get_node(src_data), dest = get_node(dest_data);

      if (src && dest) insert_edge(create_edge(src, dest, weight));

      clear_stream(iss);
      line.clear();
    }
  }

  void insert_node(const shared_node_ptr& node) {
    nodes.push_back(node);
    if (nodes.size() > tot_nodes) tot_nodes = nodes.size();
  }

  void insert_edge(const shared_edge_ptr& edge) {
    edges.push_back(edge);
    edge->get_source()->add_adjacent(edge->get_destination());
    if (edges.size() > tot_edges) tot_edges = edges.size();
  }

  shared_node_ptr get_node(int data) {
    if (nodes.empty()) {
      std::cerr << "[get_node ERROR] No nodes in graph" << std::endl;
      return nullptr;
    }

    for (auto& node : nodes) {
      if (node->get_data() == data) return node;
    }

    return nullptr;
  }

  shared_edge_ptr get_edge(const shared_node_ptr& src, const shared_node_ptr& dest) {
    if (edges.empty()) {
      std::cerr << "[get_edge ERROR] No edges in graph" << std::endl;
      return nullptr;
    }

    for (auto& edge : edges) {
      if (edge->get_source() == src && edge->get_destination() == dest) return edge;
    }

    return nullptr;
  }

  void print(std::string message = "Graph", std::ostream& out = std::cout) {
    out << message << std::endl;
    for (auto& node : nodes) node->print(out);
    out << "Edges" << std::endl;
    for (auto& edge : edges) edge->print(out);
    out << std::endl;
    out << "Total cycles => " << cycles << std::endl;
  }

  void dfs() {
    for (auto& node : nodes) {
      node->set_color(Color::white);
      node->set_start_visit(INT_MAX);
      node->set_end_visit(INT_MAX);
      node->set_predecessor(nullptr);
    }

    std::vector<shared_node_ptr> path;
    for (auto& node : nodes) {
      if (node->get_color() == Color::white) dfs_visit(node, path);
    }

    path.clear();
  }
};

#endif