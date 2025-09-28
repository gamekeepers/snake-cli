#include "snake.h"
#include <windows.h>

int main(int argc, char *argv[]) {
    // Enable ANSI color support in Windows console
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);

    std::atomic<bool> running(true);
    std::thread input_thread(input_handler, std::ref(running));
    std::thread game_thread(game_play, std::ref(running));   
    input_thread.join();
    game_thread.join();
    return 0;
}