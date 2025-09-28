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
        char input = getchar();
        if (input == 'p') {
            is_paused = !is_paused; // Toggle pause state
        }
        else if (keymap.find(input) != keymap.end())
        {
            // This now correctly modifies the single, shared 'direction' variable
            direction = keymap[input];
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

pair<int, int> get_poisonous_food(deque<pair<int, int>> &snake)
{
    pair<int, int> food = make_pair(rand() % 10, rand() % 10);
    while (find(snake.begin(), snake.end(), food) != snake.end())
    {
        food = make_pair(rand() % 10, rand() % 10);
    }
    return food;
}


void game_play()
{
    system("clear");
    deque<pair<int, int>> snake;
    snake.push_back(make_pair(0, 0));

    pair<int, int> food = get_food(snake);
    pair<int, int> poisonousFood = get_poisonous_food(snake);
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
            system("clear");
            cout << "Game Over" << endl;
            exit(0);
        }
        else if (head.first == food.first && head.second == food.second)
        {
            // grow snake
            food = get_food(snake);
            poisonousFood = get_poisonous_food(snake);
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
