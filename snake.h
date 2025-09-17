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
bool paused = false;


void input_handler(){
    // change terminal settings
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    // turn off canonical mode and echo
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    map<char, char> keymap = {{'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}, {'q', 'q'}};
    while (true) {
        char input = getchar();
        if (keymap.find(input) != keymap.end()) {
            // This now correctly modifies the single, shared 'direction' variable
            direction = keymap[input];
        }else if (input == 'p') {
            paused = !paused;
        }else if (input == 'q'){
            exit(0);
        }
        // You could add an exit condition here, e.g., if (input == 'q') break;
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}


void render_game(int size, deque<pair<int, int>> &snake, pair<int, int> food, pair<int, int> poison){
    for(size_t i=0;i<size;i++){
        for(size_t j=0;j<size;j++){
            if (i == food.first && j == food.second){
                cout << "🍎";
            }else if (i == poison.first && j == poison.second){
                cout << "☠️";
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
    }else if (direction=='l')
    {
        next = make_pair(current.first, current.second==0?9:current.second-1);
    }else if(direction =='d'){
            next = make_pair((current.first+1)%10,current.second);
        }else if (direction=='u'){
            next = make_pair(current.first==0?9:current.first-1, current.second);
        }
    return next;
    
}



void game_play(){
    system("clear");
    deque<pair<int, int>> snake;
    snake.push_back(make_pair(0,0));

    auto generate_food = [&](deque<pair<int, int>> &snake, pair<int,int> otherFood) {
        pair<int, int> newFood;
        do {
            newFood = make_pair(rand() % 10, rand() % 10);
        } while (find(snake.begin(), snake.end(), newFood) != snake.end() || newFood == otherFood);
        return newFood;
    };

    pair<int, int> food = generate_food(snake, {-1, -1});
    pair<int, int> poison = generate_food(snake, food);

    int score = 0;
    int foodEaten = 0;
    int speed = 500;
    
    for(pair<int, int> head=make_pair(0,1);; head = get_next_head(head, direction)){
        // send the cursor to the top
        cout << "\033[H";

        if(paused){
            cout << "Game Paused - Press 'p' to Resume" << endl;
            sleep_for(200ms);
            continue;
        }
        // check self collision
        if (find(snake.begin(), snake.end(), head) != snake.end()) {
            system("clear");
            cout << "Game Over! You ate yourself 🐍" << endl;
            cout << "Final Score: " << score << endl;
            exit(0);
        }else if (head.first == food.first && head.second == food.second) {
            // grow snake
            food = generate_food(snake, poison);
            poison = generate_food(snake, food);
            snake.push_back(head);
            foodEaten++;
            score += 10;

            if(foodEaten%10 == 0 && speed > 100){ speed -= 50; }
        }else if (head.first == poison.first && head.second == poison.second) {
            system("clear");
            cout << "Game Over! You ate poison ☠️" << endl;
            cout << "Final Score: " << score << endl;
            exit(0);
        }else{
            // move snake
            snake.push_back(head);
            snake.pop_front();
        }
        render_game(10, snake, food, poison);
        cout << "Score: " << score << endl;
        cout << "length of snake: " << snake.size() << endl;
        cout << "Speed: " << speed << "ms" << endl;
    
        sleep_for(std::chrono::milliseconds(speed));
    }
}
