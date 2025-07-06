# BFS (Breadth-First-Search) visit algorithm

## Input parameters

- `graph`: graph from which apply the visit
- `source`: source node to start the visit

## Pseudo-code

```
bfs(graph, source):
    for node in (graph.nodes - source):
        node.color = white
        node.d = ∞
        node.predecessor = NIL

    source.color = gray
    source.d = 0
    source.predecessor = NIL

    queue = ∅
    queue.push(source)

    while queue != ∅:
        node = queue.pop()

        for adj_node in node.adj:
            if adj_node.color == white:
                adj_node.color = gray
                adj_node.d = node.d + 1
                adj_node.predecessor = node

                queue.push(adj_node)

        node.color = black
```
