//
// Created by michal on 23.05.24.
//

#ifndef GAMEALGORITHM_H
#define GAMEALGORITHM_H
#include "gameHandler.h"

enum interface { NET, GUI };


class gameAlgorithm : public gameHandler{
private:
    interface variant;
    color playerColor;
    int depth;
    std::string ip_address;
    std::string ip_port;
    int roundsCtr;

    std::vector<pos> getPlayerFields();

    std::vector<pos> getEmptyFields();

    int calculateBoard();

    std::vector<std::vector<pos>> generateMovesList();

    // float minMAxAlgo_1(int depth, gameHandler curGame, gameState playerColor);
    //
    // float minMAxAlgo_2(int depth, gameHandler curGame, gameState playerColor,
    //                    float alpha, float beta);

    int minMAxAlgo_3(gameAlgorithm curGame, int depth, int alpha, int beta, bool maximizingPlayer);

    void getBestMove();

public:
    gameAlgorithm(int depth);

    gameAlgorithm(interface variant, color playerColor, int depth, std::string ip_address,
                      std::string ip_port);

    ~gameAlgorithm() = default;

    gameAlgorithm &operator=(const gameAlgorithm &other) = default;

    void play();
};


#endif //GAMEALGORITHM_H
