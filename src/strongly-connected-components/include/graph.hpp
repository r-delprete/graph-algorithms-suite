#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <fstream>
#include <queue>
#include <set>
#include <sstream>
#include <stack>

#include "edge.hpp"
#include "node.hpp"

inline void format_line(std::string& line) {
  if (line.empty()) return;
  if (line.front() == '<') line = line.substr(1);
  if (line.back() == '>') line.pop_back();
  for (auto& ch : line) {
    if (ch == ',') ch = ' ';
  }
}

inline void clear_stream(std::istringstream& stream) {
  stream.clear();
  stream.str("");
}

class Graph {
  std::vector<shared_node> nodes;
  std::vector<shared_edge> edges;
  int time = 0, tot_nodes, tot_edges;
  std::stack<shared_node> topological;
  std::stack<shared_node> scc;
  std::vector<std::vector<shared_node>> sccs_list;

  void dfs_topological_order(const shared_node& node) {
    node->set_color(Color::gray);
    node->set_start_visit(++time);

    for (auto& adj : node->get_adj_list()) {
      const auto adjacent = adj.lock();
      if (adjacent->get_color() == Color::white) {
        adjacent->set_predecessor(node);
        dfs_topological_order(adjacent);
      }
    }

    node->set_color(black);
    node->set_end_visit(++time);

    topological.push(node);
  }

  void dfs_sccs(const shared_node& node) {
    node->set_color(Color::gray);
    node->set_start_visit(++time);

    for (auto& adj : node->get_adj_list()) {
      const auto adjacent = adj.lock();
      if (adjacent->get_color() == Color::white) {
        adjacent->set_predecessor(node);
        dfs_topological_order(adjacent);
      }
    }

    node->set_color(black);
    node->set_end_visit(++time);

    scc.push(node);
  }

  std::shared_ptr<Graph> create_transpose() {
    std::shared_ptr<Graph> g = std::make_shared<Graph>();

    for (auto& node : nodes) g->insert_node(create_node(node->get_data()));
    for (auto& edge : edges)
      g->insert_edge(create_edge(g->get_node(edge->get_destination()->get_data()),
                                 g->get_node(edge->get_source()->get_data()), edge->get_weight()));

    return g;
  }

  void get_scc(const shared_node& node, std::vector<shared_node>& current_scc, std::vector<bool>& visited) {
    visited[node->get_data()] = true;
    current_scc.push_back(node);

    for (auto& adj : node->get_adj_list()) {
      const auto adjacent = adj.lock();
      if (!visited[adjacent->get_data()]) {
        adjacent->set_predecessor(node);
        get_scc(adjacent, current_scc, visited);
      }
    }
  }

  void initialize_single_source(const shared_node& src) {
    for (auto& node : nodes) {
      node->set_distance(INT_MAX);
      node->set_predecessor(nullptr);
    }

    src->set_distance(0);
    src->set_predecessor(nullptr);
  }

  void relax(const shared_edge& edge) {
    const auto src = edge->get_source();
    const auto dest = edge->get_destination();

    if (dest->get_distance() > src->get_distance() + edge->get_weight()) {
      dest->set_predecessor(src);
      dest->set_distance(src->get_distance() + edge->get_weight());
    }
  }

  struct Compare {
    bool operator()(const shared_node& node1, const shared_node& node2) {
      return node1->get_distance() > node2->get_distance();
    }
  };

public:
  Graph() {}

  Graph(std::ifstream& input) { load(input); }

  void reset() {
    nodes.clear();
    edges.clear();
    time = tot_nodes = tot_edges = 0;

    sccs_list.clear();
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

    line.clear();
    clear_stream(iss);

    while (std::getline(input, line)) {
      format_line(line);
      iss.str(line);

      int src_data, dest_data, weight;
      iss >> src_data >> dest_data >> weight;

      const shared_node src = get_node(src_data);
      const shared_node dest = get_node(dest_data);

      if (src && dest) insert_edge(create_edge(src, dest, weight));

      clear_stream(iss);
      line.clear();
    }
  }

  void insert_node(const shared_node& node) {
    nodes.push_back(node);
    if (nodes.size() > tot_nodes) tot_nodes = nodes.size();
  }

  void insert_edge(const shared_edge& edge) {
    edges.push_back(edge);
    edge->get_source()->add_adjacent(edge->get_destination());
    if (edges.size() > tot_edges) tot_edges = edges.size();
  }

  shared_node get_node(const int data) const {
    if (nodes.empty()) {
      std::cerr << "[get_node ERROR] No node in graph" << std::endl;
      return nullptr;
    }

    for (auto& node : nodes) {
      if (node->get_data() == data) return node;
    }

    return nullptr;
  }

  shared_edge get_edge(const shared_node& src, const shared_node& dest) const {
    if (edges.empty()) {
      std::cerr << "[get_edge ERROR] No edge in graph" << std::endl;
      return nullptr;
    }

    for (auto& edge : edges) {
      if (edge->get_source() == src && edge->get_destination() == dest) return edge;
    }

    return nullptr;
  }

