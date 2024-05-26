//
// Created by michal on 23.05.24.
//

#include "gameAlgorithm.h"

#include <cfloat>
#include <climits>

// https://www.youtube.com/watch?v=l-hh51ncgDI
// https://gamedev.stackexchange.com/questions/31166/how-to-utilize-minimax-algorithm-in-checkers-game

/**
 * Constructor for the gameAlgorithm class.
 *
 * @param depth The maximum depth for the minimax algorithm.
 */
gameAlgorithm::gameAlgorithm(int depth) {
    maxDepth = depth;
}

/**
 * Finds the best move for the current game state.
 *
 * @param game The current game state.
 */
void gameAlgorithm::getBestMove(gameHandler &game) {
    std::vector<std::vector<pos>> legalMoves;
    legalMoves = generateMovesList(game);
    int val = 0;
    int minValue = 0, maxValue = 0;
    int minValueIndex = 0, maxValueIndex = 0;

    gameState whoseTurn = game.getCurrentGameState();;

    if (legalMoves.empty() && whoseTurn == WHITE_TURN) {
        game.curentGameState = B_WIN;
        return;
    }
    if (legalMoves.empty() && whoseTurn == BLACK_TURN) {
        game.curentGameState = W_WIN;
        return;
    }

    bool isMaxPlayer = true;

    if(whoseTurn == BLACK_TURN)
        isMaxPlayer = false;
    if(whoseTurn == WHITE_TURN)
        isMaxPlayer = true;

    // alpha = -inf (worst case for max player white)
    // beta = inf (worst case for min player black)
    for (int i = 0; i < legalMoves.size(); i++) {
        game.currentMoves = legalMoves[i];
        val = minMAxAlgo_3(game, 0, -INT_MAX, INT_MAX, isMaxPlayer);

        if (val < minValue) {
            minValue = val;
            minValueIndex = i;
        }
        if (val > maxValue) {
            maxValue = val;
            maxValueIndex = i;
        }
    }

    if (!isMaxPlayer)
        game.currentMoves = legalMoves[minValueIndex];
    if (isMaxPlayer)
        game.currentMoves = legalMoves[maxValueIndex];
}

/**
 * Generates a list of possible moves for the current game state.
 *
 * @param curGame The current game state.
 * @return A vector of possible moves.
 */
std::vector<std::vector<pos>> gameAlgorithm::generateMovesList(gameHandler curGame) {
    std::vector<pos> piecesPos, emptyFields;
    std::vector<std::vector<pos>> legalMoves;
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

    if (moveFound == false) {
        legalMoves.clear();
        return legalMoves;
    }

    return legalMoves;
}

/**
 * Gets the positions of the current player's pieces.
 *
 * @param curGame The current game state.
 * @return A vector of positions of the current player's pieces.
 */
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

/**
 * Gets the positions of empty fields on the board.
 *
 * @param curGame The current game state.
 * @return A vector of positions of empty fields.
 */
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

/**
 * Calculates the score of the board for the current game state.
 *
 * @param board The game board.
 * @return The score of the board.
 */
int gameAlgorithm::calculateBoard(std::vector<std::vector<field>> board) {
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

    // min - black, max - white
    return whiteCtr - blackCtr;

}

/**
 * Minimax algorithm implementation without alpha-beta pruning.
 *
 * @param depth The current depth of the search.
 * @param curGame The current game state.
 * @param playerColor The color of the player to move.
 * @return The evaluation score of the board.
 */
float gameAlgorithm::minMAxAlgo_1(int depth, gameHandler curGame, gameState playerColor) {
    // sprawdzanie głębokosci
    if (depth >= maxDepth) {
        return calculateBoard(curGame.getBoard());
    }

    // zwracanie dla końca gry
    if (curGame.curentGameState != WHITE_TURN && curGame.curentGameState != BLACK_TURN)
        return calculateBoard(curGame.getBoard());

    std::vector<std::vector<pos>> moves = generateMovesList(curGame);

    // sprawdzanie czy były jakieś legalne ruchy
    if (moves.empty())
        return calculateBoard(curGame.getBoard());

    // min - black, max - white
    gameState nextPlayerColor;
    float minEval, maxEval;

    // if min player
    if (playerColor == BLACK_TURN) {
        minEval = FLT_MAX;
        nextPlayerColor = WHITE_TURN;
    }

    // if max player
    if (playerColor == WHITE_TURN) {
        maxEval = -FLT_MAX;
        nextPlayerColor = BLACK_TURN;
    }

    // sprawdzanie kolejnych ruchów dla każdego przypadku
    for (int i = 0; i < moves.size(); i++) {
        gameHandler newGame = curGame;
        newGame.currentMoves = moves[i];
        newGame.handleNextMoves();
        newGame.isGameFinished();

        float eval = minMAxAlgo_1(depth + 1, newGame, nextPlayerColor);

        maxEval = std::max(eval, maxEval);
        minEval = std::min(eval, minEval);
    }

    if (playerColor == BLACK_TURN) {
        return minEval;
    }

    if (playerColor == WHITE_TURN) {
        return maxEval;
    }

    return 0.0f; // Default return value to avoid compiler warnings
}

/**
 * Minimax algorithm implementation with alpha-beta pruning.
 *
 * @param depth The current depth of the search.
 * @param curGame The current game state.
 * @param playerColor The color of the player to move.
 * @param alpha The alpha value for alpha-beta pruning.
 * @param beta The beta value for alpha-beta pruning.
 * @return The evaluation score of the board.
 */
