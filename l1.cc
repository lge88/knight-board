#include <vector>
#include <sstream>
#include <iostream>

struct Config {
  int depth_, width_;
  int startX_, startY_;
  int verbose_;
  Config() :
      depth_(8), width_(8), startX_(1), startY_(2), verbose_(0)
  {}
};

// Simple vector class representing position on the board as well as
// movement.
struct Vec2 {
  int x_, y_;
  Vec2(int x, int y): x_(x), y_(y) {}
  Vec2& operator+=(const Vec2& other) {
    x_ += other.x_;
    y_ += other.y_;
    return *this;
  }
};

// Comparator, used in std::find
bool operator==(const Vec2& v1, const Vec2& v2) {
  return v1.x_ == v2.x_ && v1.y_ == v2.y_;
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

// An immutable class provides query functions: getWidth, getDepth and isInside.
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
  }

  int getDepth() const { return depth_; }

  int getWidth() const { return width_; }

  bool isInside(const Vec2& pos) const {
    return pos.x_ >= 0 && pos.x_ < width_ && pos.y_ >= 0 && pos.y_ < depth_;
  }

 private:
  int depth_, width_;

}; // class Board

// Some helper function to print the program states.
void printInvalidInitialPos(const Board& board, const Vec2& pos, std::ostream& to) {
  to << "Initial position (" << pos.x_ << ", " << pos.y_ << ") is not inside the ";
  to << board.getDepth() << "x" << board.getWidth() << " board.\n";
}

void printConfig(const Config& config, std::ostream& to) {
  to << "depth_: " << config.depth_ << "\n";
  to << "width_: " << config.width_ << "\n";
  to << "startX_: " << config.startX_ << "\n";
  to << "startY_: " << config.startY_ << "\n";
  to << "verbose_: " << config.verbose_ << "\n";
}

void printKightBoard(const Board& board, const Vec2& knightPos, std::ostream& to) {
  const int depth = board.getDepth(), width = board.getWidth();
  for (int y = 0; y < depth; ++y) {
    for (int x = 0; x < width; ++x) {
      to << (knightPos.x_ == x && knightPos.y_ == y ? "K " : ". ");
    }
    to << "\n";
  }
}

void printInvalidMove(const Vec2& move, std::ostream& to) {
  to << "Move (" << move.x_ << ", " << move.y_ << ") is not a valid knight move.\n";
}

void printValidMove(const Vec2& move, std::ostream& to) {
  to << "Apply knight move (" << move.x_ << ", " << move.y_ << ").\n";
}

void printNewPosOutsideBoard(const Vec2& move, const Vec2& newPos, std::ostream& to) {
  to << "After applying knight move (" << move.x_ << ", " << move.y_ << "), ";
  to << "new position (" << newPos.x_ << ", " << newPos.y_ << ") is outside the board.\n";
}

// Read config from istream.
Config readConfig(std::istream& from) {
  Config config;
  std::string line;
  std::getline(from, line);
  std::stringstream iss(line);
  iss >> config.depth_ >> config.width_;
  iss >> config.startX_ >> config.startY_;
  iss >> config.verbose_;
  return config;
}

// Read move sequences from istream
std::vector<Vec2> readMoves(std::istream& from) {
  // Since c++11 has move semantics, no longer need to use signature
  // void readMoves(std::istream& from, std::vector<Vec2>& moves) any more.
  std::vector<Vec2> moves;
  std::string line;
  while (std::getline(from, line)) {
    std::stringstream iss(line);
    int x = 0, y = 0;
    iss >> x >> y;
    moves.push_back(Vec2(x, y));
  }
  return moves;
}

// Main logic for KnightBoard level-1
bool validateMoves(const std::vector<Vec2>& moves, const Config& config) {
  Board board(config.depth_, config.width_);
  Vec2 knightPos(config.startX_, config.startY_);
  const bool verbose = config.verbose_;

  if (verbose) printConfig(config, std::cout);

  // Check the initial position of the knight.
  if (!board.isInside(knightPos)) {
    if (verbose) printInvalidInitialPos(board, knightPos, std::cout);
    return false;
  }

  if (verbose) printKightBoard(board, knightPos, std::cout);

  for (auto move : moves) {
    // Check where the move is a valid knight move based on chess rule
    if (!ChessRule::isValidKnightMove(move)) {
      if (verbose) printInvalidMove(move, std::cout);
      return false;
    }

    // Update knight position
    knightPos += move;

    // Check whether knight is still inside the board.
    if (!board.isInside(knightPos)) {
      if (verbose) printNewPosOutsideBoard(move, knightPos, std::cout);
      return false;
    }

    if (verbose) {
      printValidMove(move, std::cout);
      printKightBoard(board, knightPos, std::cout);
    }
  }
  return true;
}

int main(int argc, char* argv[]) {
  Config config = readConfig(std::cin);
  std::vector<Vec2> moves = readMoves(std::cin);

  if (validateMoves(moves, config)) {
    return 0;
  } else{
    return 1;
  }
}
