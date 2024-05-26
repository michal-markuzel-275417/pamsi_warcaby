#include <iostream>
#include "checkersBoard.h"
#include "gameAlgorithm.h"
#include "gameHandler.h"

// prog interface turn depth random_seed ip-address ip-port

int main(int argc, char *argv[]) {
    interface variant;
    color playerColor;
    int depth;
    int random_seed; // ignore
    std::string ip_address;
    std::string ip_port;

    if (argc < 7)
        return 0;

    // arg 1 - interference
    std::string arg1 = argv[1];
    if (arg1 == "NET") {
        variant = NET;
    } else if (arg1 == "GUI") {
        variant = GUI;
    } else {
        return 0;
    }

    // arg 2 - player color
    std::string arg2 = argv[2];
    if (arg2 == "WHITE") {
        playerColor = WHITE;
    } else if (arg2 == "BLACK") {
        playerColor = BLACK;
    } else {
        return 0;
    }

    // arg 3 - min_max depth
    try {
        depth = std::stoi(argv[3]);
    } catch (const std::invalid_argument &e) {
        return 0;
    }

    // arg 4 - random seed - ignore
    //int random_seed;

    // arg 5 - ip_adress
    ip_address = argv[5];

    // arg 6 - ip_port
    ip_port = argv[6];

    gameAlgorithm gra(variant, playerColor, depth, ip_address, ip_port);
    gra.play();

    return 0;
}
