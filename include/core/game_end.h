#ifndef INCLUDEcoregame_endgame_end
#define INCLUDEcoregame_endgame_end

#include "./board.h"

typedef enum {
  DRAW_NONE,
  DRAW_STALEMATE,
  DRAW_FIFTY_MOVE,
  DRAW_INSUFFICIENT,
  DRAW_REPETITION
} DrawType;

// Basic checks to see if the game ended or someone is in check
uint8_t is_in_check(const Board *board, const Color color);
uint8_t is_checkmate(const Board *board, const Color color);
DrawType is_draw(const Board *board);

#endif // INCLUDEcoregame_endgame_end
