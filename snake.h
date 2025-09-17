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
        }else if (input == 'q'){
            exit(0);
        }
        // You could add an exit condition here, e.g., if (input == 'q') break;
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}


void render_game(int size, deque<pair<int, int>> &snake, pair<int, int> food, pair<int,int> poison) {
    for (size_t i = 0; i < size; i++) {
        for (size_t j = 0; j < size; j++) {
            if (i == food.first && j == food.second) {
                cout << "🍎";   // good food
            } else if (i == poison.first && j == poison.second) {
                cout << "☠️";   // poison food
            } else if (find(snake.begin(), snake.end(), make_pair(int(i), int(j))) != snake.end()) {
                cout << "🐍";   // snake body
            } else {
                cout << "⬜";   // empty cell
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

pair<int,int> generate_poison(int size, const deque<pair<int,int>> &snake, pair<int,int> food) {
    pair<int,int> poison;
    do {
        poison = make_pair(rand() % size, rand() % size);
    } while (
        find(snake.begin(), snake.end(), poison) != snake.end() || poison == food
    );
    return poison;
}

void game_play() {
    system("clear");
    deque<pair<int, int>> snake;
    snake.push_back(make_pair(0, 0));

    pair<int, int> food = generate_food(10, snake);
    pair<int, int> poison = generate_poison(10, snake, food);


    int food_eaten = 0;   // count how many food items eaten
    int speed = 500;      // start speed in milliseconds
    int score = 0;        // for tracking of the score
    int poison_timer = 0; // track poison movement

    for (pair<int, int> head = make_pair(0, 1);; head = get_next_head(head, direction)) {
        // send the cursor to the top
        cout << "\033[H";

        // check self collision
        if (find(snake.begin(), snake.end(), head) != snake.end()) {
            system("clear");
            cout << "Game Over" << endl;
            exit(0);
        } 
        else if (head == poison) {
              system("clear");
              cout << "Game Over ( Snake ate poison ☠️ )" << endl;
              exit(0);
          }
        else if (head.first == food.first && head.second == food.second) {
            // grow snake
            food = generate_food(10, snake);

            snake.push_back(head);

            food_eaten++;
            score+=10;
          
            if (food_eaten % 10 == 0 && speed > 100) {
                speed += 50;  
            }
        } else {
            // move snake
            snake.push_back(head);
            snake.pop_front();
        }
         poison_timer += speed; // add time passed in this loop
        if (poison_timer >= 5000) {
            poison = generate_poison(10, snake, food);
          poison_timer = 0;
        }
        render_game(10, snake, food, poison);
        cout << "length of snake: " << snake.size() << endl;
        cout << "Score: " << score << endl;

        cout << "food eaten: " << food_eaten << " | current speed: " << speed << "ms" << endl;

        sleep_for(std::chrono::milliseconds(speed));
    }
}

