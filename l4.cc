#include <vector>
#include <set>
#include <sstream>
#include <iostream>
#include <ios>
#include <iomanip>

// Simple vector class representing position on the board as well as
// movement.
struct Vec2 {
  int x_, y_;
  Vec2(int x, int y): x_(x), y_(y) {}
  Vec2(): x_(0), y_(0) {}
};

std::ostream& operator<<(std::ostream& to, const Vec2& v) {
  return to << "(" << v.x_ << ", " << v.y_ << ")";
}

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

// A class for graph queries.
class KnightMap {
 public:
  enum CellType { DEFAULT = 0, WATER, ROCK, BARRIER, TELEPORT, LAVA };

  KnightMap(int depth, int width): depth_(depth), width_(width) {
    reset();
  }

  KnightMap(): KnightMap(0, 0) {}

  // Read from istream
  friend std::istream& operator>>(std::istream& from, KnightMap& map) {
    std::ostringstream oss;
    std::string line;
    std::vector<CellType> cells;
    std::set<int> teleports;
    int width = -1, depth = 0;
    while (std::getline(std::cin, line)) {
      std::stringstream iss(line);
      char c;
      int widthThisRow = 0;
      while (iss >> c) {
        switch (c) {
          case '.':
            cells.push_back(DEFAULT); break;
          case 'W':
            cells.push_back(WATER); break;
          case 'R':
            cells.push_back(ROCK); break;
          case 'B':
            cells.push_back(BARRIER); break;
          case 'T':
            cells.push_back(TELEPORT);
            teleports.insert(cells.size() - 1);
            break;
          case 'L':
            cells.push_back(LAVA); break;
          default:
            oss.str("");
            oss << "Unknown cell " << c << ".";
            throw std::runtime_error(oss.str());
        }
        ++widthThisRow;
      }

      // First row, do not check width, set it.
      if (width <= 0) width = widthThisRow;

      if (widthThisRow != width) {
        oss.str("");
        oss << "At row " << depth << ", width is " << widthThisRow << ", ";
        oss << "but previous row width is " << width << ".";
        throw std::runtime_error(oss.str());
      }

      ++depth;
    }
    map.width_ = width;
    map.depth_ = depth;
    std::swap(map.cells_, cells);
    std::swap(map.teleports_, teleports);
    return from;
  }

  friend std::ostream& operator<<(std::ostream& to, const KnightMap& map) {
    for (int y = 0, depth = map.getDepth(); y < depth; ++y) {
      for (int x = 0, width = map.getWidth(); x < width; ++x) {
        const Vec2 u(x, y);
        const CellType type(map.getCellType(u));
        switch (type) {
          case DEFAULT: to << ". "; break;
          case WATER: to << "W "; break;
          case ROCK: to << "R "; break;
          case BARRIER: to << "B "; break;
          case TELEPORT: to << "T "; break;
          case LAVA: to << "L "; break;
        }
      }
      to << "\n";
    }
    return to;
  }

  inline void reset() {
    cells_.resize(depth_ * width_, DEFAULT);
  }

  inline int getDepth() const { return depth_; }

  inline int getWidth() const { return width_; }

  // Return true if the pos is inside the map
  inline bool isInside(const Vec2& pos) const {
    return pos.x_ >= 0 && pos.x_ < width_ && pos.y_ >= 0 && pos.y_ < depth_;
  }

  inline CellType getCellType(const Vec2& u) const {
    return cells_[posToIndex(u)];
  }

  inline void setCellType(const Vec2& u, const CellType& type) {
    const int index = posToIndex(u);
    cells_[index] = type;
    if (type == TELEPORT) {
      teleports_.insert(index);
    } else {
      teleports_.erase(index);
    }
  }

