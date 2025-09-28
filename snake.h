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
char direction = 'r';

chrono::duration sleep_time = 500ms;
int score = 10;
bool is_paused = false;
vector<int> top_scores;
bool waiting_for_restart = false;

void game_play();

bool is_opposite_direction(char current, char next) {
    return (current == 'r' && next == 'l') ||
           (current == 'l' && next == 'r') ||
           (current == 'u' && next == 'd') ||
           (current == 'd' && next == 'u');
}

void input_handler()
{
    // change terminal settings
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    // turn off canonical mode and echo
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    map<char, char> keymap = {{'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}, {'q', 'q'}};
    while (true)
    {
        if (waiting_for_restart) continue; // Ignore input during restart prompt
        char input = getchar();
        if (input == 'p') {
            is_paused = !is_paused; // Toggle pause state
        }
        else if (keymap.find(input) != keymap.end())
        {
            // This now correctly modifies the single, shared 'direction' variable
            char next_dir = keymap[input];
            if (!is_opposite_direction(direction, next_dir)) {
                direction = next_dir;
            }
        }
        else if (input == 'q')
        {
            exit(0);
        }
        // You could add an exit condition here, e.g., if (input == 'q') break;
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void render_game(int size, deque<pair<int, int>> &snake, pair<int, int> food, pair<int, int> poisonousFood)
{
    for (size_t i = 0; i < size; i++)
    {
        for (size_t j = 0; j < size; j++)
        {
            if (i == food.first && j == food.second)
            {
                cout << "🍎";
            }
            else if (i == poisonousFood.first && j == poisonousFood.second)
            {
                cout << "💀";
            }
            else if (find(snake.begin(), snake.end(), make_pair(int(i), int(j))) != snake.end())
            {
                cout << "🐍";
            }
            else
            {
                cout << "⬜";
            }
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
    if (snake.size() % 10 == 0 && sleep_time > 100ms)
    {
        sleep_time -= 50ms;
    }
}

pair<int, int> get_food(deque<pair<int, int>> &snake)
{
    pair<int, int> food = make_pair(rand() % 10, rand() % 10);
    while (find(snake.begin(), snake.end(), food) != snake.end())
    {
        food = make_pair(rand() % 10, rand() % 10);
    }
    return food;
}

void increase_score()
{
    score += 10;
}

pair<int, int> get_poisonous_food(deque<pair<int, int>> &snake, pair<int, int> food)
{
    pair<int, int> pfood = make_pair(rand() % 10, rand() % 10);
    while (find(snake.begin(), snake.end(), pfood) != snake.end() || food == pfood)
    {
        pfood = make_pair(rand() % 10, rand() % 10);
    }
    return pfood;
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
            score = 10;
            direction = 'r';
            waiting_for_restart = false;
            game_play();
            break;
        } else if (input == 'q') {
            exit(0);
        }
    }
}


void game_play()
{
    system("clear");
    deque<pair<int, int>> snake;
    snake.push_back(make_pair(0, 0));

    pair<int, int> food = get_food(snake);
    pair<int, int> poisonousFood = get_poisonous_food(snake, food);
    for (pair<int, int> head = make_pair(0, 1);;)
    {
        // send the cursor to the top
        cout << "\033[H";
        if (is_paused) {
            cout << "Game Paused. Press 'p' to resume." << endl;
            sleep_for(300ms); // Slow refresh while paused
            continue; // Skip game logic
        }
        // check self collision
        if (find(snake.begin(), snake.end(), head) != snake.end() || (head.first == poisonousFood.first && head.second == poisonousFood.second))
        {
            waiting_for_restart = true;
            system("clear");
            cout << "Game Over" << endl;
            update_top_scores(score);
            show_top_scores();
            wait_for_restart();
            return; // End current game_play loop
            // exit(0);
        }
        else if (head.first == food.first && head.second == food.second)
        {
            // grow snake
            food = get_food(snake);
            poisonousFood = get_poisonous_food(snake, food);
            snake.push_back(head);
            reduce_sleep_time(snake);
            increase_score();
        }
        else
        {
            // move snake
            snake.push_back(head);
            snake.pop_front();
        }
        render_game(10, snake, food, poisonousFood);
        cout << "length of snake: " << snake.size() << "  -  score: " << score << endl;

        sleep_for(sleep_time);
        head = get_next_head(head, direction);
    }
}
