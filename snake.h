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
bool paused = false; // track pause state
vector<int> high_scores; // NEW: top 10 scores

void input_handler(){
    // change terminal settings
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    // turn off canonical mode and echo
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    map<char, char> keymap = {{'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}};

    while (true) {
        char input = getchar();

        if (keymap.find(input) != keymap.end()) {
            if (!paused) direction = keymap[input]; // movement only when not paused
        } else if (input == 'q') {
            exit(0);
        } else if (input == 'p') {
            paused = !paused; // toggle pause/resume
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void render_game(int size, deque<pair<int, int>> &snake, pair<int, int> food, pair<int, int> poison){
    for(size_t i=0;i<size;i++){
        for(size_t j=0;j<size;j++){
            if (i == food.first && j == food.second){
                cout << "🍎";
            }else if (i == poison.first && j == poison.second){
                cout << "💀";
            }else if (find(snake.begin(), snake.end(), make_pair(int(i), int(j))) != snake.end()) {
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

// helper: generate new item not inside snake
pair<int,int> generate_item(const deque<pair<int,int>> &snake, pair<int,int> other, int size=10){
    pair<int,int> pos;
    do {
        pos = make_pair(rand() % size, rand() % size);
    } while(find(snake.begin(), snake.end(), pos) != snake.end() || pos == other); 
    return pos;
}

// NEW: update and print top 10 scores
void update_high_scores(int score) {
    high_scores.push_back(score);
    sort(high_scores.begin(), high_scores.end(), greater<int>()); // sort descending
    if (high_scores.size() > 10) high_scores.pop_back(); // keep only top 10

    cout << "\n=== High Scores ===" << endl;
    for (size_t i = 0; i < high_scores.size(); i++) {
        cout << i+1 << ". " << high_scores[i] << endl;
    }
}

void game_play(){
    system("clear");
    deque<pair<int, int>> snake;
    snake.push_back(make_pair(0,0));

    pair<int, int> food = generate_item(snake, {-1,-1});   // ensures no overlap
    pair<int, int> poison = generate_item(snake, food);    // separate from snake & food

    int score = 0;      // track score
    int moves = 0;      // track moves for poison relocation

    int speed = 500;      // initial speed in ms
    const int min_speed = 100;  // max difficulty
    const int decrement = 20;   // speed decrease per growth

    for(pair<int, int> head=make_pair(0,1);; head = get_next_head(head, direction)){
        cout << "\033[H"; // send cursor to top-left

        if (paused) {
            render_game(10, snake, food, poison);
            cout << "length of snake: " << snake.size() << endl;
            cout << "score: " << score << endl;
            cout << "[PAUSED] Press 'p' to resume" << endl;
            sleep_for(chrono::milliseconds(200));
            continue;
        }

        if (find(snake.begin(), snake.end(), head) != snake.end()) {
            system("clear");
            cout << "Game Over! Final Score: " << score << endl;
            update_high_scores(score);
            exit(0);
        } else if (head == food) {
            snake.push_back(head);

            // spawn new food
            food = generate_item(snake, poison);

            // increase difficulty
            speed -= decrement;
            if(speed < min_speed) speed = min_speed;

            // update score
            score += 10;
        } else if (head == poison) {
            system("clear");
            cout << "You ate poison! Game Over. Final Score: " << score << endl;
            update_high_scores(score);
            exit(0);
        } else {
            snake.push_back(head);
            snake.pop_front();
        }

        // relocate poison every 5 moves
        moves++;
        if (moves % 5 == 0) {
            poison = generate_item(snake, food);
        }

        render_game(10, snake, food, poison);
        cout << "length of snake: " << snake.size() << endl;
        cout << "score: " << score << endl;

        sleep_for(chrono::milliseconds(speed));
    }
}
