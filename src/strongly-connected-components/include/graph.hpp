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
  std::stack<shared_node_ptr> stack;
  std::vector<std::vector<shared_node_ptr>> scc_list;

  struct Compare {
    bool operator()(const shared_node_ptr& node1, const shared_node_ptr& node2) {
      return node1->get_distance() > node2->get_distance();
    }
  };

  void format_line(std::string& line) {
    if (line.front() == '<') line = line.substr(1);
    if (line.back() == '>') line.pop_back();
    for (auto& c : line) {
      if (c == ',') c = 32;
    }
  }

  void clear_stream(std::istringstream& iss) {
    iss.clear();
    iss.str("");
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

    stack.push(node);
  }

  void dfs_visit_util(const shared_node_ptr& node, std::vector<bool>& visited,
                      std::vector<shared_node_ptr>& current_scc) {
    visited[node->get_data()] = true;
    current_scc.push_back(node);

    for (auto& adj : node->get_adj_list()) {
      if (!visited[adj->get_data()]) dfs_visit_util(adj, visited, current_scc);
    }
  }

  void initialize_single_source(const shared_node_ptr& src) {
    for (auto& node : nodes) {
      node->set_distance(INT_MAX);
      node->set_predecessor(nullptr);
    }
    src->set_distance(0);
    src->set_predecessor(nullptr);
  }

  void relax(const shared_edge_ptr& edge) {
    if (edge->get_destination()->get_distance() > edge->get_source()->get_distance() + edge->get_weight()) {
      edge->get_destination()->set_distance(edge->get_source()->get_distance() + edge->get_weight());
      edge->get_destination()->set_predecessor(edge->get_source());
    }
  }

  std::shared_ptr<Graph> create_transpose() {
    auto g = std::make_shared<Graph>();

    for (auto& node : nodes) g->insert_node(std::make_shared<Node>(node->get_data()));
    for (auto& edge : edges) {
      auto src_data = edge->get_source()->get_data();
      auto dest_data = edge->get_destination()->get_data();
      auto src = g->get_node(dest_data);
      auto dest = g->get_node(src_data);
      g->insert_edge(std::make_shared<Edge>(src, dest, edge->get_weight()));
    }
    return g;
  }

  void get_strongly_connected_components() {
    for (auto& node : nodes) {
      node->set_color(Color::white);
      node->set_start_visit(INT_MAX);
      node->set_end_visit(INT_MAX);
      node->set_predecessor(nullptr);
    }

    for (auto& node : nodes) {
      if (node->get_color() == Color::white) dfs_visit(node);
    }

    auto transposed_graph = create_transpose();
    scc_list.clear();
    std::vector<bool> visited(tot_nodes, false);

    while (!stack.empty()) {
      auto node = stack.top();
      stack.pop();

      if (!visited[node->get_data()]) {
        std::vector<shared_node_ptr> current_scc;
        dfs_visit_util(transposed_graph->get_node(node->get_data()), visited, current_scc);
        scc_list.push_back(current_scc);
      }
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
    clear_stream(iss);
    line.clear();

    for (int i = 0; i < tot_nodes; i++) insert_node(shared_node_ptr(new Node(i)));

    while (std::getline(input, line)) {
      int src_data, dest_data, weight;
      format_line(line);
      iss.str(line);

      iss >> src_data >> dest_data >> weight;
      auto src = get_node(src_data), dest = get_node(dest_data);

      if (src && dest) insert_edge(std::make_shared<Edge>(src, dest, weight));

      clear_stream(iss);
      line.clear();
    }
  }

  void insert_node(const shared_node_ptr& node) {
    nodes.push_back(node);
    if (nodes.size() > tot_nodes) tot_nodes = nodes.size();
  }

  shared_node_ptr get_node(int data) {
    if (nodes.empty()) {
      std::cerr << "[get_node ERROR] No nodes in graph" << std::endl;
      return nullptr;
    }

    for (auto& node : nodes) {
      if (node->get_data() == data) return node;
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
      if (edge->get_source() == src && edge->get_destination() == dest) return edge;
    }

    std::cerr << "[get_edge ERROR] Edge (" << src->get_data() << ") -> (" << dest->get_data() << ") not found"
              << std::endl;
    return nullptr;
  }

  void insert_edge(const shared_edge_ptr& edge) {
    edges.push_back(edge);
    edge->get_source()->add_adjacent(edge->get_destination());
    if (edges.size() > tot_edges) tot_edges = edges.size();
  }

  void dfs() {
    for (auto& node : nodes) {
      node->set_color(Color::white);
      node->set_start_visit(INT_MAX);
      node->set_end_visit(INT_MAX);
      node->set_predecessor(nullptr);
    }

    for (auto& node : nodes) {
      if (node->get_color() == Color::white) dfs_visit(node);
    }
  }

  void print(std::ostream& out = std::cout, std::string message = "Graph") {
    out << message << std::endl << "Nodes" << std::endl;
    for (auto& node : nodes) node->print(out);
    out << "Edges" << std::endl;
    for (auto& edge : edges) edge->print(out);
    out << std::endl;
  }

  void topological_order(std::ostream& out = std::cout) {
    out << "Topological order" << std::endl;
    while (!stack.empty()) {
      auto node = stack.top();
      stack.pop();

      node->print(out);
    }

    out << std::endl;
  }

  void bellman_ford(const shared_node_ptr& src, const shared_node_ptr& dest, std::ostream& out = std::cout) {
    std::vector<shared_node_ptr> path;

    initialize_single_source(src);

    for (int i = 0; i < tot_nodes; i++) {
      for (auto& edge : edges) relax(edge);
    }

    if (dest->get_distance() == INT_MAX) {
      out << "[bellman_ford ERROR] Destination node (" << dest->get_data() << ") is unreachable from ("
          << src->get_data() << ")" << std::endl;
      return;
    }

    for (auto& edge : edges) {
      int src_distance = edge->get_source()->get_distance();
      int dest_distance = edge->get_destination()->get_distance();
      if (src_distance != INT_MAX && dest_distance > src_distance + edge->get_weight()) {
        out << "[bellman_ford ERROR] Graph contains a negative-weight cycle" << std::endl;
        return;
      }
    }

    shared_node_ptr current = dest;
    while (current) {
      path.push_back(current);
      current = current->get_predecessor().lock();
    }

    out << "BELLMAN FORD" << std::endl
        << "Minimum distance (" << src->get_data() << ") -> (" << dest->get_data() << ") => " << dest->get_distance()
        << std::endl;
    out << "Minimum path (" << src->get_data() << ") -> (" << dest->get_data() << ") => ";
    for (int i = path.size() - 1; i > 0; i--) out << "(" << path[i]->get_data() << ") -> ";
    out << "(" << path[0]->get_data() << ")" << std::endl;
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

    out << std::endl << "DIJKSTRA" << std::endl << "Minimum distances from (" << src->get_data() << ")" << std::endl;
    for (auto& node : nodes) out << "(" << node->get_data() << ") => distance: " << node->get_distance() << std::endl;
    out << std::endl;
  }

  void print_scc(std::ostream& out = std::cout, std::string message = "Strongly connected components") {
    get_strongly_connected_components();
    out << message << std::endl;

    int count = 0;
    for (auto& scc : scc_list) {
      out << "Component #" << ++count << std::endl;
      for (auto& node : scc) {
        std::cout << "(" << node->get_data() << ")\n";
      }
      out << std::endl;
    }
  }
};

#endif