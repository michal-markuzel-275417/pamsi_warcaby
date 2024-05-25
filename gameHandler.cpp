//
// Created by michal on 15.05.24.
//

#include "gameHandler.h"

gameHandler::gameHandler() : roundsCtr(0) {
    curentGameState = BLACK_TURN;

    whiteKingMoves = 0;
    blackKingMoves = 0;
}

/**
 * Prompts the user for the next move and updates the currentMoves vector.
 */
void gameHandler::askNextMove() {
    std::string input;

    if (curentGameState == BLACK_TURN)
        std::cout << "Ruch czarnych: ";
    if (curentGameState == WHITE_TURN)
        std::cout << "Ruch białych: ";
    std::cin >> input;

    currentMoves = notationToPos(readInput(input));
}

/**
 * Reads input string of moves in notation form and converts them to positions.
 *
 * @param input The string containing the moves in notation form.
 * @return A vector of integers representing the moves.
 */
std::vector<int> gameHandler::readInput(std::string &input) {
    std::vector<int> moves;
    std::istringstream iss(input);
    std::string number;
    int num;

    while (std::getline(iss, number, 'x')) {
        std::istringstream(number) >> num;
        moves.push_back(num);
    }

    return moves;
}

/**
 * Calculates the orientation between two positions.
 *
 * @param pos_1 The first position.
 * @param pos_2 The second position.
 * @return The orientation between the two positions.
 */
orientation gameHandler::calculateOrientation(pos pos_1, pos pos_2) {
    if ((pos_1.y - pos_2.y) == 0 || (pos_1.x - pos_2.x) == 0)
        return WRONG;

    if (pos_1.x < pos_2.x && pos_1.y < pos_2.y)
        return DOWN_R;

    if (pos_1.x > pos_2.x && pos_1.y < pos_2.y)
        return DOWN_L;

    if (pos_1.x > pos_2.x && pos_1.y > pos_2.y)
        return UP_L;

    if (pos_1.x < pos_2.x && pos_1.y > pos_2.y)
        return UP_R;

    return WRONG;
}

/**
 * Checks if the current set of moves is legal.
 *
 * @return True if the moves are legal, false otherwise.
 */
bool gameHandler::isLeagalMoves() {
    pos tempPos = {0, 0};
    pos tempPos_1 = {0, 0};
    pos tempPos_2 = {0, 0};
    field temp_piece;
    color enemy = NONE;
    color currentColor = NONE;
    pos first_pos = currentMoves[0];
    field first_field = board[first_pos.x][first_pos.y];

    if (curentGameState == WHITE_TURN) {
        enemy = BLACK;
        currentColor = WHITE;
    }
    if (curentGameState == BLACK_TURN) {
        enemy = WHITE;
        currentColor = BLACK;
    }

    // ruszanie pionkiem przeciwnika
    if (first_field.pieceColor == enemy) {
        if (debug)
            std::cerr << "Ruszanie pionkiem przeciwnika" << std::endl;
        return false;
    }

    // ruszanie w złym kierunku
    for (int i = 1; i < currentMoves.size(); i++) {
        if (!goodOrientation(currentMoves[i - 1], currentMoves[i])) {
            if (debug)
                std::cerr << "Ruszanie w złym kierunku" << std::endl;
            return false;
        }
    }

    // czy miejsca do ruszania sa puste
    for (int i = 1; i < currentMoves.size(); i++) {
        tempPos = currentMoves[i];
        if (board[tempPos.x][tempPos.y].pieceKind != EMPTY) {
            if (debug)
                std::cerr << "Docelowe pole jest zajęte" << std::endl;
            return false;
        }
    }

    // sprawdzanie skoku o więcej niż dwa
    float distance;

    for (int i = 0; i < currentMoves.size() - 1; i++) {
        tempPos_1 = currentMoves[i];
        tempPos_2 = currentMoves[i + 1];

        distance = getDistanceBetween(tempPos_1, tempPos_2);

        if (distance > 2 * sqrt(2) * 1.1) {
            if (debug)
                std::cerr << "Za duży skok" << std::endl;
            return false;
        }
    }

    // sprawdzanie bicia dla skoku
    int takingCounter = 0;

    for (int i = 1; i < currentMoves.size(); i++) {
        tempPos_1 = currentMoves[i - 1];
        tempPos_2 = currentMoves[i];
        distance = getDistanceBetween(tempPos_1, tempPos_2);
        temp_piece = getPieceBetween(tempPos_1, tempPos_2);

        if (distance > sqrt(2) && temp_piece.pieceColor != enemy) {
            if (debug)
                std::cerr << "Złe skucie, brak przeciwnika" << std::endl;
            return false;
        }
        if (distance > sqrt(2) && temp_piece.pieceColor == enemy)
            takingCounter++;
    }


    // sprawdzanie ilość ruchów jest zgodna z ilością kuć
    if (currentMoves.size() - 2 > takingCounter) {
        if (debug)
            std::cerr << "Za dużo ruchów bez kucia" << std::endl;
        return false;
    }

    // sprawdzanie czy było kucie jeśli było możliwe
    bool wasTaken = false;
    bool couldTake = false;

    std::vector<pos> fieldsToJump;
    fieldsToJump.clear();
    std::vector<pos> checkers;

    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++)
            if (board[x][y].pieceColor == currentColor)
                checkers.push_back({x, y});

    for (int i = 0; i < checkers.size() - 1; i++)
        if (canTake(checkers[i], fieldsToJump))
            couldTake = true;

    if (couldTake == true) {
        for (int i = 0; i < currentMoves.size(); i++) {
            for (int j = 0; j < fieldsToJump.size(); j++) {
                if (currentMoves[i] == fieldsToJump[j]) {
                    wasTaken = true;
                }
            }
        }
    }

    if (couldTake == true && wasTaken == false || couldTake == true && takingCounter < currentMoves.size() - 2) {
        if (debug)
            std::cerr << "Brak wykonanego kucia" << std::endl;
        return false;
    }

    return true;
}

