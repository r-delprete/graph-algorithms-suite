#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "edge.hpp"

class Graph {
  std::vector<Node*> nodes;
  std::vector<Edge*> edges;
  int time = 0, tot_nodes, tot_edges, cycles = 0;

  void clear() {
    if (!nodes.empty()) {
      for (auto& node : nodes) {
        delete node;
        node = nullptr;
      }

      nodes.clear();
    }

    if (!edges.empty()) {
      for (auto& edge : edges) {
        delete edge;
        edge = nullptr;
      }

      edges.clear();
    }

    time = tot_nodes = tot_edges = cycles = 0;
  }

  void clear_stream(std::istringstream& stream) {
    stream.clear();
    stream.str("");
  }

  void format_line(std::string& line) {
    if (line.front() == '<') line = line.substr(1);
    if (line.back() == '>') line.pop_back();
    for (auto& c : line) c = c == ',' ? ' ' : c;
  }

  void dfs_visit(Node* node, std::vector<Node*>& path) {
    node->set_color(Color::gray);
    node->set_start_discovery(++time);
    path.push_back(node);

    for (auto& adj : node->get_adj_list()) {
      Edge* edge = get_edge(node, adj);

      if (adj->get_color() == Color::white) {
        adj->set_predecessor(node);
        edge->set_type(EdgeType::tree);
        dfs_visit(adj, path);
      } else if (adj->get_color() == Color::gray) {
        std::cout << "[dfs_visit INFO] Cycle found" << std::endl;
        ++cycles;

        edge->set_type(EdgeType::backward);

        int index = -1;
        for (int i = 0; i < path.size(); i++) {
          if (path[i] == adj) {
            index = i;
            break;
          }
        }

        if (index != -1) {
          for (int i = index; i < path.size(); i++) std::cout << "(" << path[i]->get_data() << ") -> ";
          std::cout << "(" << path[index]->get_data() << ")" << std::endl;
        }
      } else if (adj->get_color() == Color::black) {
        if (node->get_start_discovery() < adj->get_start_discovery()) {
          edge->set_type(EdgeType::forward);
        } else if (node->get_start_discovery() > adj->get_start_discovery()) {
          edge->set_type(EdgeType::cross);
        }
      }
    }

    node->set_end_visit(++time);
    node->set_color(Color::black);

    path.pop_back();
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

    for (int i = 0; i < tot_nodes; i++) insert_node(new Node(i));

    while (std::getline(input, line)) {
      format_line(line);
      clear_stream(iss);
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
    tot_nodes = tot_nodes < nodes.size() ? nodes.size() : tot_nodes;
  }

  void insert_edge(Edge* edge) {
    edge->get_source()->add_adjacent(edge->get_destination());
    edges.push_back(edge);
    tot_edges = tot_edges < edges.size() ? edges.size() : tot_edges;
  }

  Node* get_node(int data) {
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

  Edge* get_edge(Node* src, Node* dest) {
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

  void dfs() {
    std::vector<Node*> path;

    for (auto& node : nodes) {
      node->set_color(Color::white);
      node->set_predecessor(nullptr);
      node->set_start_discovery(INT_MAX);
      node->set_end_visit(INT_MAX);
    }

    for (auto& node : nodes) {
      if (node->get_color() == Color::white) dfs_visit(node, path);
    }
  }

  void print(std::string message = "Graph", std::ostream& out = std::cout) {
    out << message << std::endl;
    for (auto& node : nodes) node->print(out);
    out << "Edges" << std::endl;
    for (auto& edge : edges) edge->print(out);
    out << std::endl;
    out << "Total cycles => " << cycles << std::endl;
  }
};

#endif