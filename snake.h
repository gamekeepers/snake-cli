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

typedef pair<int, int> Cell;

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


void render_game(int size, deque<Cell> &snake, Cell food){
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
    Cell next; 
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

void reset_cursor(){
    cout << "\033[H";
}

bool is_part_of_snake(deque<Cell> snake, Cell position){
    return find(snake.begin(), snake.end(), position) != snake.end();
}

void grow_snake(deque<Cell> &snake, Cell new_head){
    snake.push_back(new_head);
}

void move_snake(deque<Cell> &snake, Cell new_head){
    snake.push_back(new_head);
    snake.pop_front();
}

Cell generate_random_cell(){
    Cell new_pos = make_pair(rand() % 10, rand() % 10);
    return new_pos;
}

void game_play(){
    system("clear");
    deque<Cell> snake;
    snake.push_back(make_pair(0,0));

    Cell food = generate_random_cell();
    for(Cell head=make_pair(0,1);; head = get_next_head(head, direction)){
        reset_cursor();
        // check self collision
        if (is_part_of_snake(snake, head)) {
            system("clear");
            cout << "Game Over" << endl;
            exit(0);
        }else if (is_part_of_snake(snake, food)) {
            grow_snake(snake, head);
            food = generate_random_cell();
        }else{
            move_snake(snake, head);
        }

        render_game(10, snake, food);
        cout << "length of snake: " << snake.size() << endl;
        
        sleep_for(500ms);
    }
}
