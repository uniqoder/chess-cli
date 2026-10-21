#include "../../include/core/move.h"
#include "../../include/utils/scanner.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Helpers: small validators for every piece
int8_t validate_pawn_move(const Board *board, const Move move) {
  Piece fromPiece = get_piece(board, move.from);
  Piece toPiece = get_piece(board, move.to);
  int8_t diff = move.to.rank - move.from.rank;
  int8_t nextStep = (fromPiece.color == WHITE) ? 1 : -1;
  int8_t ret = MV_UNREACHABLE;

  // Check for straight moves
  if ((move.from.file == move.to.file && abs(diff) <= 2 &&
       diff * nextStep > 0) &&
      (get_piece(board, (Position){move.from.rank + nextStep, move.from.file})
           .type == EMPTY) &&
      (abs(diff) == 1 ||
       (abs(diff) == 2 && !fromPiece.has_moved &&
        get_piece(board,
                  (Position){move.from.rank + 2 * nextStep, move.from.file})
                .type == EMPTY)))
    return MV_OK;

  // Check for captures (en passant included)
  if ((toPiece.type != EMPTY ||
       (position_eq(move.to, board->enPassantSquare))) &&
      (abs(move.to.file - move.from.file) == 1 && diff == nextStep)) {
    Piece captured;
    if (position_eq(board->enPassantSquare, move.to))
      captured =
          get_piece(board, (Position){move.to.rank - nextStep, move.to.file});
    else
      captured = toPiece;

    if (captured.color != fromPiece.color)
      ret = MV_CAPTURE;
  }

  // Handle promotion
  if (diff == nextStep && (move.to.rank == 0 || move.to.rank == 7)) {
    if (ret == MV_CAPTURE)
      ret |= MV_PROMOTION;
    else
      ret = MV_PROMOTION;
  }

  return ret;
}

int8_t validate_rook_move(const Board *board, const Move move) {
  Piece toPiece = get_piece(board, move.to);

  uint8_t isValid =
      move.from.file == move.to.file || move.from.rank == move.to.rank;
  if (!isValid)
    return MV_UNREACHABLE;

  uint8_t pathEmpty = path_is_empty(board, move.from, move.to);
  if (toPiece.type != EMPTY && pathEmpty)
    return MV_CAPTURE;
  if (pathEmpty)
    return MV_OK;

  return MV_UNREACHABLE;
}

int8_t validate_bishop_move(const Board *board, const Move move) {
  Piece toPiece = get_piece(board, move.to);
  int8_t rankDiff = move.to.rank - move.from.rank;
  int8_t fileDiff = move.to.file - move.from.file;

  if (abs(rankDiff) != abs(fileDiff))
    return MV_UNREACHABLE;

  uint8_t pathEmpty = path_is_empty(board, move.from, move.to);
  if (toPiece.type != EMPTY && pathEmpty)
    return MV_CAPTURE;
  if (pathEmpty)
    return MV_OK;

  return MV_UNREACHABLE;
}

int8_t validate_queen_move(const Board *board, const Move move) {
  Piece toPiece = get_piece(board, move.to);
  int8_t rankDiff = move.to.rank - move.from.rank;
  int8_t fileDiff = move.to.file - move.from.file;

  uint8_t isStraight =
      move.from.file == move.to.file || move.from.rank == move.to.rank;
  uint8_t isDiagonal = abs(rankDiff) == abs(fileDiff);
  if (!isStraight && !isDiagonal)
    return MV_UNREACHABLE;

  uint8_t pathEmpty = path_is_empty(board, move.from, move.to);
  if (toPiece.type != EMPTY && pathEmpty)
    return MV_CAPTURE;
  if (pathEmpty)
    return MV_OK;

  return MV_UNREACHABLE;
}

int8_t validate_knight_move(const Board *board, const Move move) {
  Piece toPiece = get_piece(board, move.to);
  int8_t rankDiff = move.to.rank - move.from.rank;
  int8_t fileDiff = move.to.file - move.from.file;

  uint8_t isValid = abs(rankDiff) + abs(fileDiff) == 3;
  if (!isValid)
    return MV_UNREACHABLE;

  if (toPiece.type != EMPTY)
    return MV_CAPTURE;
  else
    return MV_OK;
}