float gameAlgorithm::minMAxAlgo_2(int depth, gameHandler curGame, gameState playerColor, float alpha, float beta) {
    // sprawdzanie głębokosci
    if (depth >= maxDepth) {
        return calculateBoard(curGame.getBoard());
    }

    // zwracanie dla końca gry
    if (curGame.curentGameState != WHITE_TURN && curGame.curentGameState != BLACK_TURN)
        return calculateBoard(curGame.getBoard());

    std::vector<std::vector<pos>> moves = generateMovesList(curGame);

    // sprawdzanie czy były jakieś legalne ruchy
    if (moves.empty())
        return calculateBoard(curGame.getBoard());

    // min - black, max - white
    gameState nextPlayerColor;
    float minEval, maxEval;

    // if min player
    if (playerColor == BLACK_TURN) {
        minEval = FLT_MAX;
        nextPlayerColor = WHITE_TURN;
    }

    // if max player
    if (playerColor == WHITE_TURN) {
        maxEval = -FLT_MAX;
        nextPlayerColor = BLACK_TURN;
    }

    // sprawdzanie kolejnych ruchów dla każdego przypadku
    for (int i = 0; i < moves.size(); i++) {
        gameHandler newGame = curGame;
        newGame.currentMoves = moves[i];
        newGame.handleNextMoves();
        newGame.isGameFinished();

        float eval = minMAxAlgo_2(depth + 1, newGame, nextPlayerColor, alpha, beta);

        // min player - beta pruning
        if (playerColor == BLACK_TURN) {
            minEval = std::min(eval, minEval);
            beta = std::min(beta, eval);
            if (beta <= alpha)
                break;
        }

        // max player - alpha pruning
        if (playerColor == WHITE_TURN) {
            maxEval = std::max(eval, maxEval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha)
                break;
        }
    }

    // min player
    if (playerColor == BLACK_TURN) {
        return minEval;
    }

    // max player
    if (playerColor == WHITE_TURN) {
        return maxEval;
    }

    return 0.0f; // Default return value to avoid compiler warnings
}

/**
 * Minimax algorithm implementation with alpha-beta pruning and an additional maximizingPlayer flag.
 *
 * @param curGame The current game state.
 * @param depth The current depth of the search.
 * @param alpha The alpha value for alpha-beta pruning.
 * @param beta The beta value for alpha-beta pruning.
 * @param maximizingPlayer True if the current player is the maximizing player.
 * @return The evaluation score of the board.
 */
int gameAlgorithm::minMAxAlgo_3(gameHandler curGame, int depth, int alpha, int beta, bool maximizingPlayer) {
    // sprawdzanie głębokosci
    if (depth >= maxDepth) {
        return calculateBoard(curGame.getBoard());
    }

    // zwracanie dla końca gry
    if (curGame.curentGameState == W_WIN || curGame.curentGameState == B_WIN || curGame.curentGameState == DRAW)
        return calculateBoard(curGame.getBoard());

    std::vector<std::vector<pos>> moves = generateMovesList(curGame);

    // sprawdzanie czy były jakieś legalne ruchy
    if (moves.empty())
        return calculateBoard(curGame.getBoard());

    if (maximizingPlayer == true) {
        int maxEval = -INT_MAX;

        for (int i = 0; i < moves.size(); i++) {
            gameHandler newGame = curGame;
            newGame.currentMoves = moves[i];
            newGame.handleNextMoves();
            newGame.isGameFinished();

            int eval = minMAxAlgo_3(newGame, depth + 1, alpha, beta, false);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);

            if (beta <= alpha)
                break;
        }

        return maxEval;
    } else {
        int minEval = INT_MAX;

        for (int i = 0; i < moves.size(); i++) {
            gameHandler newGame = curGame;
            newGame.currentMoves = moves[i];
            newGame.handleNextMoves();
            newGame.isGameFinished();

            int eval = minMAxAlgo_3(newGame, depth + 1, alpha, beta, true);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);

            if (beta <= alpha)
                break;
        }

        return minEval;
    }
}

/**
 * Plays the game by alternating turns until the game ends.
 */
void gameAlgorithm::play() {
    game.printBoard();

    while (game.curentGameState == BLACK_TURN || game.curentGameState == WHITE_TURN) {
        if (game.roundsCtr > 0) {
            game.isGameFinished();
        }

        if (game.curentGameState == BLACK_TURN) {
            // game.askNextMove();

            std::cout << "Czarne: random" << std::endl;
            game.randomMoves();

            // std::cout << "Czarne: min-max" << std::endl;
            // getBestMove(game);
        } else if (game.curentGameState == WHITE_TURN) {
            // askNextMove();

            std::cout << "Białe: min-max" << std::endl;
            getBestMove(game);
        }

        game.handleNextMoves();
        game.printBoard();

        game.roundsCtr++;
    }

    if (game.curentGameState == B_WIN)
        std::cout << "KONIEC GRY - czarne wygrywają " << game.roundsCtr << std::endl << std::endl;
    else if (game.curentGameState == W_WIN)
        std::cout << "KONIEC GRY - białe wygrywają " << game.roundsCtr << std::endl << std::endl;
    else
        std::cout << "KONIEC GRY - remis" << std::endl << std::endl;
}
