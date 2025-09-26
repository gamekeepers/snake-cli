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

constexpr int BOARD_SIZE = 10;

char direction = 'r';

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

void render_game(int size, deque<pair<int, int>> &snake, pair<int, int> food, pair<int, int> poison, int score)
{
    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            if (i == food.first && j == food.second)
            {
                cout << "🍎";
            }
            else if (i == poison.first && j == poison.second)
            {
                cout << "☠️";
            }
            else if (find(snake.begin(), snake.end(), make_pair(i, j)) != snake.end())
            {
                cout << "🐍";
            }
            else
            {
                cout << "⬜";
            }
        }
        cout << '\n';
    }
    cout << "Length: " << snake.size() << "  Score: " << score << "\n";
}

pair<int, int> get_next_head(pair<int, int> current, char direction)
{
    pair<int, int> next;
    if (direction == 'r')
    {
        next = make_pair(current.first, (current.second + 1) % BOARD_SIZE);
    }
    else if (direction == 'l')
    {
        next = make_pair(current.first, current.second == 0 ? BOARD_SIZE - 1 : current.second - 1);
    }
    else if (direction == 'd')
    {
        next = make_pair((current.first + 1) % BOARD_SIZE, current.second);
    }
    else
    { 
        next = make_pair(current.first == 0 ? BOARD_SIZE - 1 : current.first - 1, current.second);
    }
    return next;
}

pair<int, int> spawn_food(const deque<pair<int, int>> &snake)
{
    vector<pair<int, int>> freeCells;
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        for (int j = 0; j < BOARD_SIZE; ++j)
        {
            pair<int, int> pos = make_pair(i, j);
            if (find(snake.begin(), snake.end(), pos) == snake.end())
            {
                freeCells.push_back(pos);
            }
        }
    }

    if (freeCells.empty())
        return make_pair(-1, -1); 

    int idx = rand() % freeCells.size();
    return freeCells[idx];
}

void save_score(int score)
{
    ofstream file("scores.txt", ios::app);
    if (file.is_open())
    {
        file << score << "\n";
        file.close();
    }
}

vector<int> load_top_scores()
{
    ifstream file("scores.txt");
    vector<int> scores;
    int s;

    while (file >> s)
    {
        scores.push_back(s);
    }
    file.close();

    sort(scores.begin(), scores.end(), greater<int>());

    if (scores.size() > 10)
    {
        scores.resize(10);
    }

    return scores;
}

void game_play()
{
    system("clear");
    deque<pair<int, int>> snake;
    snake.push_back(make_pair(0, 0)); 

    pair<int, int> food = spawn_food(snake);
    pair<int, int> poison = spawn_food(snake); 

    int score = 0;
    int level = 1;     
    int baseDelay = 500; 

    auto sleep_duration = 500ms;  
    int food_eaten = 0;          

    while (true)
    {
        pair<int, int> currentHead = snake.back();
        pair<int, int> nextHead = get_next_head(currentHead, direction);

        cout << "\033[H";

        bool willGrow = (nextHead == food);
        bool collision = false;

        if (direction == 'P')
        {
            sleep_for(100ms);
            continue;
        }

        if (willGrow)
        {
            if (find(snake.begin(), snake.end(), nextHead) != snake.end())
                collision = true;
        }
        else
        {
            auto itStart = snake.begin();
            if (!snake.empty())
                ++itStart; 
            if (find(itStart, snake.end(), nextHead) != snake.end())
                collision = true;
        }

        if (collision)
        {
            system("clear");
            cout << "Game Over\n";
            cout << "Final Length: " << snake.size() << "  Final Score: " << score << "\n";

            save_score(score);

            cout << "\n=== Top 10 Scores ===\n";
            vector<int> topScores = load_top_scores();
            for (int i = 0; i < topScores.size(); i++)
            {
                cout << (i + 1) << ". " << topScores[i] << "\n";
            }

            exit(0);
        }

        if (nextHead == poison)
        {
            system("clear");
            cout << "Snake ate poison ☠️ Game Over!\n";
            cout << "Final Length: " << snake.size() << "  Final Score: " << score << "\n";

            save_score(score);

            cout << "\n=== Top 10 Scores ===\n";
            vector<int> topScores = load_top_scores();
            for (int i = 0; i < topScores.size(); i++)
            {
                cout << (i + 1) << ". " << topScores[i] << "\n";
            }

            exit(0);
        }

        snake.push_back(nextHead);

        if (willGrow)
        {
            score += 1;
            food_eaten++; 
            food = spawn_food(snake);

            if (score % 3 == 0)
            {
                poison = spawn_food(snake);
            }

            if (food_eaten % 10 == 0 && sleep_duration > 100ms)
            {
                sleep_duration -= 50ms;
            }

            if (food.first == -1)
            {
                system("clear");
                cout << "You Win! Final Length: " << snake.size() << "  Final Score: " << score << "\n";

                save_score(score);

                cout << "\n=== Top 10 Scores ===\n";
                vector<int> topScores = load_top_scores();
                for (int i = 0; i < topScores.size(); i++)
                {
                    cout << (i + 1) << ". " << topScores[i] << "\n";
                }

                exit(0);
            }
        }
        else
        {
            snake.pop_front();
        }

        level = (score / 5) + 1;

        render_game(BOARD_SIZE, snake, food, poison, score);

        cout << "Level: " << level << "\n";
        cout << "Speed: " << sleep_duration.count() << " ms per frame\n"; 

        int delay_ms = max(50, baseDelay - (level - 1) * 50 - (int)snake.size() * 5);
        sleep_for(min(sleep_duration, chrono::milliseconds(delay_ms)));
    }
}
