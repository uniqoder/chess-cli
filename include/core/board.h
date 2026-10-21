#ifndef INCLUDEcoreboardboard
#define INCLUDEcoreboardboard

#include <stdint.h>

#define INVALID_POSITION ((Position){255, 255})
#define EMPTY_PIECE ((Piece){NO_COLOR, EMPTY, 0, 0})

typedef enum { EMPTY, PAWN, KNIGHT, ROOK, BISHOP, QUEEN, KING } PieceType;

typedef enum {
  NO_COLOR = -1,
  WHITE = 0,
  BLACK = 1,
} Color;

typedef struct {
  Color color;
  PieceType type;
  uint8_t is_alive;
  uint8_t has_moved;
} Piece;

typedef struct {
  uint8_t rank;
  uint8_t file;
} Position;

typedef struct {
  Position from;
  Position to;
  Piece fromPiece;
  Piece toPiece;
} PreviousMove;

typedef struct {
  Piece board[8][8];
  Position enPassantSquare;
  Position king_pos[2]; // [white_king, black_king]
  PreviousMove last_move;
  Color turn;
} Board;

// Returns a new instance of a board allocated on the heap
Board *create_board();
// Draws the current board
void draw_board(const Board *board);
uint8_t undo_move(Board *board);
char *get_pos_str(const Position pos);

// Small helpers
static inline uint8_t position_eq(const Position p1, const Position p2) {
  return p1.file == p2.file && p1.rank == p2.rank;
}

static inline uint8_t pos_is_valid(const Position pos) {
  return (pos.rank < 8) && (pos.file < 8);
}

static inline Piece get_piece(const Board *board, const Position pos) {
  if (!pos_is_valid(pos))
    return EMPTY_PIECE;
  return board->board[pos.rank][pos.file];
}

#endif // INCLUDEcoreboardboard
