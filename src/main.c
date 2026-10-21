#include "core/board.h"
#include "core/move.h"
#include "utils/scanner.h"
#include "utils/string.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void clear_scr() { system("clear"); }

Move parse_move(const char *str) {
  Position from, to;

  if (strlen(str) != 5) {
    printf("Invalid input: \"%s\" Example input: e2 e4 (from, to).\n", str);
    return (Move){INVALID_POSITION, INVALID_POSITION};
  }

  // Parsing `from`
  char ff = str[0];
  if (ff < 'a' || ff > 'h') {
    printf("Invalid input: \"%s\" Files must be between 'a' and 'h' "
           "(inclusive).\n",
           str);
    return (Move){INVALID_POSITION, INVALID_POSITION};
  } else {
    from.file = ff - 'a';
  }

  char fr = str[1];
  if (fr < '1' || fr > '8') {
    printf("Invalid input \"%s\" Ranks must be between 1 and 8 (inclusive).\n",
           str);
    return (Move){INVALID_POSITION, INVALID_POSITION};

  } else {
    from.rank = fr - '0' - 1;
  }

  if (str[2] != ' ') {
    printf("Invalid input: \"%s\" Example input: e2 e4 (from, to).\n", str);
    return (Move){INVALID_POSITION, INVALID_POSITION};
  }

  // Parsing `to`
  char tf = str[3];
  if (tf < 'a' || tf > 'h') {
    printf("Invalid input: \"%s\" Files must be between 'a' and 'h' "
           "(inclusive).\n",
           str);
    return (Move){INVALID_POSITION, INVALID_POSITION};
  } else {
    to.file = tf - 'a';
  }

  char tr = str[4];
  if (tr < '1' || tr > '8') {
    printf("Invalid input \"%s\" Ranks must be between 1 and 8 (inclusive).\n",
           str);
    return (Move){INVALID_POSITION, INVALID_POSITION};
  } else {
    to.rank = tr - '0' - 1;
  }

  return (Move){from, to};
}

Move get_move(Color turn) {
  Move move;
  do {
    if (turn == WHITE) {
      printf("White to move > ");
    } else {
      printf("Black to move > ");
    }

    char input[32];
    fgets(input, 32, stdin);
    str_trim(input);
    if (strcmp(input, "quit") == 0 || strcmp(input, "exit") == 0)
      return (Move){INVALID_POSITION, INVALID_POSITION};
    move = parse_move(input);
  } while (position_eq(move.from, INVALID_POSITION));
  return move;
}

const char *get_piece_type_name(PieceType type) {
  switch (type) {
  case PAWN:
    return "Pawn";
  case KNIGHT:
    return "Knight";
  case BISHOP:
    return "Bishop";
  case ROOK:
    return "Rook";
  case QUEEN:
    return "Queen";
  default:
    return "Piece";
  }
}

char *print_move_result(const MoveResult result, const Board *board,
                        const Move move, Piece captured) {
  if (result == MV_UNREACHABLE) {
    char *sq = get_pos_str(move.to);
    printf("%s is not a valid destination for this piece.\n", sq);
    free(sq);
  } else if (result == MV_BLOCKED) {
    char *sq = get_pos_str(move.to);
    printf("%s can't be reached. It is blocked.\n", sq);
    free(sq);
  } else if (result == MV_NO_PIECE) {
    char *sq = get_pos_str(move.from);
    printf("%s doesn't contain a piece to move.\n", sq);
    free(sq);
  } else if (result == MV_WRONG_COLOR) {
    printf("You can't move the opponents piece. Stop cheating!\n");
  } else if (result == MV_SELF_CHECK) {
    printf("You can't intentionally put your king in check!\n");
  } else if (result == MV_CAPTURE) {
    Piece self = get_piece(board, move.to);
    const char *color = self.color == WHITE ? "White" : "Black";
    char *msg = (char *)malloc(128);
    snprintf(msg, 128, "%s captured a %s!", color,
             get_piece_type_name(captured.type));
    return msg;
  } else if (result == MV_PROMOTION) {
    Piece self = get_piece(board, move.to);
    char *msg = (char *)malloc(128);
    const char *color = self.color == WHITE ? "White" : "Black";
    snprintf(msg, 128, "%s's Pawn promoted to a %s!", color,
             get_piece_type_name(self.type));
    return msg;
  } else if (result == (MV_CAPTURE | MV_PROMOTION)) {
    Piece self = get_piece(board, move.to);
    const char *color = self.color == WHITE ? "White" : "Black";
    char *msg = (char *)malloc(128);
    snprintf(msg, 128, "%s captured a %s and promoted to a %s!", color,
             get_piece_type_name(captured.type),
             get_piece_type_name(self.type));
    return msg;
  }

  return NULL;
}

Move normalized_move(Move mv, Color col) {
  if (col == BLACK) {
    mv.from.rank = 7 - mv.from.rank;
    mv.to.rank = 7 - mv.to.rank;
  }
  return mv;
}

uint8_t result_is_valid(MoveResult result) {
  return result != MV_UNREACHABLE && result != MV_BLOCKED &&
         result != MV_NO_PIECE && result != MV_WRONG_COLOR &&
         result != MV_SELF_CHECK && result;
}

int main(void) {
  Board *board = create_board();
  char *msg = NULL;

  uint8_t isRunning = 1;
  while (isRunning) {
    clear_scr();
    draw_board(board);

    if (msg != NULL) {
      printf("%s\n", msg);
      free(msg);
      msg = NULL;
    }

    MoveResult result;
    do {
      Move move = get_move(board->turn);
      if (!move_is_valid(move)) {
        free(board);
        isRunning = 0;
        break;
      }
      const Move normal_mv = normalized_move(move, board->turn);

      Piece captured;
      result = validate_and_move(board, normal_mv, &captured);

      msg = print_move_result(result, board, move, captured);

      // Check for king checks
      if (is_king_checked(board, board->turn)) {
        Piece movedPiece = get_piece(board, move.to);
        const char *color = movedPiece.color == WHITE ? "White" : "Black";
        char *oldMsg = strdup(oldMsg);
        free(msg);
        msg = (char *)malloc(512);
        snprintf(msg, 512, "%s\n%s's king is in check!\n", oldMsg, color);
      }

      if (result_is_valid(result))
        board->turn = !board->turn;

    } while (!result_is_valid(result));
  }

  return 0;
}
