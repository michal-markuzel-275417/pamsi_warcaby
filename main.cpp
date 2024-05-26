#include <iostream>
#include "checkersBoard.h"
#include "gameAlgorithm.h"
#include "gameHandler.h"

// prog interface turn depth random_seed ip-address ip-port

int main()
{
    gameAlgorithm gra(10);
    gra.play();

    return 0;
}