/**
 * Returns the piece between two positions.
 *
 * @param pos_1 The first position.
 * @param pos_2 The second position.
 * @return The piece located between the two positions.
 */
field gameHandler::getPieceBetween(pos pos_1, pos pos_2) {
    int temp_x, temp_y;
    temp_x = (pos_1.x + pos_2.x) / 2;
    temp_y = (pos_1.y + pos_2.y) / 2;

    return board[temp_x][temp_y];
}

/**
 * Returns the field position between two positions.
 *
 * @param pos_1 The first position.
 * @param pos_2 The second position.
 * @return The field position located between the two positions.
 */
pos gameHandler::getFieldPosBetween(pos pos_1, pos pos_2) {
    int temp_x, temp_y;
    temp_x = (pos_1.x + pos_2.x) / 2;
    temp_y = (pos_1.y + pos_2.y) / 2;

    return {temp_x, temp_y};
}

/**
 * Converts a vector of move notations to positions.
 *
 * @param moves A vector of integers representing the moves in notation form.
 * @return A vector of positions corresponding to the moves.
 */
std::vector<pos> gameHandler::notationToPos(const std::vector<int> &moves) {
    std::vector<pos> movesPos;
    std::vector<pos> blackPositions;

    // Wypełnianie wektora współrzędnymi czarnych pól na planszy 8x8
    for (int y = 0; y < 8; ++y) {
        for (int x = (y % 2 == 0) ? 1 : 0; x < 8; x += 2) {
            blackPositions.push_back({x, y});
        }
    }

    movesPos.reserve(moves.size());
    for (int move: moves)
        movesPos.push_back(blackPositions[move - 1]);

    return movesPos;
}

/**
 * Converts a single move notation to a position.
 *
 * @param field The move in notation form.
 * @return The position corresponding to the move.
 */
pos gameHandler::notationToPos(int field) {
    std::vector<pos> blackPositions;

    // Wypełnianie wektora współrzędnymi czarnych pól na planszy 8x8
    for (int y = 0; y < 8; ++y) {
        for (int x = (y % 2 == 0) ? 1 : 0; x < 8; x += 2) {
            blackPositions.push_back({x, y});
        }
    }

    return blackPositions[field - 1];
}

/**
 * Handles the next set of moves by updating the game state and the board.
 */
