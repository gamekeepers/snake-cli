#ifndef SNAKE_H
#define SNAKE_H

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
#include <utility>
#include <fstream>

using namespace std;
using std::chrono::system_clock;
using namespace std::this_thread;

inline char direction = 'r';
constexpr int BOARD_SIZE = 10;

inline void input_handler()
{
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    map<char, char> keymap = {{'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}};
    char lastValidDirection = 'r';

    while (true)
    {
        char input = getchar();

        if (keymap.count(input))
        {
            char newDirection = keymap[input];
            if ((newDirection == 'r' && direction != 'l') ||
                (newDirection == 'l' && direction != 'r') ||
                (newDirection == 'u' && direction != 'd') ||
                (newDirection == 'd' && direction != 'u') ||
                direction == 'P')
            {
                direction = newDirection;
                lastValidDirection = newDirection;
            }
        }
        else if (input == 'q')
        {
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            exit(0);
        }
        else if (input == 'p')
        {
            if (direction != 'P')
            {
                lastValidDirection = direction;
                direction = 'P';
            }
            else
            {
                direction = lastValidDirection;
            }
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

inline pair<int, int> get_next_head(pair<int, int> current, char direction)
{
    int next_r = current.first;
    int next_c = current.second;

    if (direction == 'r')
        next_c = (current.second + 1) % BOARD_SIZE;
    else if (direction == 'l')
        next_c = (current.second == 0) ? BOARD_SIZE - 1 : current.second - 1;
    else if (direction == 'd')
        next_r = (current.first + 1) % BOARD_SIZE;
    else if (direction == 'u')
        next_r = (current.first == 0) ? BOARD_SIZE - 1 : current.first - 1;

    return {next_r, next_c};
}

inline pair<int, int> spawn_item(const deque<pair<int, int>> &snake)
{
    vector<pair<int, int>> freeCells;
    for (int i = 0; i < BOARD_SIZE; ++i)
        for (int j = 0; j < BOARD_SIZE; ++j)
            if (find(snake.begin(), snake.end(), make_pair(i, j)) == snake.end())
                freeCells.push_back({i, j});

    if (freeCells.empty())
        return {-1, -1};

    return freeCells[rand() % freeCells.size()];
}

inline void save_score(int score)
{
    ofstream file("scores.txt", ios::app);
    if (file.is_open())
    {
        file << score << "\n";
        file.close();
    }
    else
    {
        cerr << "Warning: Could not open scores.txt for saving.\n";
    }
}

inline vector<int> load_top_scores()
{
    ifstream file("scores.txt");
    vector<int> scores;
    int s;
    while (file >> s)
        scores.push_back(s);
    file.close();

    sort(scores.begin(), scores.end(), greater<int>());

    if (scores.size() > 10)
        scores.resize(10);

    return scores;
}

inline void render_game(int size, deque<pair<int, int>> &snake, pair<int, int> food,
                        pair<int, int> poison, int score, int level)
{
    cout << "\033[H"; 
    
    for (int k = 0; k < size + 2; ++k) cout << "🧱"; 
    cout << "\n";

    for (int i = 0; i < size; ++i)
    {
        cout << "🧱";
        for (int j = 0; j < size; ++j)
        {
            pair<int, int> current_cell = {i, j};
            if (current_cell == food)
                cout << "🍎";
            else if (current_cell == poison)
                cout << "☠️";
            else if (find(snake.begin(), snake.end(), current_cell) != snake.end())
            {
                if (current_cell == snake.back())
                    cout << "🟢";
                else
                    cout << "🟩";
            }
            else
                cout << "  ";
        }
        cout << "🧱\n";
    }

    for (int k = 0; k < size + 2; ++k) cout << "🧱";
    cout << "\n\n";

    cout << "🎮 Score: " << score << " | Length: " << snake.size() << " | Speed Level: " << level << "\n";
    cout << "Controls: (W, A, S, D) | Pause: (P) | Quit: (Q)\n";
}

inline void game_play()
{
    system("clear");
    srand(time(0));

    deque<pair<int, int>> snake;
    snake.push_back({0, 0});
    snake.push_back({0, 1});

    pair<int, int> food = spawn_item(snake);
    pair<int, int> poison = spawn_item(snake);

    int score = 0;
    int level = 1;
    const int baseDelay = 300;

    while (true)
    {
        if (direction == 'P')
        {
            render_game(BOARD_SIZE, snake, food, poison, score, level);
            cout << "\n\n\t\t\t\t\t\tPAUSED (Press P to resume)";
            this_thread::sleep_for(chrono::milliseconds(100));
            continue;
        }

        pair<int, int> currentHead = snake.back();
        pair<int, int> nextHead = get_next_head(currentHead, direction);

        bool willGrow = (nextHead == food);
        bool collision = false;

        if (willGrow)
        {
            if (find(snake.begin(), snake.end(), nextHead) != snake.end())
                collision = true;
        }
        else
        {
            if (snake.size() > 1 && find(snake.begin() + 1, snake.end(), nextHead) != snake.end())
                collision = true;
        }

        if (collision)
        {
            system("clear");
            cout << "Game Over! 💥 Self-collision!\n";
            break;
        }

        if (nextHead == poison)
        {
            system("clear");
            cout << "Game Over! ☠️ Snake ate poison!\n";
            break;
        }

        snake.push_back(nextHead);

        if (willGrow)
        {
            score++;
            level = (score / 5) + 1;
            
            food = spawn_item(snake);

            if (score % 3 == 0)
                poison = spawn_item(snake);
            else if (poison.first == -1)
                poison = spawn_item(snake);

            if (food.first == -1)
            {
                system("clear");
                cout << "You Win! 🎉 Board Full!\n";
                break;
            }
        }
        else
        {
            snake.pop_front();
        }
        
        render_game(BOARD_SIZE, snake, food, poison, score, level);

        int delay_ms = max(50, baseDelay - (level - 1) * 30 - (int)snake.size() * 5);
        this_thread::sleep_for(chrono::milliseconds(delay_ms));
    }

    cout << "Final Length: " << snake.size() << " | Final Score: " << score << "\n";
    save_score(score);

    cout << "\n=== Top 10 Scores ===\n";
    vector<int> topScores = load_top_scores();
    for (int i = 0; i < topScores.size(); i++)
        cout << "  " << (i + 1) << ". " << topScores[i] << "\n";

    this_thread::sleep_for(chrono::seconds(3));
    exit(0);
}

#endif // SNAKE_H
