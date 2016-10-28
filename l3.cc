#include <vector>
#include <queue>
#include <sstream>
#include <iostream>
#include <ios>

// Simple vector class representing position on the board as well as
// movement.
struct Vec2 {
  int x_, y_;
  Vec2(int x, int y): x_(x), y_(y) {}
  Vec2(): x_(0), y_(0) {}
};

// Comparator, used in std::find
bool operator==(const Vec2& v1, const Vec2& v2) {
  return v1.x_ == v2.x_ && v1.y_ == v2.y_;
}

// Sum
Vec2 operator+(const Vec2& v1, const Vec2& v2) {
  return Vec2(v1.x_ + v2.x_, v1.y_ + v2.y_);
}

// Substract
Vec2 operator-(const Vec2& v1, const Vec2& v2) {
  return Vec2(v1.x_ - v2.x_, v1.y_ - v2.y_);
}

// A class encapsulate the rule the a knight must follow. At any
// point, a knight might have 8 potential moves.
class ChessRule {
 public:
  // Since the set of valid moves is very small (only 8 elements), use
  // std::vector is efficient even for linear search.
  static const std::vector<Vec2> validKnightMoves;

  static bool isValidKnightMove(const Vec2& move) {
    return std::find(validKnightMoves.begin(), validKnightMoves.end(), move) !=
        validKnightMoves.end();
  }
}; // class ChessRule

const std::vector<Vec2> ChessRule::validKnightMoves = {
  { +1, +2 },
  { +2, +1 },
  { +2, -1 },
  { +1, -2 },
  { -1, +2 },
  { -2, +1 },
  { -2, -1 },
  { -1, -2 }
};

// A helper class to store the vertex states of breadth first search.
class Board {
 public:
  // Coordinate system:
  // ---> x(width)
  // |
  // V
  // y(depth)
  Board(int depth, int width): depth_(depth), width_(width) {
    if (depth_ <= 0) throw std::runtime_error("Board.depth_ must > 0");
    if (width_ <= 0) throw std::runtime_error("Board.width_ must > 0");
    reset();
  }

  // Reset to clean state
  inline void reset() {
    const int n = depth_ * width_;
    visited_.resize(n, false);
    prev_.resize(n, -1);
  }

  // Return whether the vertex u is visited or not.
  inline bool getVisited(const Vec2& u) const {
    return visited_[posToIndex(u)];
  }

  // Set whether the vertex u is visited or not.
  inline void setVisited(const Vec2& u, bool visited) {
    visited_[posToIndex(u)] = visited;
  }

  // Return the prev vertex on the bfs path for vertex u. User is
  // responsible to call hasPrev to check whether u has prev vertex
  // before calling this.
  inline Vec2 getPrev(const Vec2& u) const {
    return indexToPos(prev_[posToIndex(u)]);
  }

  // Return true if vertex u has prev vertex on the bfs path.
  inline bool hasPrev(const Vec2& u) const {
    return prev_[posToIndex(u)] >= 0;
  }

  // Set the prev vertex on the bfs path to u for vertex v
  inline void setPrev(const Vec2& v, const Vec2& u) {
    prev_[posToIndex(v)] = posToIndex(u);
  }

  inline bool isInside(const Vec2& pos) const {
    return pos.x_ >= 0 && pos.x_ < width_ && pos.y_ >= 0 && pos.y_ < depth_;
  }

 private:
  int depth_, width_;
  std::vector<bool> visited_;
  std::vector<int> prev_;

  // map 2d coordinate on the board to the index in states array.
  inline int posToIndex(const Vec2& u) const { return u.y_ * width_ + u.x_; }

  // map the index in states array to the 2d coordinate on the board
  inline Vec2 indexToPos(int i) const { return Vec2(i % width_, i / width_); }

}; // class Board

// Bread first search for a shortest path from u to dest.
bool bfs(const Vec2& start, const Vec2& dest, Board& board, std::vector<Vec2>& moves) {
  std::queue<Vec2> q;
  q.push(start);

  board.reset();
  while (!q.empty()) {
    Vec2 u = q.front();
    q.pop();

    if (u == dest) {
      // Output the move sequence by reverse tracing the prev
      // vertices.
      Vec2 cur = dest;
      while (board.hasPrev(cur)) {
        Vec2 prev = board.getPrev(cur);
        Vec2 move = cur - prev;
        moves.push_back(move);
        cur = prev;
      }
      std::reverse(moves.begin(), moves.end());
      return true;
    }

    // Foeach neighbor v of u, if not visited, put it on the queue.
    for (auto move: ChessRule::validKnightMoves) {
      Vec2 v = u + move;
      if (board.isInside(v) && !board.getVisited(v)) {
        q.push(v);
        board.setPrev(v, u);
      }
    }

    board.setVisited(u, true);
  }

  return false;
}

struct MoveResult {
  bool found_;
  std::vector<Vec2> moves_;
  MoveResult(): found_(false), moves_(0) {}
};

MoveResult findMoves(int depth, int width, const Vec2& start, const Vec2& end) {
  MoveResult result;
  Board board(depth, width);
  result.found_ = bfs(start, end, board, result.moves_);
  return result;
}

int main(int argc, char* argv[]) {
  std::string line;
  std::getline(std::cin, line);
  std::stringstream iss(line);
  int depth, width;
  Vec2 start, end;
  bool verbose;
  iss >> depth >> width;
  iss >> start.x_ >> start.y_;
  iss >> end.x_ >> end.y_;

  MoveResult result = findMoves(depth, width, start, end);

  if (!result.found_) {
    std::cout << "NULL\n";
  } else {
    std::cout << std::showpos;
    for (auto move : result.moves_) {
      std::cout << move.x_ << "\t" << move.y_ << "\n";
    }
  }
}
