#include <vector>
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

// A helper class to store the vertex states of depth first search.
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
    visited_.resize(depth_ * width_, false);
  }

  // Return whether the vertex is visited or not.
  inline bool getVisited(const Vec2& pos) const {
    return visited_[pos.y_ * width_ + pos.x_];
  }

  // Set whether the vertex is visited or not.
  inline void setVisited(const Vec2& pos, bool visited) {
    visited_[pos.y_ * width_ + pos.x_] = visited;
  }

  inline bool isInside(const Vec2& pos) const {
    return pos.x_ >= 0 && pos.x_ < width_ && pos.y_ >= 0 && pos.y_ < depth_;
  }

 private:
  int depth_, width_;
  std::vector<bool> visited_;

}; // class Board

// Depth first search for a path from u to dest. Return true if found
// a path. board stores the visited state, movesSofar store the
// sequence of moves from start to current vertex u.
bool dfs(const Vec2& u, const Vec2& dest, Board& board, std::vector<Vec2>& movesSofar) {
  if (u == dest) return true;

  board.setVisited(u, true);
  // foreach neighbor v of u, if it is not visited, recursive dfs.
  for (auto move: ChessRule::validKnightMoves) {
    Vec2 v = u + move;
    if (!board.isInside(v)) continue;

    if (!board.getVisited(v)) {
      movesSofar.push_back(move);
      if (dfs(v, dest, board, movesSofar)) return true;
      movesSofar.pop_back();
    }
  }
  board.setVisited(u, false);
  return false;
}

// Main logic of level-2
struct MoveResult {
  bool found_;
  std::vector<Vec2> moves_;
  MoveResult(): found_(false), moves_(0) {}
};

MoveResult findMoves(int depth, int width, const Vec2& start, const Vec2& end) {
  MoveResult result;
  Board board(depth, width);
  result.found_ = dfs(start, end, board, result.moves_);
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
