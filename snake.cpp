#include "snake.h"

using namespace std;
using namespace std::this_thread;
using namespace std::chrono;

Game::Game(int size) : size(size), direction('r'), is_running(true) {
    snake.push_back({0, 0});
    food = {rand() % size, rand() % size};
    load_high_scores(); 
    spawn_food();
    size(size),
    direction('r'),
    is_running(true),
    is_paused(false), 
    score(0) // <-- Initialize score here
    game_speed_ms(500) // <-- Initialize speed (500ms)

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
        if (is_paused) {
            render(); // Re-render the game to show the pause message
            cout << "         PAUSED" << endl;
            cout << "Press 'p' to continue" << endl;
            sleep_for(milliseconds(100));
            continue;
        }
        update();
        render();
        cout << "Score: " << score << endl; // <-- Display the score
        cout << "Length of snake: " << snake.size() << endl;
        sleep_for(game_speed_ms); // <-- Use the variable here
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
            std::pair<int, int> current_pos = {i, j};
            if (current_pos == food) {
                cout << "🍎";
            } else if (find(poison.begin(), poison.end(), current_pos) != poison.end()) {
                cout << "☠️ "; // <-- Render poison
            } else if (find(snake.begin(), snake.end(), current_pos) != snake.end()) {
                // ... snake rendering logic ...
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
        score += 10;
        spawn_food();
        if (score > 0 && score % 30 == 0) {
            spawn_poison();
        }

        // Speed up the game every 20 points
        if ((score % 20 == 0) && (game_speed_ms.count() > 150)) {
            game_speed_ms -= std::chrono::milliseconds(50);
        }
    } else {
        snake.pop_front();
    }

}
void Game::spawn_poison() {
    std::pair<int, int> new_poison_pos;
    do {
        new_poison_pos = {rand() % size, rand() % size};
    } while (
        // Ensure it's not on the snake
        find(snake.begin(), snake.end(), new_poison_pos) != snake.end() ||
        // Ensure it's not on the regular food
        new_poison_pos == food
    );
    poison.push_back(new_poison_pos);
}

bool Game::check_collision() {
    pair<int, int> head = snake.back();
    // Check self collision
    for (auto it = snake.begin(); it != snake.end() - 1; ++it) {
        if (*it == head) {
            return true;
        }
    }
        // Check poison collision
    if (find(poison.begin(), poison.end(), head) != poison.end()) {
        return true;
    }

    return false;
}

void Game::game_over() {
    is_running = false;
    high_scores.push_back(score);
    sort(high_scores.rbegin(), high_scores.rend());
    if (high_scores.size() > 10) {
        high_scores.resize(10);
    }
    save_high_scores();
    system("clear");
    cout << "===========================" << endl;
    cout << "        Game Over" << endl;
    cout << "===========================" << endl;
    cout << "     Your score: " << score << endl;
    cout << "===========================" << endl;
    cout << "      -- High Scores --" << endl;
    for (int i = 0; i < high_scores.size(); ++i) {
        cout << "      " << i + 1 << ". " << high_scores[i] << endl;
    }

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
        } else if (input == 'p') { // <-- Add pause logic
            game->is_paused = !game->is_paused;
        } else if (input == 'q') {
            game->is_running = false;
        }
    }


    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}
void Game::load_high_scores() {
    ifstream file("high_scores.txt");
    if (file.is_open()) {
        int score;
        while (file >> score) {
            high_scores.push_back(score);
        }
        file.close();
    }
}
void Game::save_high_scores() {
    ofstream file("high_scores.txt");
    if (file.is_open()) {
        for (int i = 0; i < high_scores.size(); ++i) {
            file << high_scores[i] << endl;
        }
        file.close();
    }
}
