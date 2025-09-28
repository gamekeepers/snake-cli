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

constexpr int BOARD_SIZE = 10;
extern char direction;

// ------------------ Snake Class ------------------
class Snake
{
private:
    deque<pair<int, int>> body;

public:
    Snake()
    {
        body.push_back({0, 0}); // start at (0,0)
    }

    void grow(pair<int, int> newHead)
    {
        body.push_back(newHead);
    }

    void move(pair<int, int> newHead)
    {
        body.push_back(newHead);
        body.pop_front();
    }

    pair<int, int> getHead() const
    {
        return body.back();
    }

    int getSize() const
    {
        return body.size();
    }

    const deque<pair<int, int>> &getBody() const
    {
        return body;
    }
};

// ------------------ Input Handling ------------------
inline void input_handler()
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
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            exit(0);
        }
        else if (input == 'p')
        {
            if (direction != 'P')
                direction = 'P';
            else
                direction = 'r';
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

// ------------------ Rendering ------------------
inline void render_game(int size, const Snake &snake, pair<int, int> food,
                        pair<int, int> poison, int score)
{
    const auto &body = snake.getBody();
    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            if (i == food.first && j == food.second)
                cout << "🍎";
            else if (i == poison.first && j == poison.second)
                cout << "☠️";
            else if (find(body.begin(), body.end(), make_pair(i, j)) != body.end())
                cout << "🐍";
            else
                cout << "⬜";
        }
        cout << '\n';
    }
    cout << "Length: " << snake.getSize() << "  Score: " << score << "\n";
}

// ------------------ Game Helpers ------------------
inline pair<int, int> get_next_head(pair<int, int> current, char direction)
{
    if (direction == 'r')
        return {current.first, (current.second + 1) % BOARD_SIZE};
    else if (direction == 'l')
        return {current.first, current.second == 0 ? BOARD_SIZE - 1 : current.second - 1};
    else if (direction == 'd')
        return {(current.first + 1) % BOARD_SIZE, current.second};
    else if (direction == 'u')
        return {current.first == 0 ? BOARD_SIZE - 1 : current.first - 1, current.second};
    else if (direction == 'P') // <-- pause means no movement
        return current;
    else
        return current; // safe fallback
}

inline pair<int, int> spawn_food(const deque<pair<int, int>> &body)
{
    vector<pair<int, int>> freeCells;
    for (int i = 0; i < BOARD_SIZE; ++i)
        for (int j = 0; j < BOARD_SIZE; ++j)
            if (find(body.begin(), body.end(), make_pair(i, j)) == body.end())
                freeCells.push_back({i, j});

    if (freeCells.empty())
        return {-1, -1};

    return freeCells[rand() % freeCells.size()];
}

// ------------------ Score Management ------------------
inline void save_score(int score)
{
    ofstream file("scores.txt", ios::app);
    if (file.is_open())
    {
        file << score << "\n";
        file.close();
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

// ------------------ Game Loop ------------------
inline void game_play()
{
    system("clear");
    Snake snake;
    pair<int, int> food = spawn_food(snake.getBody());
    pair<int, int> poison = spawn_food(snake.getBody());

    int score = 0, level = 1, baseDelay = 500;

    while (true)
    {
        auto currentHead = snake.getHead();
        auto nextHead = get_next_head(currentHead, direction);

        cout << "\033[H";

        if (direction == 'P')
        {
            this_thread::sleep_for(chrono::milliseconds(100));
            continue;
        }

        bool willGrow = (nextHead == food);
        bool collision = false;
        const auto &body = snake.getBody();

        if (willGrow)
        {
            if (find(body.begin(), body.end(), nextHead) != body.end())
                collision = true;
        }
        else
        {
            auto itStart = body.begin();
            if (!body.empty())
                ++itStart;
            if (find(itStart, body.end(), nextHead) != body.end())
                collision = true;
        }

        if (collision)
        {
            system("clear");
            cout << "Game Over\n";
            cout << "Final Length: " << snake.getSize() << "  Final Score: " << score << "\n";
            save_score(score);

            cout << "\n=== Top 10 Scores ===\n";
            auto topScores = load_top_scores();
            for (int i = 0; i < topScores.size(); i++)
                cout << (i + 1) << ". " << topScores[i] << "\n";
            exit(0);
        }

        if (nextHead == poison)
        {
            system("clear");
            cout << "Snake ate poison ☠️ Game Over!\n";
            cout << "Final Length: " << snake.getSize() << "  Final Score: " << score << "\n";
            save_score(score);

            cout << "\n=== Top 10 Scores ===\n";
            auto topScores = load_top_scores();
            for (int i = 0; i < topScores.size(); i++)
                cout << (i + 1) << ". " << topScores[i] << "\n";
            exit(0);
        }

        if (willGrow)
        {
            snake.grow(nextHead);
            score++;
            food = spawn_food(snake.getBody());

            if (score % 3 == 0)
                poison = spawn_food(snake.getBody());

            if (food.first == -1)
            {
                system("clear");
                cout << "You Win! Final Length: " << snake.getSize() << "  Final Score: " << score << "\n";
                save_score(score);

                cout << "\n=== Top 10 Scores ===\n";
                auto topScores = load_top_scores();
                for (int i = 0; i < topScores.size(); i++)
                    cout << (i + 1) << ". " << topScores[i] << "\n";
                exit(0);
            }
        }
        else
        {
            snake.move(nextHead);
        }

        level = (score / 5) + 1;
        render_game(BOARD_SIZE, snake, food, poison, score);
        cout << "Level: " << level << "\n";

        int delay_ms = max(50, baseDelay - (level - 1) * 50 - snake.getSize() * 5);
        this_thread::sleep_for(chrono::milliseconds(delay_ms));
    }
}

#endif