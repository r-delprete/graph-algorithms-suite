#include "./include/graph.hpp"

using namespace std;

int main() {
  ifstream input("input2.txt");
  ofstream output("output.txt");

  Graph graph(input);
  graph.print();

  auto src = graph.get_node(0);
  if (src) {
    graph.bfs(src);
    graph.print(cout, "After BFS visit");
    graph.print(output, "After BFS visit");

    graph.prim(src);
    graph.print_mst();
    graph.print_mst(output);
  }

  if (graph.is_complete_binary())
    cout << "BF tree is complete binary" << endl;
  else
    cout << "BF tree is not complete binary" << endl;

  if (graph.is_binary())
    cout << "MST is a binary tree" << endl;
  else
    cout << "MST is not a binary tree" << endl;

  input.close();
  output.close();

  return 0;
}