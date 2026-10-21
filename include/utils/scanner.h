#ifndef INCLUDEcorescannerscanner
#define INCLUDEcorescannerscanner

#include "../core/board.h"
#include <stdint.h>

// Checks if one of the squares in the ray between `from` and `to` is blocked by
// some piece
uint8_t path_is_empty(const Board *board, const Position from,
                      const Position to);

// Returns a list of positions from where `pos` is attacked by opponent
Position *get_attackers(const Board *board, const Position pos,
                        const Color opponent);
uint8_t is_king_checked(const Board *board, Color color);

#endif // INCLUDEcorescannerscanner