int8_t validate_king_move(const Board *board, const Move move) {
  Piece fromPiece = get_piece(board, move.from);
  Piece toPiece = get_piece(board, move.to);
  int8_t rankDiff = move.to.rank - move.from.rank;
  int8_t fileDiff = move.to.file - move.from.file;

  // Check for castling
  uint8_t isCastle =
      abs(fileDiff) == 2 && move.from.rank == 0 && move.to.rank == 0;
  if (!fromPiece.has_moved && isCastle) {
    int8_t step = fileDiff > 0 ? 1 : -1;
    Position pathSquares[3] = {
        move.from, (Position){move.from.rank, move.from.file + step}, move.to};
    uint8_t pathIsSafe = path_is_empty(
        board, move.from,
        (Position){0, move.to.file + (fileDiff > 0 ? step : 2 * step)});
    for (uint8_t i = 0; i < 3; i++) {
      Position *atk = get_attackers(board, pathSquares[i], !fromPiece.color);
      if (atk) {
        pathIsSafe = 0;
        free(atk);
      }
    }

    Piece rook = get_piece(board, (Position){0, move.to.file + 1});
    if (fileDiff < 0)
      rook = get_piece(board, (Position){0, move.to.file - 2});

    if (pathIsSafe && rook.type == ROOK && rook.color == fromPiece.color &&
        !rook.has_moved)
      return fileDiff < 0 ? MV_QUEEN_CASTLE : MV_KING_CASTLE;
  }

  uint8_t isValid = abs(rankDiff) < 2 && abs(fileDiff) < 2;
  if (!isValid)
    return MV_UNREACHABLE;

  if (toPiece.type != EMPTY)
    return MV_CAPTURE;
  else
    return MV_OK;
}

int8_t validate_move(const Board *board, const Move move) {
  Piece fromPiece = get_piece(board, move.from);
  Piece toPiece = get_piece(board, move.to);

  if (fromPiece.type == EMPTY)
    return MV_NO_PIECE;
  if (fromPiece.color != board->turn)
    return MV_WRONG_COLOR;
  if (fromPiece.color == toPiece.color || toPiece.type == KING)
    return MV_BLOCKED;

  if (fromPiece.type == PAWN) {
    return validate_pawn_move(board, move);
  } else if (fromPiece.type == ROOK) {
    return validate_rook_move(board, move);
  } else if (fromPiece.type == BISHOP) {
    return validate_bishop_move(board, move);
  } else if (fromPiece.type == QUEEN) {
    return validate_queen_move(board, move);
  } else if (fromPiece.type == KNIGHT) {
    return validate_knight_move(board, move);
  } else if (fromPiece.type == KING) {
    return validate_king_move(board, move);
  }

  return MV_UNREACHABLE;
}

PieceType get_promotion_type() {
  char prom;
  do {
    printf("Your pawn is evolving! Select your promotion\n[Q]ueen, K[N]ight, "
           "[B]ishop, [R]ook\n> ");
    prom = toupper((unsigned char)getchar());
  } while (prom != 'Q' && prom != 'N' && prom != 'B' && prom != 'R');
  getchar(); // Get rid of the newline in the input stream

  PieceType type = QUEEN;
  switch (prom) {
  case 'N':
    type = KNIGHT;
    break;
  case 'B':
    type = BISHOP;
    break;
  case 'R':
    type = ROOK;
  }

  return type;
}

MoveResult validate_and_move(Board *board, const Move move, Piece *captured) {
  Piece *fromPiece = &board->board[move.from.rank][move.from.file];
  Piece *toPiece = &board->board[move.to.rank][move.to.file];
  MoveResult result = validate_move(board, move);

  if (result == MV_OK || result == MV_CAPTURE || result == MV_QUEEN_CASTLE ||
      result == MV_KING_CASTLE || result == (MV_CAPTURE | MV_PROMOTION)) {
    // Set last_move for undo
    board->last_move = (PreviousMove){move.from, move.to, *fromPiece, *toPiece};

    // Handle Capture
    if (result == MV_CAPTURE || result == (MV_CAPTURE | MV_PROMOTION)) {
      *captured = *toPiece;
    }

    // Handle en passant situation
    int8_t step = (fromPiece->color == WHITE) ? 1 : -1;
    board->enPassantSquare = INVALID_POSITION; // reset enPassantSquare
    if (fromPiece->type == PAWN) {
      if (abs(move.to.rank - move.from.rank) == 2) // set enPassantSquare
        board->enPassantSquare = (Position){move.to.rank - step, move.to.file};

      if (toPiece->type == EMPTY && result != MV_OK) { // kill en passant pawn
        board->board[move.to.rank - step][move.to.file] = EMPTY_PIECE;
      }
    }

    // Handle kings in the board
    if (fromPiece->type == KING) {
      board->king_pos[fromPiece->color] = move.to;
    }

    *toPiece = *fromPiece;
    *fromPiece = EMPTY_PIECE;

    toPiece->has_moved = 1;
  }

  // Handle promotion
  if (result == MV_PROMOTION || result == (MV_CAPTURE | MV_PROMOTION)) {
    toPiece->type = get_promotion_type();
  }

  // Handle castling
  if (result == MV_QUEEN_CASTLE) {
    board->board[0][0] = EMPTY_PIECE;
    board->board[0][3] = (Piece){toPiece->color, ROOK, 1, 1};
  } else if (result == MV_KING_CASTLE) {
    board->board[0][7] = EMPTY_PIECE;
    board->board[0][5] = (Piece){toPiece->color, ROOK, 1, 1};
  }

  // Make sure king its not a self check
  if (is_king_checked(board, board->turn)) {
    undo_move(board);
    return MV_SELF_CHECK;
  }

  return result;
}
