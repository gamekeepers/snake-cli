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
#include <memory>

using namespace std;
using std::chrono::system_clock;
using namespace std::this_thread;

typedef pair<int, int> Cell;

class InputManager {
public:
    virtual ~InputManager() {}
    virtual char getInput() = 0;
};

class KeyboardInputManager : public InputManager {
public:
    KeyboardInputManager() {
        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    }

    ~KeyboardInputManager() {
        struct termios oldt;
        tcgetattr(STDIN_FILENO, &oldt);
        oldt.c_lflag |= (ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }

    char getInput() override {
        return getchar();
    }
};

class MockInputManager : public InputManager {
private:
    char next_input;
public:
    char getInput() override {
        return next_input;
    }
    void setNextInput(char input) {
        next_input = input;
    }
};

class Snake{
    private:
        deque<Cell> body;
        int size=0;
        char direction = 'x';
    public:
    // default constructor
    Snake(){
        this->body.push_back(make_pair(0,0));
    }

    // constructor with default position
    Snake(Cell position){
        this->body.push_back(position);
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
        if (direction != 'r' && direction != 'l' && direction != 'u' && direction != 'd'){
            return;
        }else if(direction == this->direction){
            
        }else if(direction == 'r' && this->direction == 'l'){
            return;
        }else if (direction == 'l' && this->direction == 'r'){
            return;
        }else if (direction == 'u' && this->direction == 'd'){
            return;
        }else if (direction == 'd' && this->direction == 'u'){
            return;
        }

        this->direction = direction;
    }

    char get_direction(){
        return this->direction;
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

class Game{
    private:
    int size=10;
    // speed
    std::chrono::milliseconds speed_timer = 500ms;
    shared_ptr<InputManager> input_manager;

    public:
    Snake snake;
    Cell food;
    Game(shared_ptr<InputManager> input_manager) : input_manager(input_manager) {
        this->snake = Snake();
    }

    std::chrono::milliseconds getSpeed(){
        return this->speed_timer;
    }
    int getSize(){
        return this->size;
    }

    Cell generate_random_cell(){
        Cell new_pos = make_pair(rand() % this->getSize(), rand() % this->getSize());
        while(this->snake.contains(new_pos)){
            new_pos = make_pair(rand() % this->getSize(), rand() % this->getSize());
        }
        return new_pos;
    }

    bool checkCollission(Cell position){
        return this->snake.contains(position);
    }

    void set_direction(char direction){
        this->snake.set_direction(direction);
    }

    void render(Cell food){
        for(size_t i=0;i<this->getSize();i++){
            for(size_t j=0;j<this->getSize();j++){
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
    sleep_for(this->getSpeed());
    }
    
    shared_ptr<InputManager> getInputManager(){
        return this->input_manager;
    }
};


void handle_single_input(Game& game);
void input_handler(Game& game);
void reset_cursor();
void move_snake(deque<Cell> &snake, Cell new_head);
void game_play(Game& game);

