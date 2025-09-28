#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h> // for system clear
#include <map>
#include <deque>
#include <algorithm>
using namespace std;
using std::chrono::system_clock;
using namespace std::this_thread;

struct GameState {
    char direction = 'r';
    chrono::milliseconds sleep_time = chrono::milliseconds(500);
    int score = 10;
    bool is_paused = false;
    bool waiting_for_restart = false;
    deque<pair<int, int>> snake;
    pair<int, int> food;
    pair<int, int> poisonousFood;
};

GameState gameState;
std::vector<int> top_scores;

void game_play(GameState& state);

bool is_opposite_direction(char current, char next) {
    return (current == 'r' && next == 'l') ||
           (current == 'l' && next == 'r') ||
           (current == 'u' && next == 'd') ||
           (current == 'd' && next == 'u');
}

void setup_terminal(struct termios& oldt, struct termios& newt) {
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    // turn off canonical mode and echo
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void restore_terminal(struct termios& oldt) {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void handle_direction_input(char input, map<char, char>& keymap) {
    char next_dir = keymap[input];
    if (!is_opposite_direction(gameState.direction, next_dir)) {
        gameState.direction = next_dir;
    }
}

void input_handler(GameState& state)
{
    struct termios oldt, newt;
    setup_terminal(oldt, newt);
    map<char, char> keymap = {{'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}, {'q', 'q'}};
    while (true)
    {
        if (state.waiting_for_restart) continue;
        char input = getchar();
        if (input == 'p') {
            state.is_paused = !state.is_paused;
        }
        else if (keymap.find(input) != keymap.end())
        {
            char next_dir = keymap[input];
            if (!is_opposite_direction(state.direction, next_dir)) {
                state.direction = next_dir;
            }
        }
        else if (input == 'q')
        {
            exit(0);
        }
    }
    restore_terminal(oldt);
}

string render_cell(int i, int j, const deque<pair<int, int>>& snake, const pair<int, int>& food, const pair<int, int>& poisonousFood) {
    if (i == food.first && j == food.second)
        return "🍎";
    else if (i == poisonousFood.first && j == poisonousFood.second)
        return "💀";
    else if (find(snake.begin(), snake.end(), make_pair(i, j)) != snake.end())
        return "🐍";
    else
        return "⬜";
}

void render_game(int size, deque<pair<int, int>> &snake, pair<int, int> food, pair<int, int> poisonousFood)
{
    for (size_t i = 0; i < size; i++)
    {
        for (size_t j = 0; j < size; j++)
        {
            cout << render_cell(i, j, snake, food, poisonousFood);
        }
        cout << endl;
    }
}

pair<int, int> get_next_head(pair<int, int> current, char direction)
{
    pair<int, int> next;
    if (direction == 'r')
    {
        next = make_pair(current.first, (current.second + 1) % 10);
    }
    else if (direction == 'l')
    {
        next = make_pair(current.first, current.second == 0 ? 9 : current.second - 1);
    }
    else if (direction == 'd')
    {
        next = make_pair((current.first + 1) % 10, current.second);
    }
    else if (direction == 'u')
    {
        next = make_pair(current.first == 0 ? 9 : current.first - 1, current.second);
    }
    return next;
}


void reduce_sleep_time(deque<pair<int, int>> &snake)
{
    if (snake.size() % 10 == 0 && gameState.sleep_time > 100ms)
    {
        gameState.sleep_time -= 50ms;
    }
}

pair<int, int> get_random_position(const deque<pair<int, int>>& snake, const pair<int, int>* avoid = nullptr) {
    pair<int, int> pos;
    do {
        pos = make_pair(rand() % 10, rand() % 10);
    } while (
        find(snake.begin(), snake.end(), pos) != snake.end() ||
        (avoid && pos == *avoid)
    );
    return pos;
}

pair<int, int> get_food(deque<pair<int, int>> &snake)
{
    return get_random_position(snake);
}

void increase_score()
{
    gameState.score += 10;
}

pair<int, int> get_poisonous_food(deque<pair<int, int>> &snake, pair<int, int> food)
{
    return get_random_position(snake, &food);
}

void show_top_scores() {
    cout << "\n--- Top 10 Scores ---\n";
    for (size_t i = 0; i < top_scores.size(); ++i) {
        cout << i + 1 << ". " << top_scores[i] << endl;
    }
}

void update_top_scores(int new_score) {
    top_scores.push_back(new_score);
    sort(top_scores.rbegin(), top_scores.rend());
    if (top_scores.size() > 10) top_scores.resize(10);
}

void wait_for_restart() {
    cout << "Press 'n' to start again or 'q' to quit." << endl;
    while (true) {
        char input = getchar();
        if (input == 'n') {
            gameState.score = 10;
            gameState.direction = 'r';
            gameState.waiting_for_restart = false;
            game_play(gameState);
            break;
        } else if (input == 'q') {
            exit(0);
        }
    }
}


void handle_pause() {
    cout << "Game Paused. Press 'p' to resume." << endl;
    sleep_for(300ms);
}

bool is_collision(const deque<pair<int, int>>& snake, const pair<int, int>& head, const pair<int, int>& poisonousFood) {
    return find(snake.begin(), snake.end(), head) != snake.end() ||
           (head.first == poisonousFood.first && head.second == poisonousFood.second);
}

void handle_game_over(GameState& state) {
    gameState.waiting_for_restart = true;
    system("clear");
    cout << "Game Over" << endl;
    update_top_scores(gameState.score);
    show_top_scores();
    wait_for_restart();
}

void handle_food_eaten(deque<pair<int, int>>& snake, pair<int, int>& food, pair<int, int>& poisonousFood, const pair<int, int>& head) {
    food = get_food(snake);
    poisonousFood = get_poisonous_food(snake, food);
    snake.push_back(head);
    reduce_sleep_time(snake);
    increase_score();
}

void move_snake(deque<pair<int, int>>& snake, const pair<int, int>& head) {
    snake.push_back(head);
    snake.pop_front();
}

void display_status(const deque<pair<int, int>>& snake) {
    cout << "length of snake: " << snake.size() << "  -  score: " << gameState.score << endl;
}

void game_play(GameState& state)
{
    system("clear");
    state.snake.clear();
    state.snake.push_back(make_pair(0, 0));
    state.food = get_food(state.snake);
    state.poisonousFood = get_poisonous_food(state.snake, state.food);
    pair<int, int> head = make_pair(0, 1);

    while (true)
    {
        cout << "\033[H";
        if (state.is_paused) {
            handle_pause();
            continue;
        }
        if (is_collision(state.snake, head, state.poisonousFood)) {
            handle_game_over(state);
            return;
        }
        else if (head.first == state.food.first && head.second == state.food.second)
        {
            handle_food_eaten(state.snake, state.food, state.poisonousFood, head);
            state.score += 10;
        }
        else
        {
            move_snake(state.snake, head);
        }
        render_game(10, state.snake, state.food, state.poisonousFood);
        display_status(state.snake);

        sleep_for(state.sleep_time);
        head = get_next_head(head, state.direction);
    }
}
