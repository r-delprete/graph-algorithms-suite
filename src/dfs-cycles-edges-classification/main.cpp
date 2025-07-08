#include "./include/graph.hpp"

using namespace std;

int main(int argc, char **argv) {
  ifstream input("input.txt");
  ifstream input2("input2.txt");
  ofstream output("output.txt");

  Graph graph(input);

  graph.dfs();
  graph.print("Graph", output);

  graph.load(input2);
  graph.print("\nGraph with input2 - pre DFS", output);
  graph.dfs();
  graph.print("\nGraph with input2 - post DFS", output);

  input.close();
  input2.close();
  output.close();

  return 0;
}