//
// Created by michal on 23.05.24.
//

#include "gameAlgorithm.h"

#include <cfloat>

gameAlgorithm::gameAlgorithm(int depth) {
    maxDepth=depth;
}

std::vector<pos> gameAlgorithm::getBestMove() {
    std::vector<std::vector<pos> > legalMoves;
    legalMoves = generateMovesList(game);
    float val;
    float minValue = 0, maxValue = 0;
    int minValueIndex = 0, maxValueIndex = 0;


    for (int i = 0; i < legalMoves.size(); i++) {
        val = evaluatePositionRecursive(0, game, 1.0f);

        if (val < minValue) {
            minValue = val;
            minValueIndex = i;
        }
        if (val > maxValue) {
            maxValue = val;
            maxValueIndex = i;
        }
    }

    if (game.getCurrentGameState() == BLACK_TURN)
        return legalMoves[maxValueIndex];
    if (game.getCurrentGameState() == WHITE_TURN)
        return legalMoves[minValueIndex];
}

std::vector<std::vector<pos> > gameAlgorithm::generateMovesList(gameHandler curGame) {
    std::vector<pos> piecesPos, emptyFields;
    std::vector<std::vector<pos> > legalMoves;
    std::vector<pos> moves;

    bool moveFound = false;
    piecesPos = getPlayerFields(curGame);
    emptyFields = getEmptyFields(curGame);

    for (int i = 0; i < emptyFields.size(); i++) {
        for (int j = 0; j < piecesPos.size(); j++) {
            curGame.currentMoves.clear();
            curGame.currentMoves.push_back(piecesPos[j]);
            curGame.currentMoves.push_back(emptyFields[i]);

            if (curGame.isLeagalMoves()) {
                moveFound = true;
                legalMoves.push_back(curGame.currentMoves);
            }
        }
    }

    return legalMoves;
}

std::vector<pos> gameAlgorithm::getPlayerFields(gameHandler curGame) {
    std::vector<pos> piecesPos;
    gameState curentGameState = curGame.getCurrentGameState();

    // Zbieranie pozycji figur aktualnego gracza
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if ((curentGameState == WHITE_TURN && curGame.board[x][y].pieceColor == WHITE) ||
                (curentGameState == BLACK_TURN && curGame.board[x][y].pieceColor == BLACK)) {
                piecesPos.push_back({x, y});
            }
        }
    }

    return piecesPos;
}

std::vector<pos> gameAlgorithm::getEmptyFields(gameHandler curGame) {
    std::vector<pos> emptyFields;

    // Zbieranie pustych pól
    pos temp;
    for (int i = 1; i < 33; i++) {
        temp = curGame.notationToPos(i);

        if (curGame.board[temp.x][temp.y].pieceKind == EMPTY) {
            emptyFields.push_back(temp);
        }
    }

    return emptyFields;
}

int gameAlgorithm::calculateBoard(std::vector<std::vector<field> > board) {
    int blackCtr = 0;
    int whiteCtr = 0;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if (board[x][y].pieceColor == WHITE)
                whiteCtr++;
            if (board[x][y].pieceColor == BLACK)
                blackCtr++;
        }
    }

    return blackCtr - whiteCtr;
}

float gameAlgorithm::evaluatePositionRecursive(int depth, gameHandler curGame, float signFactor) {
    if (depth >= maxDepth)
        return calculateBoard(curGame.getBoard());

    std::vector<std::vector<pos> > moves = generateMovesList(curGame);

    float posValue = -FLT_MAX;

    for (int i = 0; i < moves.size(); i++) {
        gameHandler newGame;
        newGame.setBoard(curGame.getBoard());
        newGame.currentMoves = moves[i];
        newGame.handleNextMoves();

        float newValue = signFactor * evaluatePositionRecursive(depth + 1, newGame, -signFactor);

        if (newValue > posValue)
            posValue = newValue;
    }

    return signFactor * posValue;
}

/**
 * Plays the game by alternating turns until the game ends.
 */
void gameAlgorithm::play() {
    printBoard();
    std::vector<pos> moves;

    while (curentGameState == BLACK_TURN || curentGameState == WHITE_TURN) {
        if (curentGameState == BLACK_TURN) {
            askNextMove();
            // std::cout << "Czarne: random" << std::endl;
            // randomMoves();
        } else {
            // askNextMove();
            std::cout << "Białe: min-max" << std::endl;
            moves = getBestMove();
            currentMoves = moves;
        }

        handleNextMoves();
        isGameFinished();
        printBoard();

        roundsCtr++;
    }

    if (curentGameState == B_WIN)
        std::cout << "KONIEC GRY - czarne wygrywają " << roundsCtr << std::endl << std::endl;
    else if (curentGameState == W_WIN)
        std::cout << "KONIEC GRY - białe wygrywają " << roundsCtr << std::endl << std::endl;
    else
        std::cout << "KONIEC GRY - remis" << std::endl << std::endl;
}
