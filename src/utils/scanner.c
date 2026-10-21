#include "../../include/utils/scanner.h"
#include "core/board.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint8_t path_is_empty(const Board *board, const Position from,
                      const Position to) {
  if (position_eq(from, INVALID_POSITION) || position_eq(to, INVALID_POSITION))
    return 0;

  uint8_t rankStep = from.rank > to.rank ? -1 : from.rank < to.rank ? 1 : 0;
  uint8_t fileStep = from.file > to.file ? -1 : from.file < to.file ? 1 : 0;

  Position pos = {from.rank + rankStep, from.file + fileStep};
  while (!position_eq(pos, to)) {
    // printf("Checking: %c%d\n", 'a' + pos.file, pos.rank + 1);
    if (get_piece(board, pos).type != EMPTY)
      return 0;
    pos = (Position){pos.rank + rankStep, pos.file + fileStep};
  };
  // printf("Checks passed!\n");

  return 1;
}

Position *get_attackers(const Board *board, const Position pos,
                        const Color opponent) {
  const uint8_t max_attackers = 16;
  const uint8_t size =
      sizeof(Position) * (max_attackers + 1); // +1 is for terminator at the end
  Position *positions = (Position *)malloc(size);
  uint8_t pos_idx = 0;

  /* Check verticals, horizontals, and diagonals */
  Position temp;
  for (int8_t rStep = -1; rStep <= 1; rStep++) {
    for (int8_t fStep = -1; fStep <= 1; fStep++) {
      if (rStep == 0 && fStep == 0)
        break;
      // const char *dir1 = rStep == 1 ? "top" : rStep == -1 ? "bottom" : "";
      // const char *dir2 = fStep == 1 ? "right" : fStep == -1 ? "left" : "";
      // printf("Checking %s%s%s: [", dir1, *dir1 && *dir2 ? "-" : "", dir2);
      temp = (Position){pos.rank + rStep, pos.file + fStep};
      while (pos_is_valid(temp)) {
        // printf(" %c%d", 'a' + temp.file, 1 + temp.rank);
        Piece p = get_piece(board, temp);
        uint8_t piece_checks = 0;
        if (rStep && fStep)
          piece_checks = p.type == BISHOP || p.type == QUEEN || p.type == PAWN;
        else
          piece_checks = p.type == ROOK || p.type == QUEEN;
        if (p.color == opponent && piece_checks) {
          positions[pos_idx++] = temp;
          break;
        } else if (p.type != EMPTY)
          break;
        temp.file += fStep;
        temp.rank += rStep;
      }
      // printf(" ]\n");
    }
  }

  /* Check for knight attacks */
  Position knight_attacks[8] = {
      {pos.rank + 2, pos.file + 1}, {pos.rank + 2, pos.file - 1},
      {pos.rank - 2, pos.file + 1}, {pos.rank - 2, pos.file - 1},
      {pos.rank + 1, pos.file + 2}, {pos.rank + 1, pos.file - 2},
      {pos.rank - 1, pos.file + 2}, {pos.rank - 1, pos.file - 2}};
  for (uint8_t i = 0; i < 8; i++) {
    temp = knight_attacks[i];
    if (!pos_is_valid(temp))
      continue;
    Piece p = get_piece(board, temp);
    if (p.color == opponent && p.type == KNIGHT)
      positions[pos_idx++] = temp;
  }

  if (pos_idx == 0)
    return NULL;
  positions[pos_idx] = INVALID_POSITION;
  return positions;
}

uint8_t is_king_checked(const Board *board, Color color) {
  Position *attackers = get_attackers(board, board->king_pos[color], !color);

  uint8_t isChecked = attackers != NULL;
  free(attackers);
  return isChecked;
}
