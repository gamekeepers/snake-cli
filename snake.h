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
char direction='r';


void input_handler(){
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    map<char, char> keymap = {{'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}, {'q', 'q'}};
    while (true) {
        char input = getchar();
        if (keymap.find(input) != keymap.end()) {
            direction = keymap[input];
        }else if (input == 'q'){
            exit(0);
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}


void render_game(int size, deque<pair<int, int>> &snake, pair<int, int> food){
    for(size_t i=0;i<size;i++){
        for(size_t j=0;j<size;j++){
            if (i == food.first && j == food.second){
                cout << "🍎";
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

pair<int,int> generate_food(int size, const deque<pair<int,int>> &snake) {
    pair<int,int> food;
    do {
        food = make_pair(rand() % size, rand() % size);
    } while (find(snake.begin(), snake.end(), food) != snake.end());
    return food;
}
void game_play() {
    system("clear");
    deque<pair<int, int>> snake;
    snake.push_back(make_pair(0, 0));

    pair<int, int> food = generate_food(10, snake);


    int food_eaten = 0;
    int speed = 500;
    int score = 0;

    for (pair<int, int> head = make_pair(0, 1);; head = get_next_head(head, direction)) {
        cout << "\033[H";

        if (find(snake.begin(), snake.end(), head) != snake.end()) {
            system("clear");
            cout << "Game Over" << endl;
            exit(0);
        } else if (head.first == food.first && head.second == food.second) {
            food = generate_food(10, snake);

            snake.push_back(head);

            food_eaten++;
            score+=10;

            if (food_eaten % 10 == 0 && speed > 100) {
                speed += 50;
            }
        } else {
            snake.push_back(head);
            snake.pop_front();
        }

        render_game(10, snake, food);
        cout << "length of snake: " << snake.size() << endl;
        cout << "Score: " << score << endl;

        cout << "food eaten: " << food_eaten << " | current speed: " << speed << "ms" << endl;

        sleep_for(std::chrono::milliseconds(speed));
    }
}