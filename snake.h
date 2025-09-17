#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <map>
#include <deque>
#include <algorithm>
using namespace std;
using std::chrono::system_clock;
using namespace std::this_thread;

char direction = 'r';
bool paused = false; // new pause flag

void input_handler()
{
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    map<string, char> keymap = {
        {"\x1b[C", 'r'}, // right arrow
        {"\x1b[D", 'l'}, // left arrow
        {"\x1b[A", 'u'}, // up arrow
        {"\x1b[B", 'd'}  // down arrow
    };

    while (true)
    {
        char c1 = getchar();

        // Toggle pause with 'p'
        if (c1 == 'p' || c1 == 'P')
        {
            paused = !paused;
            continue;
        }

        if (c1 == '\x1b')
        {
            char c2 = getchar();
            char c3 = getchar();
            string seq = string() + c1 + c2 + c3;
            if (keymap.find(seq) != keymap.end())
            {
                direction = keymap[seq];
            }
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

pair<int, int> generate_food(int size, const deque<pair<int, int>> &snake, pair<int, int> other = {-1, -1})
{
    while (true)
    {
        pair<int, int> f = make_pair(rand() % size, rand() % size);
        if (find(snake.begin(), snake.end(), f) == snake.end() && f != other)
        {
            return f;
        }
    }
}

void render_game(int size, deque<pair<int, int>> &snake, pair<int, int> food, pair<int, int> poison)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (i == food.first && j == food.second)
                cout << "🟧";
            else if (i == poison.first && j == poison.second)
                cout << "❌";
            else if (make_pair(i, j) == snake.back())
            {
                if (direction == 'r')
                    cout << "👉";
                else if (direction == 'l')
                    cout << "👈";
                else if (direction == 'u')
                    cout << "👆";
                else if (direction == 'd')
                    cout << "👇";
            }
            else if (find(snake.begin(), snake.end(), make_pair(i, j)) != snake.end())
                cout << "🟦";
            else
                cout << "⬜";
        }
        cout << endl;
    }
}

pair<int, int> get_next_head(pair<int, int> current, char direction)
{
    pair<int, int> next;
    if (direction == 'r')
        next = make_pair(current.first, (current.second + 1) % 10);
    else if (direction == 'l')
        next = make_pair(current.first, current.second == 0 ? 9 : current.second - 1);
    else if (direction == 'd')
        next = make_pair((current.first + 1) % 10, current.second);
    else if (direction == 'u')
        next = make_pair(current.first == 0 ? 9 : current.first - 1, current.second);

    return next;
}

int update_score(int current_score)
{
    return current_score + 10;
}

void game_play()
{
    system("clear");
    deque<pair<int, int>> snake;
    snake.push_back(make_pair(0, 0));

    int game_speed = 200;
    int score = 0;

    pair<int, int> food = generate_food(10, snake);
    pair<int, int> poison = generate_food(10, snake, food);

    for (pair<int, int> head = make_pair(0, 1);; head = get_next_head(head, direction))
    {
        cout << "\033[H";

        // === Pause handling ===
        if (paused)
        {
            cout << "=== GAME PAUSED ===\nPress 'p' to resume.\n";
            sleep_for(chrono::milliseconds(200));
            continue; // skip rest of loop until resumed
        }

        // collision with self
        if (find(snake.begin(), snake.end(), head) != snake.end())
        {
            system("clear");
            cout << "Game Over (Hit yourself!)\n";
            cout << "Final Score: " << score << "\n";
            exit(0);
        }
        // hit poison
        else if (head == poison)
        {
            system("clear");
            cout << "Game Over (Ate poison)\n";
            cout << "Final Score: " << score << "\n";
            exit(0);
        }
        // eat food
        else if (head == food)
        {
            snake.push_back(head);

            // generate new food
            food = generate_food(10, snake);

            // poison moves whenever food is eaten
            poison = generate_food(10, snake, food);

            score += 10;

            if (game_speed > 100)
                game_speed -= 20;
        }
        else
        {
            snake.push_back(head);
            snake.pop_front();
        }

        render_game(10, snake, food, poison);
        cout << "Length: " << snake.size()
             << " | Speed: " << game_speed << "ms"
             << " | Score: " << score
             << " | Press 'p' to Pause/Resume"
             << "\n";

        sleep_for(chrono::milliseconds(game_speed));
    }
}
