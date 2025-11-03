#ifndef SERVER_H
#define SERVER_H

#include <cstdlib>
#include <iostream>
#include <vector>
#include <queue>
#include <string>

/*
 * You may need to define some global variables for the information of the game map here.
 * Although we don't encourage to use global variables in real cpp projects, you may have to use them because the use of
 * class is not taught yet. However, if you are member of A-class or have learnt the use of cpp class, member functions,
 * etc., you're free to modify this structure.
 */
int rows;         // The count of rows of the game map. You MUST NOT modify its name.
int columns;      // The count of columns of the game map. You MUST NOT modify its name.
int total_mines;  // The count of mines of the game map. You MUST NOT modify its name. You should initialize this

// ====== Global game state (you may define globals here) ======
const int MAXN = 35;  // rows, columns <= 30 per constraints
bool is_mine[MAXN][MAXN];
int adj[MAXN][MAXN];
bool visited[MAXN][MAXN];
bool marked[MAXN][MAXN];
int visited_non_mine_count = 0;
int correctly_marked_mines = 0;
int total_non_mines = 0;


inline void check_victory();

inline bool inb(int r, int c) { return r >= 0 && r < rows && c >= 0 && c < columns; }

inline void compute_adj() {
  static const int dr[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
  static const int dc[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      if (is_mine[i][j]) {
        adj[i][j] = -1;  // mark mine
        continue;
      }
      int cnt = 0;
      for (int k = 0; k < 8; ++k) {
        int ni = i + dr[k], nj = j + dc[k];
        if (inb(ni, nj) && is_mine[ni][nj]) ++cnt;
      }
      adj[i][j] = cnt;
    }
  }
}

