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
#include <atomic>

using namespace std;
using std::chrono::system_clock;
using namespace std::this_thread;

char direction = 'r';
bool paused = false;
vector<int> top_scores;
atomic<bool> game_running{true}; // NEW: to stop input_handler safely

// Save original terminal state
struct termios orig_termios;

void reset_terminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

void input_handler() {
    // change terminal settings
    struct termios newt;
    tcgetattr(STDIN_FILENO, &orig_termios);
    newt = orig_termios;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    map<char, char> keymap = {{'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}};

    while (game_running.load()) {
        char input = getchar();
        if (input == 'p') {
            paused = !paused;
        } else if (input == 'q') {
            game_running = false;
            break;
        } else if (keymap.find(input) != keymap.end()) {
            direction = keymap[input];
        }
    }

    reset_terminal(); // restore terminal when exiting
}

pair<int, int> generate_food(int size, const deque<pair<int, int>>& snake, pair<int,int> other_food = make_pair(-1,-1)) {
    while (true) {
        pair<int, int> food = make_pair(rand() % size, rand() % size);
        if (find(snake.begin(), snake.end(), food) == snake.end() && food != other_food) {
            return food;
        }
    }
}

void render_game(int size, deque<pair<int, int>> &snake, pair<int, int> food, pair<int,int> poison) {
    for(size_t i=0;i<size;i++){
        for(size_t j=0;j<size;j++){
            if (i == food.first && j == food.second){
                cout << "🍎";
            } else if (i == poison.first && j == poison.second){
                cout << "💀";
            } else if (find(snake.begin(), snake.end(), make_pair(int(i), int(j))) != snake.end()) {
                cout << "🐍";
            }else{
                cout << "⬜";
            }
        }
        cout << endl;
    }
}

pair<int,int> get_next_head(pair<int,int> current, char direction){
    pair<int, int> next; 
    if(direction =='r'){
        next = make_pair(current.first,(current.second+1) % 10);
    }else if (direction=='l'){
        next = make_pair(current.first, current.second==0?9:current.second-1);
    }else if(direction =='d'){
        next = make_pair((current.first+1)%10,current.second);
    }else if (direction=='u'){
        next = make_pair(current.first==0?9:current.first-1, current.second);
    }
    return next;
}

int calculate_speed(int food_eaten, int base_speed = 500, int min_speed = 100, int step = 50) {
    int reduced = base_speed - (food_eaten) * step;
    return max(min_speed, reduced);
}

void update_leaderboard(int score) {
    top_scores.push_back(score);
    sort(top_scores.begin(), top_scores.end(), greater<int>());
    if (top_scores.size() > 10) top_scores.resize(10);

    cout << "\n===== Leaderboard =====" << endl;
    for (size_t i=0; i<top_scores.size(); i++) {
        cout << i+1 << ". " << top_scores[i] << endl;
    }
    cout << "=======================" << endl;
}

bool game_play(){
    int score = 0;
    int speed = 500;

    system("clear");
    deque<pair<int, int>> snake;
    snake.push_back(make_pair(0,0));

    pair<int, int> food = generate_food(10, snake);
    pair<int, int> poison = generate_food(10, snake, food);
    int moves = 0;

    for(pair<int, int> head=make_pair(0,1);; head = get_next_head(head, direction)){
        if (!game_running) return false;
        if (paused) {
            sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        cout << "\033[H";
        if (find(snake.begin(), snake.end(), head) != snake.end()) {
            system("clear");
            cout << "Game Over" << endl;
            update_leaderboard(score);
            break;
        }else if (head.first == poison.first && head.second == poison.second) {
            system("clear");
            cout << "Game Over" << endl;
            update_leaderboard(score);
            break;
        }else if (head.first == food.first && head.second == food.second) {
            food = generate_food(10, snake, poison);
            snake.push_back(head);  
            score += 10;
            speed = calculate_speed(score/10);         
        }else{
            snake.push_back(head);
            snake.pop_front();
        }

        moves++;
        if (moves % 5 == 0) {
            poison = generate_food(10, snake, food);
        }

        render_game(10, snake, food, poison);
        cout << "length of snake: " << snake.size() << endl;
        cout << "score: " << score << endl;

        sleep_for(std::chrono::milliseconds(speed));
    }

    reset_terminal();
    return true;
}
