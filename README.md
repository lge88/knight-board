## Level 1

- Build executable: `make l1`.
- Run tests: `make t1`
- Usage:
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
- x1, y1, x2, y2 is the move sequences to be valided.

Output: If verbose is 0 output nothing. Exit with code 0 if moves
sequeces are valid, 1 if move sequences are invalid.
