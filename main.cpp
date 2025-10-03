#include "snake.h"
#include <windows.h>
#include <windows.h>
#include <iostream>

void enableANSI()
{
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD inMode = 0;
    GetConsoleMode(hIn, &inMode);
    inMode |= ENABLE_PROCESSED_INPUT;
    SetConsoleMode(hIn, inMode);

    // Get console output handle
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
        return;

    // Enable ANSI escape codes
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

int main()
{
    enableANSI();

    std::atomic<bool> running(true);

    Game game;
    game.init();

    std::thread inputThread(input_handler, std::ref(running));
    std::thread gameThread(&Game::run, &game, std::ref(running));

    inputThread.join();
    gameThread.join();
    return 0;
}
