//
// Created by michal on 23.05.24.
//

#include "gameAlgorithm.h"

#include <cfloat>
#include <climits>
#include <utility>

/**
 * Constructor for the gameAlgorithm class.
 *
 * @param depth The maximum depth for the minimax algorithm.
 *  @param variant Game variant GUI or NET.
 *  @param playerColor Which player begins.
 *  @param depth Depth of serching in min-max algorithm.
 *  @param ip_address IP adress for playing using NET.
 *  @param ip_port IP port for playing using NET.
 */
gameAlgorithm::gameAlgorithm(interface variant, color playerColor, int depth, std::string ip_address,
                             std::string ip_port) {
    this->variant = variant;
    this->playerColor = playerColor;
    this->depth = depth;
    this->ip_address = std::move(ip_address);
    this->ip_port = std::move(ip_port);
    roundsCtr = 0;
}

gameAlgorithm::gameAlgorithm(int argc, char *argv[]) {
    roundsCtr = 0;

    // arg 1 - interference
    std::string arg1 = argv[1];
    if (arg1 == "NET") {
        variant = NET;
    } else if (arg1 == "GUI") {
        variant = GUI;
    } else {
    }

    // arg 2 - player color
    std::string arg2 = argv[2];
    if (arg2 == "WHITE" || arg2 == "white") {
        playerColor = WHITE;
    } else if (arg2 == "BLACK" || arg2 == "black") {
        playerColor = BLACK;
    } else {
    }

    // arg 3 - min_max depth
    try {
        depth = std::stoi(argv[3]);
    } catch (const std::invalid_argument &e) {
    }
}

/**
 * Struct helpful in getBestMove() function.
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

        val = minMAxAlgo(*this, 0, -INT_MAX, INT_MAX, isMaxPlayer);

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

    if (curentGameState == DRAW)
        return 0;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if (board[x][y].pieceColor == WHITE && board[x][y].pieceKind == CHECKER)
                whiteValue += 1;
            if (board[x][y].pieceColor == WHITE && board[x][y].pieceKind == KING)
                whiteValue += 2;
            if (board[x][y].pieceColor == BLACK && board[x][y].pieceKind == CHECKER)
                blackValue += 1;
            if (board[x][y].pieceColor == BLACK && board[x][y].pieceKind == KING)
                blackValue += 2;
        }
    }

    // min - black, max - white
    return whiteValue - blackValue;
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
int gameAlgorithm::minMAxAlgo(gameAlgorithm curGame, int depth, int alpha, int beta, bool maximizingPlayer) {
    // sprawdzanie dla osiągniętej głębokosci
    if (depth >= this->depth) {
        return curGame.calculateBoard();
    }

    // zwracanie dla końca gry
    curGame.isGameFinished();
    if (curGame.curentGameState == W_WIN || curGame.curentGameState == B_WIN || curGame.curentGameState == DRAW)
        return curGame.calculateBoard();

    // generowanie listy ruchów
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

            // wywołanie algorytmu dla kolejnej planszy
            int eval = minMAxAlgo(newGame, depth + 1, alpha, beta, false);

            // alpha cięcie
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);

            // zwracanie dla ostrego cięcia
            if (beta < alpha)
                break;
        }

        return maxEval;
    } else {
        int minEval = INT_MAX;

        for (int i = 0; i < moves.size(); i++) {
            gameAlgorithm newGame = curGame;
            newGame.setCurrentMoves(moves[i]);
            newGame.handleNextMoves();

            int eval = minMAxAlgo(newGame, depth + 1, alpha, beta, true);

            // beta cięcie
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);

            // zwracanie dla ostrego cięcia
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
    std::cout << std::endl;

    while (curentGameState == BLACK_TURN || curentGameState == WHITE_TURN) {
        if (curentGameState == BLACK_TURN && playerColor == BLACK)
            askNextMove();

        else if (curentGameState == WHITE_TURN && playerColor == WHITE)
            askNextMove();

        else if (curentGameState == BLACK_TURN && playerColor == WHITE) {
            std::cout << " === Czarne: min-max ===" << std::endl;
            getBestMove();
        }

        else if (curentGameState == WHITE_TURN && playerColor == BLACK) {
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
