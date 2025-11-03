#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <utility>
#include <string>

extern int rows;         // The count of rows of the game map.
extern int columns;      // The count of columns of the game map.
extern int total_mines;  // The count of mines of the game map.

// Client-side knowledge of the board
static const int MAXN_CLIENT = 35;
char vision[MAXN_CLIENT][MAXN_CLIENT];  // what we currently see: '?', '0'-'8', '@'

inline bool inb_client(int r, int c) { return r >= 0 && r < rows && c >= 0 && c < columns; }


// You MUST NOT use any other external variables except for rows, columns and total_mines.

/**
 * @brief The definition of function Execute(int, int, bool)
 *
 * @details This function is designed to take a step when player the client's (or player's) role, and the implementation
 * of it has been finished by TA. (I hope my comments in code would be easy to understand T_T) If you do not understand
 * the contents, please ask TA for help immediately!!!
 *
 * @param r The row coordinate (0-based) of the block to be visited.
 * @param c The column coordinate (0-based) of the block to be visited.
 * @param type The type of operation to a certain block.
 * If type == 0, we'll execute VisitBlock(row, column).
 * If type == 1, we'll execute MarkMine(row, column).
 * If type == 2, we'll execute AutoExplore(row, column).
 * You should not call this function with other type values.
 */
void Execute(int r, int c, int type);

/**
 * @brief The definition of function InitGame()
 *
 * @details This function is designed to initialize the game. It should be called at the beginning of the game, which
 * will read the scale of the game map and the first step taken by the server (see README).
 */
void InitGame() {
  // Initialize client knowledge
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      vision[i][j] = '?';
    }
  }
  int first_row, first_column;
  std::cin >> first_row >> first_column;
  Execute(first_row, first_column, 0);
}

/**
 * @brief The definition of function ReadMap()
 *
 * @details This function is designed to read the game map from stdin when playing the client's (or player's) role.
 * Since the client (or player) can only get the limited information of the game map, so if there is a 3 * 3 map as
 * above and only the block (2, 0) has been visited, the stdin would be
 *     ???
 *     12?
 *     01?
 */
void ReadMap() {
  // Read the current visible map from stdin (PrintMap output)
  for (int i = 0; i < rows; ++i) {
    std::string line;
    std::cin >> line;
    for (int j = 0; j < columns; ++j) {
      vision[i][j] = line[j];
    }
  }
}

/**
 * @brief The definition of function Decide()
 *
 * @details This function is designed to decide the next step when playing the client's (or player's) role. Open up your
 * mind and make your decision here! Caution: you can only execute once in this function.
 */
void Decide() {
  // Improved Baseline:
  // 1) Auto-explore whenever flags suffice
  // 2) Mark when unknown_count == number - flagged_count
  // 3) Guess: pick unknown with lowest local risk estimate
  static const int dr[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
  static const int dc[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

  // Step 1: auto-explore
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      char ch = vision[i][j];
      if (ch < '0' || ch > '8') continue;
      int need = ch - '0';
      int flagged = 0, unknown = 0;
      for (int k = 0; k < 8; ++k) {
        int ni = i + dr[k], nj = j + dc[k];
        if (!inb_client(ni, nj)) continue;
        if (vision[ni][nj] == '@') ++flagged;
        else if (vision[ni][nj] == '?') ++unknown;
      }
      if (unknown > 0 && flagged == need) {
        Execute(i, j, 2);
        return;
      }
    }
  }

  // Step 2: mark one certain mine
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      char ch = vision[i][j];
      if (ch < '0' || ch > '8') continue;
      int need = ch - '0';
      int flagged = 0, unknown = 0;
      int ux = -1, uy = -1;
      for (int k = 0; k < 8; ++k) {
        int ni = i + dr[k], nj = j + dc[k];
        if (!inb_client(ni, nj)) continue;
        if (vision[ni][nj] == '@') ++flagged;
        else if (vision[ni][nj] == '?') { ++unknown; ux = ni; uy = nj; }
      }
      if (unknown > 0 && need == flagged + unknown) {
        Execute(ux, uy, 1);
        return;
      }
    }
  }

  // Step 3: guess with a simple local risk metric
  int best_r = -1, best_c = -1;
  double best_score = 1e9;
  // global fallback risk
  int total_unknown = 0, total_flagged = 0;
  for (int i = 0; i < rows; ++i) for (int j = 0; j < columns; ++j) {
    if (vision[i][j] == '?') ++total_unknown;
    else if (vision[i][j] == '@') ++total_flagged;
  }
  double global_risk = (total_unknown > 0) ? (double)std::max(0, total_mines - total_flagged) / (double)total_unknown : 1.0;

  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      if (vision[i][j] != '?') continue;
      double risk_sum = 0.0; int constraints = 0;
      for (int k = 0; k < 8; ++k) {
        int ni = i + dr[k], nj = j + dc[k];
        if (!inb_client(ni, nj)) continue;
        char ch = vision[ni][nj];
        if (ch >= '0' && ch <= '8') {
          int need = ch - '0';
          int flagged = 0, unknown = 0;
          for (int t = 0; t < 8; ++t) {
            int ai = ni + dr[t], aj = nj + dc[t];
            if (!inb_client(ai, aj)) continue;
            if (vision[ai][aj] == '@') ++flagged;
            else if (vision[ai][aj] == '?') ++unknown;
          }
          if (unknown > 0) {
            double local = (double)std::max(0, need - flagged) / (double)unknown;
            risk_sum += local;
            ++constraints;
          }
        }
      }
      double risk = (constraints > 0) ? (risk_sum / constraints) : global_risk;
      if (risk < best_score) { best_score = risk; best_r = i; best_c = j; }
    }
  }
  if (best_r != -1) { Execute(best_r, best_c, 0); return; }

  // Fallback
  Execute(0, 0, 0);
}

#endif