void gameHandler::handleNextMoves() {
    if (!isLeagalMoves()) {
        if (curentGameState == BLACK_TURN)
            curentGameState = W_WIN;
        if (curentGameState == WHITE_TURN)
            curentGameState = B_WIN;
        return;
    }

    pos movedPiecePos = currentMoves[0];
    pos movedPieceFinalPos = currentMoves.back();
    field movedPiece = board[movedPiecePos.x][movedPiecePos.y];

    board[movedPiecePos.x][movedPiecePos.y] = {EMPTY, NONE};
    board[movedPieceFinalPos.x][movedPieceFinalPos.y] = movedPiece;

    pos tempPos_1, tempPos_2, tempPos;
    float distance;

    for (int i = 1; i < currentMoves.size(); i++) {
        tempPos_1 = currentMoves[i - 1];
        tempPos_2 = currentMoves[i];
        distance = getDistanceBetween(tempPos_1, tempPos_2);
        if (distance > 2) {
            tempPos = getFieldPosBetween(tempPos_1, tempPos_2);
            board[tempPos.x][tempPos.y] = {EMPTY, NONE};
        }
    }

    promotion();

    curentGameState = (curentGameState == BLACK_TURN) ? WHITE_TURN : BLACK_TURN;
}

/**
 * Promotes pieces to kings when they reach the opposite end of the board.
 */
void gameHandler::promotion() {
    for (int x = 0; x < 8; x++) {
        if (board[x][0].pieceColor == WHITE & board[x][0].pieceKind == CHECKER)
            board[x][0].pieceKind = KING;

        if (board[x][7].pieceColor == BLACK & board[x][7].pieceKind == CHECKER)
            board[x][7].pieceKind = KING;
    }
}

/**
 * Checks if the game has finished by counting the remaining pieces.
 */
void gameHandler::isGameFinished() {
    int blackCtr = 0;
    int whiteCtr = 0;
    pos firstPos = currentMoves[currentMoves.size() - 1];
    gameState lastState = curentGameState;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if (board[x][y].pieceColor == WHITE)
                whiteCtr++;
            if (board[x][y].pieceColor == BLACK)
                blackCtr++;
        }
    }

    if (whiteCtr == 0) {
        curentGameState = B_WIN;
        // return;
    }

    if (blackCtr == 0) {
        curentGameState = W_WIN;
        // return;
    }

    // Sprawdzenie ruchu dla białych
    if (lastState == WHITE_TURN) {
        // Sprawdzenie czy pionek to król
        if (board[firstPos.x][firstPos.y].pieceKind == KING) {
            whiteKingMoves++;
        }
        // Sprawdzenie czy pionek to zwykły pionek
        else if (board[firstPos.x][firstPos.y].pieceKind == CHECKER) {
            whiteKingMoves = 0;
        }
    }

    // Sprawdzenie ruchu dla czarnych
    if (lastState == BLACK_TURN) {
        // Sprawdzenie czy pionek to król
        if (board[firstPos.x][firstPos.y].pieceKind == KING) {
            blackKingMoves++;
        }
        // Sprawdzenie czy pionek to zwykły pionek
        else if (board[firstPos.x][firstPos.y].pieceKind == CHECKER) {
            blackKingMoves = 0;
        }
    }

    // Sprawdzenie czy liczba ruchów króla przekroczyła maksymalną dozwoloną liczbę ruchów
    if (blackKingMoves > maxKingMoves && whiteKingMoves > maxKingMoves) {
        if (debug)
            std::cerr << "Ilosc ruchów królami: biały: " << whiteKingMoves << " czarny: " << blackKingMoves <<
                    std::endl;
        curentGameState = DRAW;
    }
}

/**
 * Plays the game by alternating turns until the game ends.
 */
