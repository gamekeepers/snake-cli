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
        if (keymap.find(input) != keymap.end())
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

void render_game(int size, deque<pair<int, int>> &snake, pair<int, int> food)
{
    for (size_t i = 0; i < size; i++)
    {
        for (size_t j = 0; j < size; j++)
        {
            if (i == food.first && j == food.second)
            {
                cout << "🍎";
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

void game_play()
{
    int sleep_time = 500; // milliseconds
    system("clear");
    deque<pair<int, int>> snake;
    snake.push_back(make_pair(0, 0));

    pair<int, int> food;
    do
    {
        food = make_pair(rand() % 10, rand() % 10);
    } while (find(snake.begin(), snake.end(), food) != snake.end());
    for (pair<int, int> head = make_pair(0, 1);; head = get_next_head(head, direction))
    {
        // send the cursor to the top
        cout << "\033[H";
        // check self collision
        if (find(snake.begin(), snake.end(), head) != snake.end())
        {
            system("clear");
            cout << "Game Over" << endl;
            exit(0);
        }
        else if (head.first == food.first && head.second == food.second)
        {
            // grow snake
            do
            {
                food = make_pair(rand() % 10, rand() % 10);
            } while (find(snake.begin(), snake.end(), food) != snake.end());
            snake.push_back(head);
            if (sleep_time > 100)
            {
                sleep_time = sleep_time - 25;
            }
        }
        else
        {
            // move snake
            snake.push_back(head);
            snake.pop_front();
        }
        render_game(10, snake, food);
        cout << "length of snake: " << snake.size() << endl;
        cout << "speed: " << 600 - sleep_time << " moves/sec" << endl;

        sleep_for(chrono::milliseconds(sleep_time));
    }
}
