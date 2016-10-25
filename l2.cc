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

// A class to store the states of depth first search.
class KnightBoard {
 public:
  // Coordinate system:
  // ---> x(width)
  // |
  // V
  // y(depth)
  static const int PREV_NOT_EXISTS;
  static const int PREV_UNKNOWN;
  KnightBoard(int depth, int width): depth_(depth), width_(width) {
    if (depth_ <= 0) throw std::runtime_error("Board.depth_ must > 0");
    if (width_ <= 0) throw std::runtime_error("Board.width_ must > 0");
    prev_.resize(depth_ * width_, PREV_UNKNOWN);
  }

  // Given the vertex position, return the id of the vertex.
  inline int posToId(const Vec2& pos) const { return pos.y_ * width_ + pos.x_; }

  // Given the vertex id, return the position of the vertex.
  inline Vec2 idToPos(int id) const { return Vec2(id % width_, id / width_); }

  // Get the previous vertex on the dfs path.
  inline int& prev(int id) { return prev_[id]; }

  inline bool isInside(const Vec2& pos) const {
    return pos.x_ >= 0 && pos.x_ < width_ && pos.y_ >= 0 && pos.y_ < depth_;
  }

 private:
  int depth_, width_;
  std::vector<int> prev_;

}; // class KnightBoard

const int KnightBoard::PREV_NOT_EXISTS = -2;

const int KnightBoard::PREV_UNKNOWN = -1;

// Depth first search for a path to dest
bool dfs(int u, int dest, KnightBoard& board) {
  if (u == dest) return true;

  // foreach neighbor v of u, if it is not visited, recurse
  for (auto move: ChessRule::validKnightMoves) {
    Vec2 newPos = board.idToPos(u) + move;
    if (!board.isInside(newPos)) continue;

    int v = board.posToId(newPos);
    if (board.prev(v) == KnightBoard::PREV_UNKNOWN) {
      board.prev(v) = u;
      if (dfs(v, dest, board)) return true;
      board.prev(v) = KnightBoard::PREV_UNKNOWN;
    }
  }
  return false;
}

// Main logic of level-2
struct MoveResult {
  bool exists_;
  std::vector<Vec2> moves_;
};

MoveResult findMoves(int depth, int width, const Vec2& start, const Vec2& end, bool verbose) {
  MoveResult result;
  KnightBoard board(depth, width);
  int src= board.posToId(start);
  int dest = board.posToId(end);

  board.prev(src) = KnightBoard::PREV_NOT_EXISTS;
  dfs(src, dest, board);

  if (board.prev(dest) == KnightBoard::PREV_UNKNOWN) {
    result.exists_ = false;
    return result;
  }

  result.exists_ = true;

  // Use the result of dfs search, reverse construct the moves from
  // src to dest.
  int cur = dest, prev = board.prev(cur);
  while (prev != KnightBoard::PREV_NOT_EXISTS) {
    Vec2 move = board.idToPos(cur) - board.idToPos(prev);
    result.moves_.push_back(move);
    cur = prev;
    prev = board.prev(cur);
  }
  std::reverse(result.moves_.begin(), result.moves_.end());

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

  MoveResult result = findMoves(depth, width, start, end, verbose);

  if (!result.exists_) {
    std::cout << "NULL\n";
  } else {
    std::cout << std::showpos;
    for (auto move : result.moves_) {
      std::cout << move.x_ << "\t" << move.y_ << "\n";
    }
  }
}
