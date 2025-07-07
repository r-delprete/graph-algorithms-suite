#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <fstream>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>

#include "edge.hpp"
#include "node.hpp"

class Graph {
  std::vector<Node*> nodes;
  std::vector<Edge*> edges;
  int time = 0, tot_nodes, tot_edges;
  std::stack<Node*> s;

  struct Compare {
    bool operator()(Node* node1, Node* node2) { return node1->get_distance() > node2->get_distance(); }
  };

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

  void format_line(std::string& line) {
    line = line.front() == '<' ? line.substr(1) : line;
    if (line.back() == '>') line.pop_back();
    for (auto& c : line) c = c == ',' ? ' ' : c;
  }

  void dfs_visit(Node* node) {
    node->set_color(Color::gray);
    node->set_start_discovery(++time);

    for (auto& adj : node->get_adj_list()) {
      if (adj->get_color() == Color::white) {
        adj->set_predecessor(node);
        dfs_visit(adj);
      }
    }

    node->set_end_visit(++time);
    node->set_color(Color::black);

    s.push(node);
  }

  void initialize_single_source(Node*& src) {
    for (auto& node : nodes) {
      node->set_distance(INT_MAX);
      node->set_predecessor(nullptr);
    }

    src->set_distance(0);
    src->set_predecessor(nullptr);
  }

  void relax(Edge*& edge) {
    if (edge->get_destination()->get_distance() > edge->get_source()->get_distance() + edge->get_weight()) {
      edge->get_destination()->set_distance(edge->get_source()->get_distance() + edge->get_weight());
      edge->get_destination()->set_predecessor(edge->get_source());
    }
  }

  bool is_node_valid(std::vector<Node*>& path, Node* node, int pos) {
    Node* last = path[pos - 1];

    for (auto& adj : last->get_adj_list()) {
      if (adj == node) {
        for (auto& path_node : path) {
          if (path_node == node) return false;
        }

        return true;
      }
    }

    return false;
  }

  bool hamiltonian_cycle_recursive(std::vector<Node*>& path, int pos) {
    if (pos == tot_nodes) {
      Node* last = path[pos - 1];
      for (auto& adj : last->get_adj_list()) {
        if (adj == path[0]) return true;
      }

      return false;
    }

    for (auto& node : nodes) {
      if (is_node_valid(path, node, pos)) {
        path[pos] = node;
        if (hamiltonian_cycle_recursive(path, pos + 1)) return true;
        path[pos] = nullptr;
      }
    }

    return false;
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

    iss.clear();
    iss.str("");

    for (int i = 0; i < tot_nodes; i++) insert_node(new Node(i));

    while (std::getline(input, line)) {
      int src_data, dest_data, weight;
      format_line(line);
      iss.str(line);

      iss >> src_data >> dest_data >> weight;
      Node *src = get_node(src_data), *dest = get_node(dest_data);
      insert_edge(new Edge(src, dest, weight));

      iss.clear();
      iss.str("");
    }
  }

  void insert_node(Node* node) {
    nodes.push_back(node);
    tot_nodes = nodes.size() > tot_nodes ? nodes.size() : tot_nodes;
  }

  void insert_edge(Edge* edge) {
    edge->get_source()->add_adjacent(edge->get_destination());
    edges.push_back(edge);
    tot_edges = edges.size() > tot_edges ? edges.size() : tot_edges;
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

  void dfs() {
    for (auto& node : nodes) {
      node->set_color(Color::white);
      node->set_start_discovery(INT_MAX);
      node->set_end_visit(INT_MAX);
      node->set_predecessor(nullptr);
    }

    for (auto& node : nodes) {
      if (node->get_color() == Color::white) dfs_visit(node);
    }
  }

  void print(std::string message = "Graph", std::ostream& out = std::cout) {
    out << message << std::endl;
    for (auto& node : nodes) node->get_info(out);
    out << "Edges" << std::endl;
    for (auto& edge : edges) edge->get_info(out);
    out << std::endl;
  }

  void print_topological_order(std::ostream& out = std::cout) {
    while (!s.empty()) {
      Node* node = s.top();
      s.pop();

      node->get_info(out);
    }

    out << std::endl;
  }

  void bellman_ford(Node* src, Node* dest, std::ostream& out = std::cout) {
    std::vector<Node*> path;

    initialize_single_source(src);

    for (int i = 0; i < nodes.size(); i++) {
      for (auto& edge : edges) relax(edge);
    }

    Node* current = dest;
    while (current) {
      path.push_back(current);
      current = current->get_predecessor();
    }

    out << "Bellman Ford" << std::endl;
    out << "Minimum path distance from (" << src->get_data() << ") to (" << dest->get_data() << ") => "
        << dest->get_distance() << std::endl;

    out << "Path" << std::endl;
    for (int i = path.size() - 1; i > 0; i--) out << "(" << path[i]->get_data() << ") -> ";
    out << "(" << path[0]->get_data() << ")" << std::endl;

    out << std::endl;
  }

  void dijkstra(Node* src, std::ostream& out = std::cout) {
    initialize_single_source(src);
    std::set<Node*> visited;
    std::priority_queue<Node*, std::vector<Node*>, Compare> pq;
    pq.push(src);

    while (!pq.empty()) {
      Node* node = pq.top();
      pq.pop();

      if (visited.find(node) != visited.end()) continue;
      visited.insert(node);

      for (auto& adj : node->get_adj_list()) {
        Edge* edge = get_edge(node, adj);

        if (edge) {
          relax(edge);
          if (visited.find(adj) == visited.end()) pq.push(adj);
        }
      }
    }

    out << "Dijkstra" << std::endl << "Minimum paths from source (" << src->get_data() << ")" << std::endl;
    for (auto& node : nodes) out << "Node (" << node->get_data() << ") => " << node->get_distance() << std::endl;
    out << std::endl;
  }

  bool hamiltonian_cycle(int pos, std::ostream& out = std::cout) {
    std::vector<Node*> path(tot_nodes, nullptr);
    path[0] = get_node(pos);

    if (hamiltonian_cycle_recursive(path, pos + 1)) {
      out << "Hamiltonian cycle found" << std::endl;

      for (auto& node : path) out << "(" << node->get_data() << ") -> ";
      out << "(" << path[0]->get_data() << ")" << std::endl;
      out << std::endl;

      return true;
    }

    out << "No hamiltonian cycle found" << std::endl;
    return false;
  }
};

#endif