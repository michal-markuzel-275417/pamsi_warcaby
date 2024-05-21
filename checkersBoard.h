//
// Created by michal on 15.05.24.
//

#ifndef CHECKERSBOARD_H
#define CHECKERSBOARD_H
#include <vector>
#include <iostream>

/**
 * Enum representing the different types of pieces in the game.
 */
enum piece {
    CHECKER, // Checker
    KING,    // Black king
    EMPTY      // Empty field
};

enum color {
    WHITE,
    BLACK,
    NONE
};

/**
 * Struct representing a position on the board.
 */
struct pos {
    int x; // X-coordinate of the position
    int y; // Y-coordinate of the position

    // Overloading the equality operator
    bool operator==(const pos& other) const {
        return (x == other.x && y == other.y);
    }
};

/**
 * Struct representing a field on the board.
 */
struct field {
    piece pieceKind; // The type of piece on the field
    color pieceColor;
};

/**
 * Class representing the checkers board and its operations.
 */
class checkersBoard {
private:
    static char returnPieceChar(field tempField);

protected:
    std::vector<std::vector<field>> board;

public:
    checkersBoard();

    ~checkersBoard() = default;

    void printBoard();
};


#endif //CHECKERSBOARD_H