inline void reveal_bfs(int r, int c) {
  if (!inb(r, c) || visited[r][c] || marked[r][c] || is_mine[r][c]) return;
  std::queue<std::pair<int, int>> q;
  visited[r][c] = true;
  ++visited_non_mine_count;
  q.push({r, c});
  static const int dr[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
  static const int dc[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
  while (!q.empty()) {
    auto [x, y] = q.front();
    q.pop();
    if (adj[x][y] != 0) continue;  // only expand zeros
    for (int k = 0; k < 8; ++k) {
      int nx = x + dr[k], ny = y + dc[k];
      if (inb(nx, ny) && !visited[nx][ny] && !marked[nx][ny] && !is_mine[nx][ny]) {
        visited[nx][ny] = true;
        ++visited_non_mine_count;
        if (adj[nx][ny] == 0) q.push({nx, ny});
      }
    }
  }
}


                  // variable in function InitMap. It will be used in the advanced task.
int game_state;  // The state of the game, 0 for continuing, 1 for winning, -1 for losing. You MUST NOT modify its name.

/**
 * @brief The definition of function InitMap()
 *
 * @details This function is designed to read the initial map from stdin. For example, if there is a 3 * 3 map in which

 * mines are located at (0, 1) and (1, 2) (0-based), the stdin would be
 *     3 3
 *     .X.
 *     ...
 *     ..X
 * where X stands for a mine block and . stands for a normal block. After executing this function, your game map
 * would be initialized, with all the blocks unvisited.
 */
void InitMap() {
  std::cin >> rows >> columns;
  total_mines = 0;

  visited_non_mine_count = 0;
  correctly_marked_mines = 0;
  for (int i = 0; i < rows; ++i) {
    std::string line;
    std::cin >> line;
    for (int j = 0; j < columns; ++j) {
      is_mine[i][j] = (line[j] == 'X');
      if (is_mine[i][j]) ++total_mines;
      visited[i][j] = false;
      marked[i][j] = false;
    }
  }
  total_non_mines = rows * columns - total_mines;
  game_state = 0;
  compute_adj();
}


inline void check_victory() {
  if (visited_non_mine_count == total_non_mines) {
    game_state = 1;
  }
}

/**
 * @brief The definition of function VisitBlock(int, int)
 *
 * @details This function is designed to visit a block in the game map. We take the 3 * 3 game map above as an example.
 * At the beginning, if you call VisitBlock(0, 0), the return value would be 0 (game continues), and the game map would
 * be
 *     1??
 *     ???
 *     ???
 * If you call VisitBlock(0, 1) after that, the return value would be -1 (game ends and the players loses) , and the
 * game map would be
 *     1X?
 *     ???
 *     ???
 * If you call VisitBlock(0, 2), VisitBlock(2, 0), VisitBlock(1, 2) instead, the return value of the last operation
 * would be 1 (game ends and the player wins), and the game map would be
 *     1@1
 *     122
 *     01@
 *
 * @param r The row coordinate (0-based) of the block to be visited.
 * @param c The column coordinate (0-based) of the block to be visited.
 *
 * @note You should edit the value of game_state in this function. Precisely, edit it to
 *    0  if the game continues after visit that block, or that block has already been visited before.
 *    1  if the game ends and the player wins.
 *    -1 if the game ends and the player loses.
 *
 * @note For invalid operation, you should not do anything.
 */
void VisitBlock(int r, int c) {
  if (!inb(r, c) || game_state != 0) return;
  if (visited[r][c] || marked[r][c]) return;
  if (is_mine[r][c]) {
    visited[r][c] = true;  // reveal the mine
    game_state = -1;
    return;
  }
  reveal_bfs(r, c);
  check_victory();
}

/**
 * @brief The definition of function MarkMine(int, int)
 *
 * @details This function is designed to mark a mine in the game map.
 * If the block being marked is a mine, show it as "@".
 * If the block being marked isn't a mine, END THE GAME immediately. (NOTE: This is not the same rule as the real
 * game) And you don't need to
 *
 * For example, if we use the same map as before, and the current state is:
 *     1?1
 *     ???
 *     ???
 * If you call MarkMine(0, 1), you marked the right mine. Then the resulting game map is:
 *     1@1
 *     ???
 *     ???
 * If you call MarkMine(1, 0), you marked the wrong mine(There's no mine in grid (1, 0)).
 * The game_state would be -1 and game ends immediately. The game map would be:
 *     1?1
 *     X??
 *     ???
 * This is different from the Minesweeper you've played. You should beware of that.
 *
 * @param r The row coordinate (0-based) of the block to be marked.
 * @param c The column coordinate (0-based) of the block to be marked.
 *
 * @note You should edit the value of game_state in this function. Precisely, edit it to
 *    0  if the game continues after visit that block, or that block has already been visited before.
 *    1  if the game ends and the player wins.
 *    -1 if the game ends and the player loses.
 *
 * @note For invalid operation, you should not do anything.
 */
void MarkMine(int r, int c) {
  if (!inb(r, c) || game_state != 0) return;
  if (visited[r][c] || marked[r][c]) return;
  marked[r][c] = true;
  if (is_mine[r][c]) {
    ++correctly_marked_mines;
  } else {
    game_state = -1;  // wrong mark ends the game immediately
  }
}

/**
 * @brief The definition of function AutoExplore(int, int)
 *
 * @details This function is designed to auto-visit adjacent blocks of a certain block.
 * See README.md for more information
 *
 * For example, if we use the same map as before, and the current map is:
 *     ?@?
 *     ?2?
 *     ??@
 * Then auto explore is available only for block (1, 1). If you call AutoExplore(1, 1), the resulting map will be:
 *     1@1
 *     122
 *     01@
 * And the game ends (and player wins).
 */
void AutoExplore(int r, int c) {
  if (!inb(r, c) || game_state != 0) return;
  if (!visited[r][c] || is_mine[r][c]) return;  // only for visited non-mine cells
  int need = adj[r][c];
  int marked_cnt = 0;
  static const int dr[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
  static const int dc[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
  for (int k = 0; k < 8; ++k) {
    int ni = r + dr[k], nj = c + dc[k];
    if (inb(ni, nj) && marked[ni][nj]) ++marked_cnt;
  }
  if (marked_cnt != need) return;
  for (int k = 0; k < 8; ++k) {
    int ni = r + dr[k], nj = c + dc[k];
    if (!inb(ni, nj) || marked[ni][nj]) continue;
    if (!visited[ni][nj] && !is_mine[ni][nj]) {
      reveal_bfs(ni, nj);
    }
  }
  check_victory();
}

/**
 * @brief The definition of function ExitGame()
 *
 * @details This function is designed to exit the game.
 * It outputs a line according to the result, and a line of two integers, visit_count and marked_mine_count,
 * representing the number of blocks visited and the number of marked mines taken respectively.
 *
 * @note If the player wins, we consider that ALL mines are correctly marked.
 */
void ExitGame() {
  if (game_state == 1) {
    std::cout << "YOU WIN!\n";
  } else {
    std::cout << "GAME OVER!\n";
  }
  int visit_count = visited_non_mine_count;
  int marked_mine_count = (game_state == 1) ? total_mines : correctly_marked_mines;
  std::cout << visit_count << " " << marked_mine_count << std::endl;
  exit(0);  // Exit the game immediately
}

/**
 * @brief The definition of function PrintMap()
 *
 * @details This function is designed to print the game map to stdout. We take the 3 * 3 game map above as an example.
 * At the beginning, if you call PrintMap(), the stdout would be
 *    ???
 *    ???
 *    ???
 * If you call VisitBlock(2, 0) and PrintMap() after that, the stdout would be
 *    ???
 *    12?
 *    01?
 * If you call VisitBlock(0, 1) and PrintMap() after that, the stdout would be
 *    ?X?
 *    12?
 *    01?
 * If the player visits all blocks without mine and call PrintMap() after that, the stdout would be
 *    1@1
 *    122
 *    01@
 * (You may find the global variable game_state useful when implementing this function.)
 *
 * @note Use std::cout to print the game map, especially when you want to try the advanced task!!!
 */
void PrintMap() {
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      char ch = '?';
      if (game_state == 1) {
        // Victory: show all mines as '@', others as numbers
        if (is_mine[i][j]) ch = '@';
        else ch = static_cast<char>('0' + adj[i][j]);
      } else {
        if (marked[i][j]) {
          ch = is_mine[i][j] ? '@' : 'X';
        } else if (visited[i][j]) {
          ch = is_mine[i][j] ? 'X' : static_cast<char>('0' + adj[i][j]);
        } else {
          ch = '?';
        }
      }
      std::cout << ch;
    }
    std::cout << std::endl;
  }
}

#endif