  // Return a list of vertices that can be reached from vertex u.
  std::vector<Vec2> adj(const Vec2& u) const {
    std::vector<Vec2> neighbors;

    // Regular valid chess knight moves
    for (auto move: ChessRule::validKnightMoves) {
      Vec2 v = u + move;

      // Can not go outside the map
      if (!isInside(v)) continue;

      // Can not land on ROCK
      if (getCellType(v) == ROCK) continue;

      // Can not cross or land on BARRIER
      if (getCellType(v) == BARRIER || isCrossingBarrier(u, move)) continue;

      neighbors.push_back(v);
    }

    // teleports
    std::set<int>::const_iterator it = teleports_.find(posToIndex(u));
    if (it != teleports_.end()) {
      for (auto i : teleports_) {
        if (*it != i) neighbors.push_back(indexToPos(i));
      }
    }

    // std::cout << "adj of " << u << ": ";
    // for (auto n: neighbors) std::cout << n << " ";
    // std::cout << "\n";

    return neighbors;
  }

  // Return the weight for edge (u, v)
  inline int edgeWeight(const Vec2& u, const Vec2& v) const {
    const CellType type = getCellType(v);
    const int NA = 1000;
    switch (type) {
      case WATER: return 2;
      case ROCK: return NA;
      case BARRIER: return NA;
      case TELEPORT: return 0;
      case LAVA: return 5;
      default:
        break;
    }
    return 1;
  }

 protected:
  int depth_, width_;
  std::vector<CellType> cells_;
  std::set<int> teleports_;

  // map 2d coordinate on the board to the index in states array.
  inline int posToIndex(const Vec2& u) const { return u.y_ * width_ + u.x_; }

  // map the index in states array to the 2d coordinate on the board
  inline Vec2 indexToPos(int i) const { return Vec2(i % width_, i / width_); }

  // Return true if the move from u crossed a barrier. Prerequisite:
  // move obey chess rule for knight and v = u + move is still inside
  // map.

  // The following are the invalid cases for move = (2, 1)
  // 1)  u B .    2)  u . .   3)  u B .
  //     . . x        . B x       . B x
  inline bool isCrossingBarrier(const Vec2& u, const Vec2& move) const {
    if (move.x_ == 2 || move.x_ == -2) {
      Vec2 mid1(u.x_ + move.x_ / 2, u.y_), mid2(u.x_ + move.x_ / 2, u.y_ + move.y_);
      if (getCellType(mid1) == BARRIER || getCellType(mid1) == BARRIER) return true;
    } else if (move.y_ == 2 || move.y_ == -2) {
      Vec2 mid1(u.x_, u.y_ + move.y_ / 2), mid2(u.x_ + move.x_, u.y_ + move.y_ / 2);
      if (getCellType(mid1) == BARRIER || getCellType(mid1) == BARRIER) return true;
    }
    return false;
  }

};

// A helper class to store the vertex states during search.
class StateBoard {
 public:
  // Coordinate system:
  // ---> x(width)
  // |
  // V
  // y(depth)
  StateBoard(int depth, int width): depth_(depth), width_(width) {
    if (depth_ <= 0) throw std::runtime_error("StateBoard.depth_ must > 0");
    if (width_ <= 0) throw std::runtime_error("StateBoard.width_ must > 0");
    reset();
  }

