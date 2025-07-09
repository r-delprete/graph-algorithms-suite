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
      auto edge = get_edge(node, adj);

      if (adj->get_color() == Color::white) {
        adj->set_predecessor(node);
        edge->set_type(EdgeType::tree);
        dfs_visit(adj, path);
      } else if (adj->get_color() == Color::gray) {
        edge->set_type(EdgeType::backward);

        std::cout << "[dfs_visit INFO] Cycle found" << std::endl;
        ++cycles;
        int start = -1;
        for (int i = 0; i < path.size(); i++) {
          if (adj == path[i]) {
            start = i;
            break;
          }
        }

        if (start != -1) {
          for (int i = start; i < path.size(); i++) std::cout << "(" << path[i]->get_data() << ") -> ";
          std::cout << "(" << path[start]->get_data() << ")" << std::endl;
        }
      } else if (adj->get_color() == Color::black) {
        if (node->get_start_visit() < adj->get_start_visit())
          edge->set_type(EdgeType::forward);
        else if (node->get_start_visit() > adj->get_start_visit())
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

    for (int i = 0; i < tot_nodes; i++) insert_node(shared_node_ptr(new Node(i)));

    while (std::getline(input, line)) {
      format_line(line);
      iss.str(line);

      int src_data, dest_data, weight;
      iss >> src_data >> dest_data >> weight;

      auto src = get_node(src_data), dest = get_node(dest_data);

      if (src && dest) insert_edge(shared_edge_ptr(new Edge(src, dest, weight)));

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
      if (node->get_data() == data) {
        std::cout << "[get_node INFO] Node (" << data << ") found" << std::endl;
        return node;
      }
    }

    std::cerr << "[get_node ERROR] Node (" << data << ") not found" << std::endl;
    return nullptr;
  }

  shared_edge_ptr get_edge(const shared_node_ptr& src, const shared_node_ptr& dest) {
    if (edges.empty()) {
      std::cerr << "[get_edge ERROR] No edges in graph" << std::endl;
      return nullptr;
    }

    for (auto& edge : edges) {
      if (edge->get_source() == src && edge->get_destination() == dest) {
        std::cout << "[get_edge INFO] Edge (" << src->get_data() << ") -> (" << dest->get_data() << ") found"
                  << std::endl;
        return edge;
      }
    }

    std::cerr << "[get_edge ERROR] Edge (" << src->get_data() << ") -> (" << dest->get_data() << ") not found"
              << std::endl;
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
  }
};

#endif