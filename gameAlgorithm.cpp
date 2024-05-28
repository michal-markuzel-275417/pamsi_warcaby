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
    this->depth = depth;
    roundsCtr = 0;
}

gameAlgorithm::gameAlgorithm(interface variant, color playerColor, int depth, std::string ip_address,
                             std::string ip_port) {
    this->variant = variant;
    this->playerColor = playerColor;
    this->depth = depth;
    this->ip_address = ip_address;
    this->ip_port = ip_port;
    roundsCtr = 0;
}

/**
 * Struct used for function below.
 */
struct evalBoards {
    int index;
    int score;
};

/**
 * Finds the best move for the current game state.
 *
 * @param game The current game state.
 */
void gameAlgorithm::getBestMove() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    std::vector<std::vector<pos> > legalMoves;
    legalMoves = generateMovesList();
    int val = 0;
    int minValue = INT_MAX, maxValue = -INT_MAX;
    int minValueIndex = 0, maxValueIndex = 0;

    if (legalMoves.empty() && curentGameState == WHITE_TURN) {
        curentGameState = B_WIN;
        return;
    }
    if (legalMoves.empty() && curentGameState == BLACK_TURN) {
        curentGameState = W_WIN;
        return;
    }

    bool isMaxPlayer = true;

    if (curentGameState == BLACK_TURN)
        isMaxPlayer = false;
    if (curentGameState == WHITE_TURN)
        isMaxPlayer = true;

    // alpha = -inf (worst case for max player white)
    // beta = inf (worst case for min player black)

    std::vector<evalBoards> valuesVector;

    for (int i = 0; i < legalMoves.size(); i++) {
        currentMoves = legalMoves[i];

        val = minMAxAlgo_3(*this, 0, -INT_MAX, INT_MAX, isMaxPlayer);

        valuesVector.push_back({i, val});

        minValue = std::min(minValue, val);
        maxValue = std::max(maxValue, val);
    }

    std::vector<int> maxIndexVector;
    std::vector<int> minIndexVector;
    for (int i = 0; i < legalMoves.size(); i++) {
        if (valuesVector[i].score == minValue)
            minIndexVector.push_back(i);

        if (valuesVector[i].score == maxValue)
            maxIndexVector.push_back(i);
    }

    // dodać losowanie ruchu dla tych samych wyników
    int randomIndex;
    if (isMaxPlayer == false) {
        randomIndex = std::rand() % minIndexVector.size();
        currentMoves = legalMoves[minIndexVector[randomIndex]];
    }
    if (isMaxPlayer == true) {
        randomIndex = std::rand() % maxIndexVector.size();
        currentMoves = legalMoves[maxIndexVector[randomIndex]];
    }
}

/**
 * Generates a list of possible moves for the current game state.
 *
 * @param curGame The current game state.
 * @return A vector of possible moves.
 */