void gameHandler::play() {
    printBoard();
    // gameAlgorithm algoMinMax(depth);
    // std::vector<pos> moves;

    while (curentGameState == BLACK_TURN || curentGameState == WHITE_TURN) {
        if (curentGameState == BLACK_TURN) {
            askNextMove();
            // std::cout << "Czarne: random" << std::endl;
            // randomMoves();
        } else {
            // askNextMove();
            std::cout << "Białe: min-max" << std::endl;
            // moves = algoMinMax.getBestMove(this);
            // currentMoves = moves;
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

/**
 * Generates random moves for the white pieces.
 */
void gameHandler::randomMoves() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    std::vector<pos> piecesPos, emptyFields;
    std::vector<std::vector<pos> > legalMoves;

    // Zbieranie pozycji figur aktualnego gracza
    // for (int y = 0; y < 8; y++) {
    //     for (int x = 0; x < 8; x++) {
    //         if ((curentGameState == WHITE_TURN && board[x][y].pieceColor == WHITE) ||
    //             (curentGameState == BLACK_TURN && board[x][y].pieceColor == BLACK)) {
    //             piecesPos.push_back({x, y});
    //         }
    //     }
    // }

    pos temp;
    color playerColor = NONE;

    if (curentGameState != WHITE_TURN && curentGameState != BLACK_TURN)
        return;

    if (curentGameState == WHITE_TURN)
        playerColor = WHITE;

    if (curentGameState == BLACK_TURN)
        playerColor = BLACK;

    for (int i = 1; i < 33; i++) {
        temp = notationToPos(i);

        if (board[temp.x][temp.y].pieceColor == playerColor) {
            piecesPos.push_back(temp);
        }
    }


    for (int i = 1; i < 33; i++) {
        temp = notationToPos(i);

        if (board[temp.x][temp.y].pieceKind == EMPTY) {
            emptyFields.push_back(temp);
        }
    }

    // Zbieranie pustych pól
    for (int i = 1; i < 33; i++) {
        temp = notationToPos(i);

        if (board[temp.x][temp.y].pieceKind == EMPTY) {
            emptyFields.push_back(temp);
        }
    }

    bool moveFound = false;

    for (int i = 0; i < emptyFields.size(); i++) {
        for (int j = 0; j < piecesPos.size(); j++) {
            // Sprawdzanie, czy ruch jest legalny
            currentMoves.clear();
            currentMoves.push_back(piecesPos[j]);
            currentMoves.push_back(emptyFields[i]);

            if (isLeagalMoves()) {
                moveFound = true;
                legalMoves.push_back(currentMoves);
            }
        }
    }

    if (!moveFound) {
        std::cerr << "Brak możliwego ruchu" << std::endl;
        curentGameState = (curentGameState == WHITE_TURN) ? B_WIN : W_WIN;
        return;
    }

    currentMoves.clear();
    int randomIndex = std::rand() % legalMoves.size();

    for (int i = 0; i < legalMoves[randomIndex].size(); i++)
        currentMoves.push_back(legalMoves[randomIndex][i]);
}


/**
 * Calculates the distance between two positions.
 *
 * @param tempPos_1 The first position.
 * @param tempPos_2 The second position.
 * @return The distance between the two positions.
 */
float gameHandler::getDistanceBetween(pos tempPos_1, pos tempPos_2) {
    return static_cast<float>(std::sqrt(
        (tempPos_1.x - tempPos_2.x) * (tempPos_1.x - tempPos_2.x)
        + (tempPos_1.y - tempPos_2.y) * (tempPos_1.y - tempPos_2.y)
    ));
}

bool gameHandler::canTake(pos piecePos, std::vector<pos> &fieldsToJump) {
    color enemy = NONE;
    bool canTake = false;

    if (curentGameState == WHITE_TURN) {
        enemy = BLACK;
    } else if (curentGameState == BLACK_TURN) {
        enemy = WHITE;
    }

    for (int x = -1; x < 2; x += 2) {
        for (int y = -1; y < 2; y += 2) {
            if (piecePos.x + 2 * x >= 0 && piecePos.x + 2 * x < 8 && piecePos.y + 2 * y >= 0 && piecePos.y + 2 * y <
                8) {
                if (goodOrientation(piecePos, {piecePos.x + 2 * x, piecePos.y + 2 * y})) {
                    if (board[piecePos.x + x][piecePos.y + y].pieceColor == enemy) {
                        if (board[piecePos.x + 2 * x][piecePos.y + 2 * y].pieceKind == EMPTY) {
                            canTake = true;
                            fieldsToJump.push_back({piecePos.x + 2 * x, piecePos.y + 2 * y});
                        }
                    }
                }
            }
        }
    }

    return canTake;
}


bool gameHandler::goodOrientation(pos piecePos1, pos piecePos2) {
    orientation tempOrientation = calculateOrientation(piecePos1, piecePos2);

    if (tempOrientation == WRONG)
        return false;

    if (board[piecePos1.x][piecePos1.y].pieceKind == KING)
        return true;

    if (curentGameState == WHITE_TURN) {
        if (tempOrientation == DOWN_L || tempOrientation == DOWN_R)
            return false;
    }

    if (curentGameState == BLACK_TURN) {
        if (tempOrientation == UP_L || tempOrientation == UP_R)
            return false;
    }

    return true;
}

void gameHandler::setBoard(std::vector<std::vector<field> > board) {
    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++) {
            this->board[x][y] = board[x][y];
        }
}

void gameHandler::setCurretnMoves(std::vector<pos> currentMoves) {
    this->currentMoves = currentMoves;
}

std::vector<std::vector<field> > gameHandler::getBoard() {
    return board;
}

gameState gameHandler::getCurrentGameState() {
    return curentGameState;
}
