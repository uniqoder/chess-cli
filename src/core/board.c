#include "../../include/core/board.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>

const PieceType backRank[8] = {ROOK, KNIGHT, BISHOP, QUEEN,
                               KING, BISHOP, KNIGHT, ROOK};

// Helper: initializes the main board
void initialize_board(Board *b) {
  const uint8_t backRanks[] = {0, 7};
  const Color colors[] = {WHITE, BLACK};

  // Black & white back ranks
  for (uint8_t c = 0; c < 2; c++) {
    for (uint8_t f = 0; f < 8; f++) {
      const uint8_t r = backRanks[c];
      const Color col = colors[c];
      b->board[r][f] = (Piece){col, backRank[f], 1, 0};
    }
  }

  // Black & white pawn ranks
  const uint8_t pawnRanks[] = {1, 6};

  for (uint8_t c = 0; c < 2; c++) {
    for (uint8_t f = 0; f < 8; f++) {
      const uint8_t r = pawnRanks[c];
      const Color col = colors[c];
      b->board[r][f] = (Piece){col, PAWN, 1, 0};
    }
  }

  // Middle of the board
  for (uint8_t r = 2; r < 6; r++) {
    for (uint8_t f = 0; f < 8; f++) {
      b->board[r][f] = EMPTY_PIECE;
    }
  }
}

// Helper: returns the character corresponding to piece type and color
char getPieceChar(Piece p) {
  switch (p.type) {
  case PAWN:
    return p.color == WHITE ? 'P' : 'p';
  case ROOK:
    return p.color == WHITE ? 'R' : 'r';
  case KNIGHT:
    return p.color == WHITE ? 'N' : 'n';
  case BISHOP:
    return p.color == WHITE ? 'B' : 'b';
  case KING:
    return p.color == WHITE ? 'K' : 'k';
  case QUEEN:
    return p.color == WHITE ? 'Q' : 'q';
  case EMPTY:
    return ' ';
  default:
    return '?';
  }
}

const char *getColorCode(Color color) {
  if (color == WHITE)
    return "\033[30;47m"; // Black on White
  if (color == BLACK)
    return "\033[37;40m"; // White on Black
  return "";
}

Board *create_board() {
  Board *b = (Board *)malloc(sizeof(Board));
  if (!b)
    return NULL;

  initialize_board(b);
  b->king_pos[WHITE] = (Position){0, 4};
  b->king_pos[BLACK] = (Position){7, 4};
  b->enPassantSquare = INVALID_POSITION;
  b->last_move = (PreviousMove){INVALID_POSITION, INVALID_POSITION, EMPTY_PIECE,
                                EMPTY_PIECE};
  b->turn = WHITE;

  return b;
}

void draw_board(const Board *board) {
  printf("+-----------------------------------+  \n");
  printf("|                 CHESS             |  \n");
  printf("|   +---+---+---+---+---+---+---+---+  \n");

  // Iterates 1 extra time for both rank and file to print rank and file labels
  int8_t start, step, end;
  if (board->turn == WHITE) {
    start = 7;
    step = -1;
    end = 0;
  } else {
    start = 0;
    step = 1;
    end = 7;
  }

  for (int8_t r = start; r != end + step; r += step) {
    for (int8_t f = 0; f < 8; f++) {
      if (f == 0) {
        printf("| %d |", (end + step - r) * step);
      }

      Piece p = board->board[r][f];
      printf("%s %c \033[0m|", getColorCode(p.color), getPieceChar(p));

      if (r == end && f == 7) {
        printf("\n");
        printf("|   +---+---+---+---+---+---+---+---+  \n");
        printf("|     a   b   c   d   e   f   g   h | \n");
        printf("+-----------------------------------+  \n");
      }
    }
    printf("\n");
    if (r != end)
      printf("|   +---+---+---+---+---+---+---+---+  \n");
  }
}

uint8_t undo_move(Board *board) {
  // Make sure a last_move exists
  if (!pos_is_valid(board->last_move.from) ||
      !pos_is_valid(board->last_move.to))
    return 0; // Failure

  Piece *fromSquare =
      &board->board[board->last_move.from.rank][board->last_move.from.file];
  Piece *toSquare =
      &board->board[board->last_move.to.rank][board->last_move.to.file];

  *fromSquare = board->last_move.fromPiece;
  *toSquare = board->last_move.toPiece;

  if (fromSquare->type == KING) {
    printf("switched king pos\n");
    board->king_pos[fromSquare->color] = board->last_move.from;
  }

  board->last_move = (PreviousMove){INVALID_POSITION, INVALID_POSITION,
                                    EMPTY_PIECE, EMPTY_PIECE};

  return 1; // Success
}

char *get_pos_str(const Position pos) {
  char file = 'a' + pos.file;
  uint8_t rank = 1 + pos.rank;

  char *str = (char *)malloc(5);
  if (!str) {
    printf("Couldn't allocate memory, try again.\n");
    return NULL;
  }
  snprintf(str, 5, "%c%d", file, rank);

  return str;
}
