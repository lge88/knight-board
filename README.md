## Level 1

- Build executable: `make l1`.
- Run tests: `make t1`

Input: read input from stdin in following format:

```
<depth> <width> <startX> <startY> <verbose>
<x1> <y1>
<x2> <y2>
...

```
Where:
- depth and width is the dimension of the board.
- startX, startY is the starting position of the knight.
- If verbose is 1, program output intermediate states to
  terminal. Otherwise output nothing.
- x1, y1, x2, y2, ... is the move sequences to be valided.

Output: If verbose is 0 output nothing. Exit with code 0 if moves
sequeces are valid, 1 if move sequences are invalid.

## Level 2

Solved using Depth-First-Search. Algorithm stops as soon as it finds a
valid path.

- Build executable: `make l2`.
- Run tests: `make t2`

Input: read input from stdin in following format:

```
<depth> <width> <startX> <startY> <endX> <endY>
```
Where:
- depth and width is the dimension of the board.
- startX, startY is the starting position of the knight.
- endX, endY is the end position of the knight.

Output:
```
<x1> <y1>
<x2> <y2>
...
```
Where: (x1, y1), (x2, y2), ... are move sequence.

## Level 3

It is a shortest path on undirected unweighted graph problem. Solved
using Bread-First-Search.

- Build executable: `make l2`.
- Run tests: `make t2`

Input and output format is identical to the one described in level 2.

## Level 4

It is a shortest path on weighted undirected graph problem. Solved
using Dijkstra's algorithm.

- Build executable: `make l4`.
- Run tests: `make t4`

Input format:
```
<startX> <startY> <endX> <endY>
<map>
```

where:
- {start,end}{X,Y} is the start and end position of the path.
- `map` is the same as the format in the problem statement, something like:
```
. T T .
. R W .
. L B .
. . . .
```

Output:
```
<distance>
<x1> <y1>
<x2> <y2>
...
```
where:
- distance is the final shortest path of the solution.
- x1, y1, x2, y2, ... is the move sequences of the shortest path.

## Level 5

It is a longest path in undirected cyclic graph problem. The problem
is NP-hard, no efficient solution are known.

Here presents a Depth-First-Search algorithm, which enumerate all
possible search pathes and keep track of the longest one. The input
and output format are identical to to one described in level 2. On my
laptop, the problem becomes intractable when board length is larger or
greater than 6.
