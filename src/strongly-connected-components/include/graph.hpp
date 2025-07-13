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
  std::vector<std::vector<shared_node_ptr>> scc_list;
  std::stack<shared_node_ptr> stack_topological;
  std::stack<shared_node_ptr> stack_sccs_order;

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

  void dfs_visit(const shared_node_ptr& node) {
    node->set_color(Color::gray);
    node->set_start_visit(++time);

    for (auto& adj : node->get_adj_list()) {
      if (adj->get_color() == Color::white) {
        adj->set_predecessor(node);
        dfs_visit(adj);
      }
    }

    node->set_end_visit(++time);
    node->set_color(Color::black);
    stack_topological.push(node);
    stack_sccs_order.push(node);
  }

  std::shared_ptr<Graph> create_transpose() {
    std::shared_ptr<Graph> graph = std::make_shared<Graph>();

    for (auto& node : nodes) graph->insert_node(node_factory(node->get_data()));

    for (auto& edge : edges) {
      auto src = graph->get_node(edge->get_destination()->get_data());
      auto dest = graph->get_node(edge->get_source()->get_data());
      graph->insert_edge(edge_factory(src, dest, edge->get_weight()));
    }

    return graph;
  }

  void dfs_util(const shared_node_ptr& node, std::vector<shared_node_ptr>& scc, std::vector<bool>& visited) {
    visited[node->get_data()] = true;
    scc.push_back(node);

    for (auto& adj : node->get_adj_list()) {
      if (!visited[adj->get_data()]) dfs_util(adj, scc, visited);
    }
  }

  void initialize_single_source(const shared_node_ptr& src) {
    for (auto& node : nodes) {
      node->set_predecessor(nullptr);
      node->set_distance(INT_MAX);
    }

    src->set_distance(0);
    src->set_predecessor(nullptr);
  }

  void relax(const shared_edge_ptr& edge) {
    auto src = edge->get_source();
    auto dest = edge->get_destination();
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
    scc_list.clear();
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

    for (int i = 0; i < tot_nodes; i++) insert_node(node_factory(i));

    while (std::getline(input, line)) {
      int src_data, dest_data, weight;
      format_line(line);
      iss.str(line);

      iss >> src_data >> dest_data >> weight;
      auto src = get_node(src_data), dest = get_node(dest_data);

      if (src && dest) insert_edge(edge_factory(src, dest, weight));

      line.clear();
      clear_stream(iss);
    }
  }

  void insert_node(const shared_node_ptr& node) {
    nodes.push_back(node);
    if (nodes.size() > tot_nodes) tot_nodes = nodes.size();
  }

  void insert_edge(const shared_edge_ptr& edge) {
    edge->get_source()->add_adjacent(edge->get_destination());
    edges.push_back(edge);
    if (edges.size() > tot_edges) tot_edges = edges.size();
  }

  shared_node_ptr get_node(const int data) {
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
      if (node->get_color() == Color::white) dfs_visit(node);
    }

    auto transposed = create_transpose();
    std::vector<bool> visited(tot_nodes, false);

    while (!stack_sccs_order.empty()) {
      auto node = stack_sccs_order.top();
      stack_sccs_order.pop();

      auto transposed_node = transposed->get_node(node->get_data());

      if (!visited[transposed_node->get_data()]) {
        std::vector<shared_node_ptr> scc;
        dfs_util(transposed_node, scc, visited);
        scc_list.push_back(scc);
      }
    }
  }

  void dfs() {
    for (auto& node : nodes) node->reset();

    for (auto& node : nodes) {
      if (node->get_color() == Color::white) dfs_visit(node);
    }
  }

  void print_sccs(std::ostream& out = std::cout, const std::string message = "Strongly connected components") {
    out << message << std::endl;
    for (int i = 0; i < scc_list.size(); i++) {
      out << "Component #" << i + 1 << std::endl;
      for (auto& node : scc_list[i]) out << "(" << node->get_data() << ")\t";
      out << std::endl;
    }
  }

  void topological_order(std::ostream& out = std::cout, const std::string message = "Topological order") {
    out << message << std::endl;
    while (!stack_topological.empty()) {
      auto node = stack_topological.top();
      stack_topological.pop();
      node->print(out);
    }
    out << std::endl;
  }

  void print(std::ostream& out = std::cout, const std::string message = "Graph") {
    out << message << std::endl << "Nodes" << std::endl;
    for (auto& node : nodes) node->print(out);
    out << "Edges" << std::endl;
    for (auto& edge : edges) edge->print(out);
    out << std::endl;
  }

  void bellman_ford(const shared_node_ptr& src, const shared_node_ptr& dest, std::ostream& out = std::cout) {
    initialize_single_source(src);

    for (int i = 0; i < tot_nodes; i++) {
      for (auto& edge : edges) relax(edge);
    }

    for (auto& edge : edges) {
      auto src = edge->get_source();
      auto dest = edge->get_destination();

      if (dest->get_distance() > src->get_distance() + edge->get_weight()) {
        std::cerr << "[bellman_ford ERROR] Negative edge found" << std::endl;
        return;
      }
    }

    std::vector<shared_node_ptr> path;
    auto current = dest;
    while (current) {
      path.push_back(current);
      current = current->get_predecessor().lock();
    }

    out << "BELLMAN FORD" << std::endl
        << "Minimum path distance from (" << src->get_data() << ") to (" << dest->get_data() << ") => "
        << dest->get_distance() << std::endl;
    out << "Path" << std::endl;
    for (int i = path.size() - 1; i > 0; i--) out << "(" << path[i]->get_data() << ")\t";
    out << "(" << path[0]->get_data() << ")" << std::endl;

    out << std::endl;
  }

  void dijkstra(const shared_node_ptr& src, std::ostream& out = std::cout) {
    initialize_single_source(src);

    std::priority_queue<shared_node_ptr, std::vector<shared_node_ptr>, Compare> pq;
    for (auto& node : nodes) pq.push(node);

    std::set<shared_node_ptr> visited;
    while (!pq.empty()) {
      auto node = pq.top();
      pq.pop();

      if (visited.find(node) != visited.end()) continue;
      visited.insert(node);

      for (auto& adj : node->get_adj_list()) {
        auto edge = get_edge(node, adj);

        if (edge) {
          relax(edge);
          if (visited.find(adj) == visited.end()) pq.push(adj);
        }
      }
    }

    out << "DIJKSTRA" << std::endl << "Minimum paths distance from (" << src->get_data() << ")" << std::endl;
    for (auto& node : nodes) out << "(" << node->get_data() << ") => " << node->get_distance() << std::endl;
    out << std::endl;
  }
};

#endif