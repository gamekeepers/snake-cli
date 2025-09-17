#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>  // for system clear
#include <map>
#include <deque>
#include <algorithm>
using namespace std;
using std::chrono::system_clock;
using namespace std::this_thread;

char direction = 'r';
pair<int, int> poison_food = {-1, -1};  // Inactive initially

void input_handler()
{
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    map<char, char> keymap = {{'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}, {'q', 'q'}};

    while (true)
    {
        char input = getchar();
        if (keymap.find(input) != keymap.end())
        {
            direction = keymap[input];
        }
        else if (input == 'q')
        {
            exit(0);
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void render_game(int size, deque<pair<int, int>> &snake, pair<int, int> food)
{
    system("clear");
    for (size_t i = 0; i < size; i++)
    {
        for (size_t j = 0; j < size; j++)
        {
            if (i == food.first && j == food.second)
            {
                cout << "🍎";
            }
            else if (i == poison_food.first && j == poison_food.second)
            {
                cout << "💣";
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

pair<int, int> get_random_empty_cell(int size, const deque<pair<int, int>> &snake, pair<int, int> other_food)
{
    pair<int, int> pos;
    do
    {
        pos = make_pair(rand() % size, rand() % size);
    } while (find(snake.begin(), snake.end(), pos) != snake.end() || pos == other_food);
    return pos;
}

void game_play()
{
    int sleep_time = 500;           // milliseconds
    const int poison_activation_speed = 300;
    system("clear");

    deque<pair<int, int>> snake;
    snake.push_back(make_pair(0, 0));

    pair<int, int> food;
    pair<int, int> poison;

    // Initialize food and poison at the start
    do
    {
        food = make_pair(rand() % 10, rand() % 10);
    } while (find(snake.begin(), snake.end(), food) != snake.end());

    do
    {
        poison = make_pair(rand() % 10, rand() % 10);
    } while (find(snake.begin(), snake.end(), poison) != snake.end() || poison == food);

    poison_food = poison;  // Initialize global poison_food variable

    for (pair<int, int> head = make_pair(0, 1);; head = get_next_head(head, direction))
    {
        cout << "\033[H";

        // Only activate poison logic if speed is high enough
        if (sleep_time > poison_activation_speed)
        {
            poison_food = {-1, -1};  // Deactivate poison food when speed is slow
        }

        // Check self collision
        if (find(snake.begin(), snake.end(), head) != snake.end())
        {
            system("clear");
            cout << "Game Over (Self Collision)" << endl;
            exit(0);
        }
        // Check poison food eaten
        else if (head.first == poison_food.first && head.second == poison_food.second)
        {
            system("clear");
            cout << "Game Over (Poison Consumed)" << endl;
            exit(0);
        }
        // Check normal food eaten
        else if (head.first == food.first && head.second == food.second)
        {
            snake.push_back(head);

            if (sleep_time > 100)
            {
                sleep_time -= 25;
            }

            // Generate new food
            food = get_random_empty_cell(10, snake, poison_food);

            // Generate new poison food if speed is high enough
            if (sleep_time <= poison_activation_speed)
            {
                poison_food = get_random_empty_cell(10, snake, food);
            }
            else
            {
                poison_food = {-1, -1};  // Deactivate poison when slow
            }
        }
        else
        {
            snake.push_back(head);
            snake.pop_front();
        }

        render_game(10, snake, food);
        cout << "length of snake: " << snake.size() << endl;
        cout << "speed: " << 600 - sleep_time << " moves/sec" << endl;

        sleep_for(chrono::milliseconds(sleep_time));
    }
}
