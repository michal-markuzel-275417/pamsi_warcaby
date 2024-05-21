//
// Created by michal on 15.05.24.
//

#ifndef GAMEHANDLER_H
#define GAMEHANDLER_H
#include "checkersBoard.h"
#include <sstream>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <ctime>

// Enum representing the current state of the game.
enum gameState {
    BLACK_TURN, // Black's turn
    WHITE_TURN, // White's turn
    DRAW, // Game is a draw
    B_WIN, // Black wins
    W_WIN // White wins
};

// Enum representing the possible orientations between two positions.
enum orientation {
    UP_R, // Up-Right
    UP_L, // Up-Left
    DOWN_R, // Down-Right
    DOWN_L, // Down-Left
    WRONG // Invalid orientation
};

// Class handling the game logic, inheriting from checkersBoard.
class gameHandler : public checkersBoard {
private:
    int roundsCtr;
    gameState curentGameState; // Current state of the game
    std::vector<pos> currentMoves; // Vector storing the current moves
    bool debug = false;
    int maxKingMoves = 10;
    int whiteKingMoves;
    int blackKingMoves;

    static std::vector<int> readInput(std::string &input);

    static std::vector<pos> notationToPos(const std::vector<int> &moves);

    static pos notationToPos(int field);

    void handleNextMoves();

    bool isLeagalMoves();

    static orientation calculateOrientation(pos pos_1, pos pos_2);

    field getPieceBetween(pos pos_1, pos pos_2);

    static pos getFieldPosBetween(pos pos_1, pos pos_2);

    void isGameFinished();

    void askNextMove();

    void randomMoves();

    static float getDistanceBetween(pos tempPos_1, pos tempPos_2);

    void promotion();

    bool canTake(pos piecePos, std::vector<pos> &fieldsToJump) ;

    bool goodOrientation(pos piecePos1, pos piecePos2);

public:
    gameHandler();

    ~gameHandler() = default;

    void play();
};


#endif //GAMEHANDLER_H
