//
// Created by michal on 23.05.24.
//

#ifndef GAMEALGORITHM_H
#define GAMEALGORITHM_H
#include "gameHandler.h"


class gameAlgorithm
{
private:
    gameHandler game;
    int maxDepth;

    std::vector<pos> getPlayerFields(gameHandler curGame);

    std::vector<pos> getEmptyFields(gameHandler curGame);

    int calculateBoard(std::vector<std::vector<field> > board);

    std::vector<std::vector<pos> > generateMovesList(gameHandler curGame);

    float evaluatePositionRecursive(int depth, gameHandler curGame, float signFactor);

    // std::vector<pos> getBestMove(const gameHandler curGame);
    void getBestMove(gameHandler& game);

public:
    gameAlgorithm(int depth);

    ~gameAlgorithm() = default;

    void play();
};


#endif //GAMEALGORITHM_H
