#ifndef INCLUDEcoremovemove
#define INCLUDEcoremovemove

#include "./board.h"

typedef enum {
  MV_OK = 10,
  MV_SELF_CHECK,
  MV_BLOCKED,
  MV_WRONG_COLOR,
  MV_CAPTURE,
  MV_PROMOTION,
  MV_UNREACHABLE,
  MV_NO_PIECE,
  MV_QUEEN_CASTLE,
  MV_KING_CASTLE
} MoveResult;

typedef struct {
  Position from;
  Position to;
} Move;

// Makes a given move on the board,
// returns a result specifying if the move was successfull or not
MoveResult validate_and_move(Board *board, const Move move, Piece *captured);

static inline uint8_t move_is_valid(const Move move) {
  return pos_is_valid(move.from) && pos_is_valid(move.to);
}

#endif // INCLUDEcoremovemove
