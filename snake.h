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

char direction='r';

// handle keyboard input in a separate thread
void input_handler(){
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    map<char, char> keymap = {
        {'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}, {'q', 'q'}
    };

    while (true) {
        char input = getchar();
        if (keymap.find(input) != keymap.end()) {
            direction = keymap[input];
        } else if (input == 'q') {
            exit(0);
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

// draw the board
void render_game(int size, deque<pair<int, int>> &snake, pair<int, int> food){
    for(size_t i=0;i<size;i++){
        for(size_t j=0;j<size;j++){
            if (i == food.first && j == food.second){
                cout << "🍎";
            } else if (find(snake.begin(), snake.end(),
                            make_pair(int(i), int(j))) != snake.end()) {
                cout << "🐍";
            } else {
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
    } else if (direction=='l') {
        next = make_pair(current.first, current.second==0?9:current.second-1);
    } else if(direction =='d'){
        next = make_pair((current.first+1)%10,current.second);
    } else if (direction=='u'){
        next = make_pair(current.first==0?9:current.first-1, current.second);
    }
    return next;
}

// main game loop with increasing difficulty
void game_play(){
    system("clear");
    deque<pair<int, int>> snake;
    snake.push_back(make_pair(0,0));

    pair<int, int> food = make_pair(rand() % 10, rand() % 10);

    // initial head
    pair<int, int> head=make_pair(0,1);

    while (true) {
        // send the cursor to the top
        cout << "\033[H";

        // check self collision
        if (find(snake.begin(), snake.end(), head) != snake.end()) {
            system("clear");
            cout << "Game Over" << endl;
            exit(0);
        } else if (head.first == food.first && head.second == food.second) {
            // grow snake
            food = make_pair(rand() % 10, rand() % 10);
            snake.push_back(head);            
        } else {
            // move snake
            snake.push_back(head);
            snake.pop_front();
        }

        render_game(10, snake, food);
        cout << "length of snake: " << snake.size() << endl;

        // === New Feature: dynamic speed ===
        // Base delay 500ms; reduce as snake gets longer
        // never go below 100ms
        int delay_ms = max(100, 500 - (int)snake.size()*20);
        sleep_for(std::chrono::milliseconds(delay_ms));

        // compute next head for next loop
        head = get_next_head(head, direction);
    }
}