  void compute_sccs() {
    for (auto& node : nodes) node->reset();

    for (auto& node : nodes) {
      if (node->get_color() == white) dfs_sccs(node);
    }

    const auto transposed = create_transpose();
    std::vector<bool> visited(tot_nodes, false);

    while (!scc.empty()) {
      auto node = scc.top();
      scc.pop();
      auto transposed_node = transposed->get_node(node->get_data());

      if (!visited[transposed_node->get_data()]) {
        std::vector<shared_node> current_scc;
        get_scc(transposed_node, current_scc, visited);
        sccs_list.push_back(current_scc);
      }
    }
  }

  void compute_topological_order() {
    for (auto& node : nodes) node->reset();

    for (auto& node : nodes) {
      if (node->get_color() == Color::white) dfs_topological_order(node);
    }
  }

  void print(std::ostream& out = std::cout, const std::string message = "Graph") const {
    out << message << std::endl;

    out << "Nodes" << std::endl;
    for (auto& node : nodes) node->print(out);
    out << std::endl;

    out << "Edges" << std::endl;
    for (auto& edge : edges) edge->print(out);
    out << std::endl;
  }

  void print_sccs(std::ostream& out = std::cout, const std::string message = "Strongly Connected Components") const {
    out << message << std::endl;

    for (int i = 0; i < sccs_list.size(); i++) {
      out << "Component #" << i + 1 << " => ";
      for (int k = 0; k < sccs_list[i].size(); k++)
        out << "(" << sccs_list[i][k]->get_data() << ")" << (k == sccs_list[i].size() - 1 ? "" : "\t");
      out << std::endl;
    }

    out << std::endl;
  }

  void print_topological_order(std::ostream& out = std::cout, const std::string message = "Topological order") {
    out << message << std::endl;

    while (!topological.empty()) {
      const auto node = topological.top();
      topological.pop();
      node->print(out);
    }

    out << std::endl;
  }

  void bellman_ford(const shared_node& src, const shared_node& dest, std::ostream& out = std::cout) {
    initialize_single_source(src);

    for (int i = 0; i < tot_nodes - 1; i++) {
      for (auto& edge : edges) relax(edge);
    }

    for (auto& edge : edges) {
      const auto source = edge->get_source();
      const auto destination = edge->get_destination();

      if (destination->get_distance() > source->get_distance() + edge->get_weight()) {
        std::cerr << "[bellman_ford ERROR] Negative cycle found" << std::endl;
        return;
      }
    }

    std::vector<shared_node> path;
    auto current = dest;
    while (current) {
      path.push_back(current);
      current = current->get_predecessor().lock();
    }

    out << "BELLMAN FORD" << std::endl;
    out << "Minimum distance from (" << src->get_data() << ") to (" << dest->get_data() << ") => "
        << dest->get_distance() << std::endl;
    out << "Path => ";
    for (int i = path.size() - 1; i > 0; i--) out << "(" << path[i]->get_data() << ") -> ";
    out << "(" << path[0]->get_data() << ")" << std::endl;

    out << std::endl;
  }

  void dijkstra(const shared_node& src, std::ostream& out = std::cout) {
    initialize_single_source(src);

    std::priority_queue<shared_node, std::vector<shared_node>, Compare> pq;
    for (auto& node : nodes) pq.push(node);
    std::set<shared_node> visited;

    while (!pq.empty()) {
      const auto node = pq.top();
      pq.pop();

      if (visited.find(node) != visited.end()) continue;
      visited.insert(node);

      for (auto& adj : node->get_adj_list()) {
        const auto adjacent = adj.lock();
        const auto edge = get_edge(node, adjacent);

        if (edge) {
          relax(edge);
          if (visited.find(adjacent) == visited.end()) pq.push(adjacent);
        }
      }
    }

    out << "DIJKSTRA" << std::endl;
    out << "Minimum distances from (" << src->get_data() << ")" << std::endl;
    for (auto& node : nodes) out << "(" << node->get_data() << ") => " << node->get_distance() << std::endl;
    out << std::endl;
  }

  void prim(const shared_node& src, std::ostream& out = std::cout) {
    for (auto& node : nodes) {
      node->set_distance(INT_MAX);
      node->set_predecessor(nullptr);
    }

    src->set_distance(0);
    src->set_predecessor(nullptr);

    std::priority_queue<shared_node, std::vector<shared_node>, Compare> pq;
    for (auto& node : nodes) pq.push(node);
    std::set<shared_node> in_mst;

    while (!pq.empty()) {
      const auto node = pq.top();
      pq.pop();

      if (in_mst.find(node) != in_mst.end()) continue;
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

    out << "Minimum Spanning Tree (MST) - Prim" << std::endl;
    for (auto& node : nodes) node->print(out);
    out << std::endl;
  }
};

#endif