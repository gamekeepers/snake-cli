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

typedef pair<int, int> Cell;

class Snake{
    private:
        deque<Cell> body;
        int size=0;
        char direction = 'r';
    public:
    Snake(){
        this->body.push_back(make_pair(0,0));
    }

    int getSize(){
        return this->body.size();
    }
    void grow(){
        Cell new_head = this->get_next_position();
        this->body.push_back(new_head);
    }

    void move(){
        Cell new_head = this->get_next_position();
        this->body.push_back(new_head);
        this->body.pop_front();
    }

    void print_snake(){
        for(Cell cell: this->body){
            cout << cell.first << "," << cell.second << ">>";
        }
        cout << endl;
    }
    bool contains(Cell position){
        return find(this->body.begin(), this->body.end(), position) != this->body.end();
    }

    void set_direction(char direction){
        
        this->direction = direction;
    }

    Cell get_head(){
        return this->body.back();
    }

    Cell get_next_position(){
        Cell current = this->get_head();
        Cell next; 
        if(this->direction =='r'){
            next = make_pair(current.first,(current.second+1) % 10);
        }else if (this->direction=='l')
        {
            next = make_pair(current.first, current.second==0?9:current.second-1);
        }else if(this->direction =='d'){
                next = make_pair((current.first+1)%10,current.second);
            }else if (this->direction=='u'){
                next = make_pair(current.first==0?9:current.first-1, current.second);
            }
        return next;
        
    }
    
};


void input_handler(Snake& snake){
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
            //direction = keymap[input];
            snake.set_direction(keymap[input]);
            cout << "changed direction to " << keymap[input] << endl;
        }else if (input == 'q'){
            exit(0);
        }
        // You could add an exit condition here, e.g., if (input == 'q') break;
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}



Cell get_next_head(Cell current, char direction){
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

void move_snake(deque<Cell> &snake, Cell new_head){
    snake.push_back(new_head);
    snake.pop_front();
}

Cell generate_random_cell(){
    Cell new_pos = make_pair(rand() % 10, rand() % 10);
    return new_pos;
}


    


void render_game(int size, Snake &snake, Cell food){
    for(size_t i=0;i<size;i++){
        for(size_t j=0;j<size;j++){
            if (snake.contains(make_pair(int(i), int(j)))) {
                cout << "🐍";
            }else if (i == food.first && j == food.second){
                cout << "🍎";
            }else{
                cout << "⬜";
            }
    }
    cout << endl;
}
}



void game_play(Snake& snake){
    system("clear");

    Cell food = generate_random_cell();
    while(true){
        reset_cursor();
        // check self collision
        if (snake.contains(snake.get_next_position())) {
            system("clear");
            cout << "Game Over" << endl;
            exit(0);
        }else if (snake.contains(food)) {
            food = generate_random_cell();
            snake.grow();
            
        }else{
            snake.move();
            // cout << "adding new head" << head.first<< ","<< head.second<< endl;
        }

        render_game(10, snake, food);
        cout << "length of snake: " << snake.getSize() << endl;

        sleep_for(500ms);
    }
}
