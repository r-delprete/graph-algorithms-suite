#include "./include/graph.hpp"

using namespace std;

int main() {
  ifstream input("input.txt");
  ifstream input2("input2.txt");
  ifstream scc_input("scc_input.txt");
  ofstream output("output.txt");

  Graph graph(input2);

  graph.print();
  graph.print(output);
  graph.compute_topological_order();
  graph.print_topological_order(output);
  graph.compute_sccs();
  graph.print_sccs();
  graph.print_sccs(output);

  auto src = graph.get_node(0);
  auto dest = graph.get_node(4);

  if (src && dest) {
    graph.bellman_ford(src, dest);
    graph.bellman_ford(src, dest, output);

    graph.dijkstra(src);
    graph.dijkstra(src, output);
  }

  input.close();
  input2.close();
  output.close();

  return 0;
}