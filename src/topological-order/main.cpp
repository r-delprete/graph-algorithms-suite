#include "./include/graph.hpp"

using namespace std;

int main() {
  ifstream input("input.txt");
  ifstream input2("input2.txt");
  ofstream output("output.txt");

  Graph graph(input2);

  graph.print();
  graph.print(output);
  graph.dfs();
  graph.print(cout, "Post DFS");
  graph.print(output, "Post DFS");
  // graph.topological_order();
  graph.topological_order(output);

  graph.hamiltonian_cycle(0, output);
  graph.hamiltonian_cycle(0);

  auto src = graph.get_node(0);
  auto dest = graph.get_node(4);

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