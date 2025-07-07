#include "./include/graph.hpp"

using namespace std;

int main() {
  ifstream input("input.txt");
  ifstream input2("input2.txt");
  ofstream output("output.txt");

  Graph graph(input);

  graph.print();
  graph.print("Graph", output);
  graph.dfs();
  graph.print_topological_order();
  // graph.print_topological_order(output);

  graph.hamiltonian_cycle(0, output);
  graph.hamiltonian_cycle(0);

  Node* src = graph.get_node(0);
  Node* dest = graph.get_node(3);

  if (src && dest) {
    graph.bellman_ford(src, dest);
    graph.bellman_ford(src, dest, output);
  }

  src = graph.get_node(0);

  if (src) {
    graph.dijkstra(src);
    graph.dijkstra(src, output);
  }

  input.close();
  input2.close();
  output.close();

  return 0;
}