  // Reset to clean state
  inline void reset() {
    const int n = depth_ * width_;
    prev_.resize(n, -1);
    dist_.resize(n, -1);
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

  // Return the distance of vertex u.
  inline int getDist(const Vec2& u) const {
    return dist_[posToIndex(u)];
  }

  // Set distance for vertex u.
  inline void setDist(const Vec2& u, int d) {
    dist_[posToIndex(u)] = d;
  }

  friend std::ostream& operator<<(std::ostream& to, const StateBoard& board) {
    for (int y = 0; y < board.depth_; ++y) {
      for (int x = 0; x < board.width_; ++x) {
        to << std::setw(6) << board.dist_[board.posToIndex(Vec2(x, y))] << " ";
      }
      to << "\n";
    }
    return to;
  }

 protected:
  int depth_, width_;
  std::vector<int> prev_;
  std::vector<int> dist_;

  // map 2d coordinate on the board to the index in states array.
  inline int posToIndex(const Vec2& u) const { return u.y_ * width_ + u.x_; }

  // map the index in states array to the 2d coordinate on the board
  inline Vec2 indexToPos(int i) const { return Vec2(i % width_, i / width_); }

}; // class StateBoard

// dist = -1 means infinity.
class ByDist {
 public:
  ByDist(const StateBoard& board): board_(board) {}
  bool operator()(const Vec2& u1, const Vec2& u2) const {
    const int d1 = board_.getDist(u1), d2 = board_.getDist(u2);

    if (d1 == -1 && d2 >= 0) return false;

    if (d1 >= 0 && d2 == -1) return true;

    if (d1 == d2) {
      if (u1.x_ == u2.x_) {
        return u1.y_ < u2.y_;
      }
      return u1.x_ < u2.x_;
    }

    return d1 < d2;
  }
 private:
  const StateBoard& board_;
};

// Dijkstra's algorithm for shortest path. Return false if no path
// found. Min priority queue is implemented using balanced binary
// search tree (std::set).
bool dijkstra(const Vec2& start, const Vec2& dest, const KnightMap& map,
              StateBoard& board, std::vector<Vec2>& moves, int& dist) {
  ByDist comparator(board);
  std::set<Vec2, ByDist> q(comparator);

  board.reset();
  for (int y = 0, depth = map.getDepth(); y < depth; ++y)
    for (int x = 0, width = map.getWidth(); x < width; ++x)
      q.insert(Vec2(x, y));

  std::set<Vec2>::const_iterator sIter = q.find(start);
  q.erase(sIter);
  board.setDist(start, 0);
  q.insert(start);

  // std::cout << "q.size: " << q.size() << "\n";
  while (!q.empty()) {
    std::set<Vec2>::const_iterator uIter = q.begin();
    Vec2 u = *uIter;
    q.erase(uIter);
    // std::cout << "u: " << u << "\n";
    // std::cout << "q.size: " << q.size() << "\n";
    // std::cout << board;
    // std::cout << "\n";

    const int uDist = board.getDist(u);
    if (uDist == -1) break;

    for (auto v: map.adj(u)) {
      const int oldDist = board.getDist(v);
      const int newDist = uDist + map.edgeWeight(u, v);
      // std::cout << "edge: " << u << " -> " << v << ", weight: " << map.edgeWeight(u, v) << "\n";
      // std::cout << "newDist: " << newDist << "\n";
      if (oldDist == -1 || newDist < oldDist) {
        // Remove v from sorted set, update its sort key, then insert
        // it back.
        std::set<Vec2>::const_iterator vIter = q.find(v);
        q.erase(vIter);
        board.setDist(v, newDist);
        board.setPrev(v, u);
        q.insert(v);
      }
    }
  }

  // No path.
  dist = board.getDist(dest);
  if (dist < 0) return false;

  Vec2 cur = dest;
  while (board.hasPrev(cur)) {
    const Vec2 prev(board.getPrev(cur));
    const Vec2 move = cur - prev;
    moves.push_back(move);
    cur = prev;
  }
  std::reverse(moves.begin(), moves.end());
  return true;
}

struct MoveResult {
  bool found_;
  int dist_;
  std::vector<Vec2> moves_;
  MoveResult(): found_(false), dist_(-1), moves_(0) {}
};

MoveResult findMoves(const KnightMap& map, const Vec2& start, const Vec2& end) {
  MoveResult result;
  StateBoard board(map.getDepth(), map.getWidth());
  result.found_ = dijkstra(start, end, map, board, result.moves_, result.dist_);
  return result;
}

int main(int argc, char* argv[]) {
  // Read start, end position
  Vec2 start, end;
  std::string line;
  std::getline(std::cin, line);
  std::stringstream iss(line);
  iss >> start.x_ >> start.y_;
  iss >> end.x_ >> end.y_;

  // Read the map
  KnightMap map;
  std::cin >> map;
  if (!map.isInside(start) || !map.isInside(end)) {
    throw std::runtime_error("start or end out of map.");
  }

  // std::cout << "start: " << start << "\n";
  // std::cout << "end: " << end << "\n";
  // std::cout << "map:\n" << map;
  // std::cout << "map.getWidth(): " << map.getWidth() << "\n";
  // std::cout << "map.getDepth(): " << map.getDepth() << "\n\n";

  MoveResult result = findMoves(map, start, end);
  if (!result.found_) {
    std::cout << "NO_PATH\n";
  } else {
    std::cout << result.dist_ << "\n";
    std::cout << std::showpos;
    for (auto move : result.moves_) {
      std::cout << move.x_ << "\t" << move.y_ << "\n";
    }

  }
}