std::vector<std::vector<pos> > gameAlgorithm::generateMovesList() {
    std::vector<pos> piecesPos, emptyFields;
    std::vector<std::vector<pos> > legalMoves;
    std::vector<pos> moves;

    bool moveFound = false;
    piecesPos = getPlayerFields();
    emptyFields = getEmptyFields();

    for (int i = 0; i < emptyFields.size(); i++) {
        for (int j = 0; j < piecesPos.size(); j++) {
            currentMoves.clear();
            currentMoves.push_back(piecesPos[j]);
            currentMoves.push_back(emptyFields[i]);

            if (isLeagalMoves()) {
                moveFound = true;
                legalMoves.push_back(currentMoves);
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
std::vector<pos> gameAlgorithm::getPlayerFields() {
    std::vector<pos> piecesPos;

    // Zbieranie pozycji figur aktualnego gracza
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if ((curentGameState == WHITE_TURN && board[x][y].pieceColor == WHITE) ||
                (curentGameState == BLACK_TURN && board[x][y].pieceColor == BLACK)) {
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
std::vector<pos> gameAlgorithm::getEmptyFields() {
    std::vector<pos> emptyFields;

    // Zbieranie pustych pól
    pos temp;
    for (int i = 1; i < 33; i++) {
        temp = notationToPos(i);

        if (board[temp.x][temp.y].pieceKind == EMPTY) {
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
int gameAlgorithm::calculateBoard() {
    int blackValue = 0;
    int whiteValue = 0;

    isGameFinished();

    if (curentGameState == W_WIN)
        return INT_MAX;

    if (curentGameState == B_WIN)
        return -INT_MAX;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if (board[x][y].pieceColor == WHITE && board[x][y].pieceKind == CHECKER)
                whiteValue += (7-y)/2;
            if (board[x][y].pieceColor == WHITE && board[x][y].pieceKind == KING)
                whiteValue += 10;
            if (board[x][y].pieceColor == BLACK && board[x][y].pieceKind == CHECKER)
                blackValue += y/2;
            if (board[x][y].pieceColor == BLACK && board[x][y].pieceKind == KING)
                blackValue += 10;
        }
    }

    // min - black, max - white
    return whiteValue - blackValue;
}

/**
 * Minimax algorithm implementation without alpha-beta pruning.
 *
 * @param depth The current depth of the search.
 * @param curGame The current game state.
 * @param playerColor The color of the player to move.
 * @return The evaluation score of the board.
 */
// float gameAlgorithm::minMAxAlgo_1(int depth, gameHandler curGame, gameState playerColor) {
//     // sprawdzanie głębokosci
//     if (depth >= this->depth) {
//         return calculateBoard();
//     }
//
//     // zwracanie dla końca gry
//     if (curGame.curentGameState != WHITE_TURN && curGame.curentGameState != BLACK_TURN)
//         return calculateBoard(curGame);
//
//     std::vector<std::vector<pos> > moves = generateMovesList(curGame);
//
//     // sprawdzanie czy były jakieś legalne ruchy
//     if (moves.empty())
//         return calculateBoard(curGame);
//
//     // min - black, max - white
//     gameState nextPlayerColor;
//     float minEval, maxEval;
//
//     // if min player
//     if (playerColor == BLACK_TURN) {
//         minEval = FLT_MAX;
//         nextPlayerColor = WHITE_TURN;
//     }
//
//     // if max player
//     if (playerColor == WHITE_TURN) {
//         maxEval = -FLT_MAX;
//         nextPlayerColor = BLACK_TURN;
//     }
//
//     // sprawdzanie kolejnych ruchów dla każdego przypadku
//     for (int i = 0; i < moves.size(); i++) {
//         gameHandler newGame = curGame;
//         newGame.currentMoves = moves[i];
//         newGame.handleNextMoves();
//         newGame.isGameFinished();
//
//         float eval = minMAxAlgo_1(depth + 1, newGame, nextPlayerColor);
//
//         maxEval = std::max(eval, maxEval);
//         minEval = std::min(eval, minEval);
//     }
//
//     if (playerColor == BLACK_TURN) {
//         return minEval;
//     }
//
//     if (playerColor == WHITE_TURN) {
//         return maxEval;
//     }
//
//     return 0.0f; // Default return value to avoid compiler warnings
// }

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
// float gameAlgorithm::minMAxAlgo_2(int depth, gameHandler curGame, gameState playerColor, float alpha, float beta) {
//     // sprawdzanie głębokosci
//     if (depth >= maxDepth) {
//         return calculateBoard(curGame);
//     }
//
//     // zwracanie dla końca gry
//     if (curGame.curentGameState != WHITE_TURN && curGame.curentGameState != BLACK_TURN)
//         return calculateBoard(curGame);
//
//     std::vector<std::vector<pos> > moves = generateMovesList(curGame);
//
//     // sprawdzanie czy były jakieś legalne ruchy
//     if (moves.empty())
//         return calculateBoard(curGame);
//
//     // min - black, max - white
//     gameState nextPlayerColor;
//     float minEval, maxEval;
//
//     // if min player
//     if (playerColor == BLACK_TURN) {
//         minEval = FLT_MAX;
//         nextPlayerColor = WHITE_TURN;
//     }
//
//     // if max player
//     if (playerColor == WHITE_TURN) {
//         maxEval = -FLT_MAX;
//         nextPlayerColor = BLACK_TURN;
//     }
//
//     // sprawdzanie kolejnych ruchów dla każdego przypadku
//     for (int i = 0; i < moves.size(); i++) {
//         gameHandler newGame = curGame;
//         newGame.currentMoves = moves[i];
//         newGame.handleNextMoves();
//         newGame.isGameFinished();
//
//         float eval = minMAxAlgo_2(depth + 1, newGame, nextPlayerColor, alpha, beta);
//
//         // min player - beta pruning
//         if (playerColor == BLACK_TURN) {
//             minEval = std::min(eval, minEval);
//             beta = std::min(beta, eval);
//             if (beta <= alpha)
//                 break;
//         }
//
//         // max player - alpha pruning
//         if (playerColor == WHITE_TURN) {
//             maxEval = std::max(eval, maxEval);
//             alpha = std::max(alpha, eval);
//             if (beta <= alpha)
//                 break;
//         }
//     }
//
//     // min player
//     if (playerColor == BLACK_TURN) {
//         return minEval;
//     }
//
//     // max player
//     if (playerColor == WHITE_TURN) {
//         return maxEval;
//     }
//
//     return 0.0f; // Default return value to avoid compiler warnings
// }

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
int gameAlgorithm::minMAxAlgo_3(gameAlgorithm curGame, int depth, int alpha, int beta, bool maximizingPlayer) {
    // sprawdzanie głębokosci
    if (depth >= this->depth) {
        return curGame.calculateBoard();
    }

    // zwracanie dla końca gry
    isGameFinished();
    if (curGame.curentGameState == W_WIN || curGame.curentGameState == B_WIN || curGame.curentGameState == DRAW)
        return curGame.calculateBoard();

    std::vector<std::vector<pos> > moves = generateMovesList();

    // sprawdzanie czy były jakieś legalne ruchy
    if (moves.empty())
        return curGame.calculateBoard();

    if (maximizingPlayer == true) {
        int maxEval = -INT_MAX;

        for (int i = 0; i < moves.size(); i++) {
            gameAlgorithm newGame = curGame;
            newGame.setCurrentMoves(moves[i]);
            newGame.handleNextMoves();

            int eval = minMAxAlgo_3(newGame, depth + 1, alpha, beta, false);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);

            // ostre cięcie
            if (beta <= alpha)
                break;
        }

        return maxEval;
    } else {
        int minEval = INT_MAX;

        for (int i = 0; i < moves.size(); i++) {
            gameAlgorithm newGame = curGame;
            newGame.setCurrentMoves(moves[i]);
            newGame.handleNextMoves();
            newGame.isGameFinished();

            int eval = minMAxAlgo_3(newGame, depth + 1, alpha, beta, true);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);

            // ostre cięcie
            if (beta < alpha)
                break;
        }

        return minEval;
    }
}

/**
 * Plays the game by alternating turns until the game ends.
 */
void gameAlgorithm::play() {
    std::cout << "== Początkowa plansza ==" << std::endl;
    printBoard();

    while (curentGameState == BLACK_TURN || curentGameState == WHITE_TURN) {
        if (curentGameState == BLACK_TURN) {
            // game.askNextMove();

            // std::cout << "Czarne: random" << std::endl;
            // randomMoves();

            std::cout << " === Czarne: min-max ===" << std::endl;
            getBestMove();

        } else if (curentGameState == WHITE_TURN) {
            // askNextMove();

            std::cout << " === Białe: min-max ===" << std::endl;
            getBestMove();
        }

        handleNextMoves();
        printBoard();
        std::cout << std::endl;
        isGameFinished();

        roundsCtr++;
    }

    std::cout << " ================ Końcowa plansza ================" << std::endl;
    printBoard();

    if (curentGameState == B_WIN)
        std::cout << " > KONIEC GRY - czarne wygrywają " << std::endl << std::endl;
    else if (curentGameState == W_WIN)
        std::cout << " > KONIEC GRY - białe wygrywają " << std::endl << std::endl;
    else
        std::cout << " > KONIEC GRY - remis" << std::endl << std::endl;
}
