#include "snake.h"

using namespace std;
using namespace std::this_thread;
using namespace std::chrono;

Game::Game(int size) : size(size), direction('r'), is_running(true) {
    snake.push_back({0, 0});
    food = {rand() % size, rand() % size};
    spawn_food();
    size(size),
    direction('r'),
    is_running(true),
    score(0) // <-- Initialize score here
{
    snake.push_back({0, 0});
    spawn_food();
}


}
void Game::spawn_food() {
    do {
        food = {rand() % size, rand() % size};
    } while (find(snake.begin(), snake.end(), food) != snake.end());
}

void Game::run() {
    system("clear");
    thread input_thread(input_handler_thread, this);

    while (is_running) {
        cout << "\033[H";
        update();
        render();
        cout << "Score: " << score << endl; // <-- Display the score
        cout << "Length of snake: " << snake.size() << endl;
        sleep_for(milliseconds(500));
    }

    input_thread.join();

}

void Game::render() {
    cout << "╔";
    for (int i = 0; i < size; ++i) cout << "══";
    cout << "╗" << endl;

    for (int i = 0; i < size; i++) {
        cout << "║";
        for (int j = 0; j < size; j++) {
            if (i == food.first && j == food.second) {
                cout << "🍎";
            } else if (find(snake.begin(), snake.end(), make_pair(i, j)) != snake.end()) {
                if (make_pair(i, j) == snake.back()) {
                    cout << "😀";
                } else {
                    cout << "🟩";
                }
            } else {
                cout << "  ";
            }
        }
        cout << "║" << endl;
    }

    cout << "╚";
    for (int i = 0; i < size; ++i) cout << "══";
    cout << "╝" << endl;
}

void Game::update() {
    pair<int, int> next_h = get_next_head();
    snake.push_back(next_h);


    if (check_collision()) {
        game_over();
        return;
    }

    if (next_h.first == food.first && next_h.second == food.second) {
        food = {rand() % size, rand() % size};
        score += 10; // <-- Increment the score
        spawn_food(); 

    } else {
        snake.pop_front();
    }
}

bool Game::check_collision() {
    pair<int, int> head = snake.back();
    // Check self collision
    for (auto it = snake.begin(); it != snake.end() - 1; ++it) {
        if (*it == head) {
            return true;
        }
    }
    return false;
}

void Game::game_over() {
    is_running = false;
    system("clear");
    cout << "===========================" << endl;
    cout << "        Game Over" << endl;
    cout << "===========================" << endl;
    cout << "     Your score: " << score << endl;
    cout << "===========================" << endl;
}

pair<int, int> Game::get_next_head() {
    pair<int, int> current_head = snake.back();
    pair<int, int> next_head;

    if (direction == 'r') {
        next_head = {current_head.first, (current_head.second + 1) % size};
    } else if (direction == 'l') {
        next_head = {current_head.first, (current_head.second == 0) ? size - 1 : current_head.second - 1};
    } else if (direction == 'd') {
        next_head = {(current_head.first + 1) % size, current_head.second};
    } else if (direction == 'u') {
        next_head = {(current_head.first == 0) ? size - 1 : current_head.first - 1, current_head.second};
    }
    return next_head;
}

void input_handler_thread(Game* game) {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    map<char, char> keymap = {{'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}};

    while (game->is_running) {
        char input = getchar();
        if (keymap.count(input)) {
            game->direction = keymap[input];
        }
        if (input == 'q') {
            game->is_running = false;
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}