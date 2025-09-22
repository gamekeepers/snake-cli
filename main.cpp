#include "snake.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <map>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <chrono>

using namespace std::chrono_literals;

static void render_game(int size, const std::deque<SnakeGame::Pos> &snake,
                        const SnakeGame::Pos &food, const SnakeGame::Pos &poison) {
    // Move cursor to top-left and draw
    std::cout << "\033[H";
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (i == food.first && j == food.second) {
                std::cout << "🍎";
            } else if (i == poison.first && j == poison.second) {
                std::cout << "☠️";
            } else if (std::find(snake.begin(), snake.end(), std::make_pair(i,j)) != snake.end()) {
                std::cout << "🐍";
            } else {
                std::cout << "⬜";
            }
        }
        std::cout << "\n";
    }
}

int main(int argc, char *argv[]) {
    SnakeGame game(10);
    std::atomic<char> direction('r');
    std::atomic<bool> paused(false);
    std::atomic<bool> stopFlag(false);

    // Input thread: non-blocking read from stdin (raw mode)
    std::thread input_thread([&](){
        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        // make stdin non-blocking
        int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

        std::map<char,char> keymap = { {'d','r'}, {'a','l'}, {'w','u'}, {'s','d'} };

        while (!stopFlag.load()) {
            int ch = getchar();
            if (ch == EOF) {
                std::this_thread::sleep_for(10ms);
                continue;
            }
            char c = static_cast<char>(ch);
            if (keymap.find(c) != keymap.end()) {
                direction.store(keymap[c]);
            } else if (c == 'p') {
                paused = !paused.load();
            } else if (c == 'q') {
                stopFlag = true;
                break;
            }
        }

        // restore flags and termios
        fcntl(STDIN_FILENO, F_SETFL, oldf);
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    });

    // clear screen once
    std::cout << "\033[2J\033[H";

    // main game loop
    while (!stopFlag.load()) {
        if (paused.load()) {
            std::cout << "Game Paused - press 'p' to resume\n";
            std::this_thread::sleep_for(200ms);
            continue;
        }

        auto status = game.step(direction.load());

        if (status == SnakeGame::Status::SelfCollision) {
            std::cout << "Game Over! You ate yourself 🐍\n";
            std::cout << "Final Score: " << game.getScore() << "\n";
            game.updateHighScores("highscores.txt");
            break;
        } else if (status == SnakeGame::Status::AtePoison) {
            std::cout << "Game Over! You ate poison ☠️\n";
            std::cout << "Final Score: " << game.getScore() << "\n";
            game.updateHighScores("highscores.txt");
            break;
        }

        render_game(game.getGridSize(), game.getSnake(), game.getFood(), game.getPoison());
        std::cout << "Score: " << game.getScore() << "   Length: " << game.getSnake().size()
                  << "   Speed: " << game.getSpeed() << "ms\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(game.getSpeed()));
    }

    // make sure input thread finishes
    if (input_thread.joinable()) input_thread.join();

    return 0;
}
