#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <fstream>
#include <queue>
#include <set>
#include <sstream>
#include <stack>

#include "edge.hpp"
#include "node.hpp"

class Graph {
  std::vector<shared_node_ptr> nodes;
  std::vector<shared_edge_ptr> edges;
  int time = 0, tot_nodes, tot_edges;
  std::stack<shared_node_ptr> topological;
  std::stack<shared_node_ptr> scc;
  std::vector<std::vector<shared_node_ptr>> scc_list;

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

  void dfs_visit_topological_order(const shared_node_ptr& node) {
    node->set_color(Color::gray);
    node->set_start_visit(++time);

    for (auto& adj : node->get_adj_list()) {
      const auto adjacent = adj.lock();
      if (adjacent->get_color() == Color::white) {
        adjacent->set_predecessor(node);
        dfs_visit_topological_order(adjacent);
      }
    }

    node->set_color(Color::black);
    node->set_end_visit(++time);

    topological.push(node);
  }

  void dfs_visit_sccs(const shared_node_ptr& node) {
    node->set_color(Color::gray);
    node->set_start_visit(++time);

    for (auto& adj : node->get_adj_list()) {
      const auto adjacent = adj.lock();
      if (adjacent->get_color() == Color::white) {
        adjacent->set_predecessor(node);
        dfs_visit_sccs(adjacent);
      }
    }

    node->set_color(Color::black);
    node->set_end_visit(++time);

    scc.push(node);
  }

  const std::shared_ptr<Graph> create_transpose() {
    std::shared_ptr<Graph> transposed = std::make_shared<Graph>();

    for (auto& node : nodes) transposed->insert_node(node);

    for (auto& edge : edges)
      transposed->insert_edge(shared_edge_factory(transposed->get_node(edge->get_destination()->get_data()),
                                                  transposed->get_node(edge->get_source()->get_data()),
                                                  edge->get_weight()));

    return transposed;
  }

  void dfs_compute_scc(const shared_node_ptr& node, std::vector<shared_node_ptr>& current_scc,
                       std::vector<bool>& visited) {
    visited[node->get_data()] = true;
    current_scc.push_back(node);

    for (auto& adj : node->get_adj_list()) {
      const auto adjacent = adj.lock();
      if (!visited[adjacent->get_data()]) dfs_compute_scc(adjacent, current_scc, visited);
    }
  }

  void initialize_single_source(const shared_node_ptr& src) {
    for (auto& node : nodes) {
      node->set_predecessor(nullptr);
      node->set_distance(INT_MAX);
    }

    src->set_predecessor(nullptr);
    src->set_distance(0);
  }

  void relax(const shared_edge_ptr& edge) {
    const auto src = edge->get_source();
    const auto dest = edge->get_destination();

    if (dest->get_distance() > src->get_distance() + edge->get_weight()) {
      dest->set_distance(src->get_distance() + edge->get_weight());
      dest->set_predecessor(src);
    }
  }

public:
  Graph() {}

  Graph(std::ifstream& input) { load(input); }

  void reset() {
    nodes.clear();
    edges.clear();
    time = tot_nodes = tot_edges = 0;
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

    for (int i = 0; i < tot_nodes; i++) insert_node(shared_node_factory(i));

    clear_stream(iss);
    line.clear();

    while (std::getline(input, line)) {
      format_line(line);
      iss.str(line);

      int src_data, dest_data, weight;
      iss >> src_data >> dest_data >> weight;

      const auto src = get_node(src_data);
      const auto dest = get_node(dest_data);

      insert_edge(shared_edge_factory(src, dest, weight));

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

  shared_node_ptr get_node(const int data) {
    if (nodes.empty()) {
      std::cerr << "[get_node ERROR] No node in graph" << std::endl;
      return nullptr;
    }

    for (auto& node : nodes) {
      if (node->get_data() == data) return node;
    }

    return nullptr;
  }

  shared_edge_ptr get_edge(const shared_node_ptr& src, const shared_node_ptr& dest) {
    if (edges.empty()) {
      std::cerr << "[get_edge ERROR] No edge in graph" << std::endl;
      return nullptr;
    }

    for (auto& edge : edges) {
      if (edge->get_source() == src && edge->get_destination() == dest) return edge;
    }

    return nullptr;
  }

  void compute_topological_order() {
    for (auto& node : nodes) {
      node->set_color(Color::white);
      node->set_predecessor(nullptr);
      node->set_start_visit(INT_MAX);
      node->set_end_visit(INT_MAX);
    }

    for (auto& node : nodes) {
      if (node->get_color() == Color::white) dfs_visit_topological_order(node);
    }
  }

  void compute_sccs() {
    for (auto& node : nodes) {
      node->set_color(Color::white);
      node->set_predecessor(nullptr);
      node->set_start_visit(INT_MAX);
      node->set_end_visit(INT_MAX);
    }

    for (auto& node : nodes) {
      if (node->get_color() == Color::white) dfs_visit_sccs(node);
    }

    const auto transposed = create_transpose();
    std::vector<bool> visited(tot_nodes, false);
    while (!scc.empty()) {
      const auto node = scc.top();
      scc.pop();
      const auto transposed_node = transposed->get_node(node->get_data());

      if (!visited[transposed_node->get_data()]) {
        std::vector<shared_node_ptr> current_scc;
        dfs_compute_scc(transposed_node, current_scc, visited);
        scc_list.push_back(current_scc);
      }
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

  void print_sccs(std::ostream& out = std::cout, const std::string message = "Strongly connected components") {
    out << message << std::endl;

    for (int i = 0; i < scc_list.size(); i++) {
      out << "Component #" << i + 1 << std::endl;
      for (auto& node : scc_list[i]) out << "(" << node->get_data() << ")\t";
      out << std::endl;
    }

    out << std::endl;
  }

  void bellman_ford(const shared_node_ptr& src, const shared_node_ptr& dest, std::ostream& out = std::cout) {
    out << "BELLMAN FORD" << std::endl;

    initialize_single_source(src);

    for (int i = 0; i < nodes.size(); i++) {
      for (auto& edge : edges) relax(edge);
    }

    for (auto& edge : edges) {
      const auto src = edge->get_source();
      const auto dest = edge->get_destination();

      if (dest->get_distance() > src->get_distance() + edge->get_weight()) {
        std::cerr << "[bellman_ford ERROR] Negative cycle found" << std::endl;
        return;
      }
    }

    out << "Minimum distance from (" << src->get_data() << ") to (" << dest->get_data() << ") => "
        << dest->get_distance() << std::endl;

    std::vector<shared_node_ptr> path;
    auto current = dest;

    while (current) {
      path.push_back(current);
      current = current->get_predecessor().lock();
    }

    out << "Path from (" << src->get_distance() << ") to (" << dest->get_distance() << ") => ";
    for (int i = path.size() - 1; i > 0; i--) out << "(" << path[i]->get_data() << ") -> ";
    out << "(" << path[0]->get_data() << ")" << std::endl;

    out << std::endl;
  }

  void dijkstra(const shared_node_ptr& src, std::ostream& out = std::cout) {
    out << "DIJKSTRA" << std::endl;

    initialize_single_source(src);
    std::priority_queue<shared_node_ptr, std::vector<shared_node_ptr>, Compare> pq;
    for (auto& node : nodes) pq.push(node);
    std::set<shared_node_ptr> visited;

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

    out << "Minimum distances from (" << src->get_data() << ")" << std::endl;
    for (auto& node : nodes) out << "(" << node->get_data() << ") => " << node->get_distance() << std::endl;
    out << std::endl;
  }
};

#endif