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
#include <stdexcept>
using namespace std;
using std::chrono::system_clock;
using namespace std::this_thread;

class SnakeGame {
private:
    static const int BOARD_SIZE = 10;
    static const int INITIAL_SPEED = 500;
    static const int MIN_SPEED = 100;
    static const int SPEED_DECREMENT = 50;
    static const int FOOD_SCORE = 10;
    static const int HIGH_SCORES_LIMIT = 10;
    
    char direction;
    vector<int> high_scores;
    deque<pair<int, int>> snake;
    pair<int, int> food;
    pair<int, int> poison;
    int food_count;
    int speed;
    int score;

public:
    SnakeGame() : direction('r'), food_count(0), speed(INITIAL_SPEED), score(0) {
        initialize_game();
    }
    
    void initialize_game() {
        try {
            // Validate board size
            if (BOARD_SIZE <= 0) {
                throw invalid_argument("Board size must be positive");
            }
            
            // Initialize snake
            snake.push_back(make_pair(0, 0));
            
            // Initialize food and poison
            spawn_food();
            spawn_poison();
        } catch (const exception& e) {
            cerr << "Error initializing game: " << e.what() << endl;
            exit(1);
        }
    }
    
    void input_handler(){
        try {
            // change terminal settings
            struct termios oldt, newt;
            if (tcgetattr(STDIN_FILENO, &oldt) == -1) {
                throw runtime_error("Failed to get terminal attributes");
            }
            newt = oldt;
            // turn off canonical mode and echo
            newt.c_lflag &= ~(ICANON | ECHO);
            if (tcsetattr(STDIN_FILENO, TCSANOW, &newt) == -1) {
                throw runtime_error("Failed to set terminal attributes");
            }
            
            map<char, char> keymap = {{'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}, {'q', 'q'}};
            while (true) {
                char input = getchar();
                if (keymap.find(input) != keymap.end()) {
                    direction = keymap[input];
                } else if (input == 'q'){
                    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
                    exit(0);
                }
            }
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        } catch (const exception& e) {
            cerr << "Error in input handler: " << e.what() << endl;
            exit(1);
        }
    }
    
    void render_board(){
        try {
            for(size_t i=0;i<BOARD_SIZE;i++){
                for(size_t j=0;j<BOARD_SIZE;j++){
                    render_cell(i, j);
                }
                cout << endl;
            }
        } catch (const exception& e) {
            cerr << "Error rendering board: " << e.what() << endl;
        }
    }
    
    void render_cell(int row, int col){
        try {
            // Validate coordinates
            if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) {
                throw out_of_range("Coordinates out of board range");
            }
            
            if (row == food.first && col == food.second){
                cout << "🍎";
            } else if (row == poison.first && col == poison.second){
                cout << "💀";
            } else if (is_snake_segment(row, col)) {
                cout << "🐍";
            } else {
                cout << "⬜";
            }
        } catch (const exception& e) {
            cerr << "Error rendering cell: " << e.what() << endl;
        }
    }
    
    bool is_snake_segment(int row, int col) {
        // Validate coordinates
        if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) {
            return false;
        }
        
        return find(snake.begin(), snake.end(), make_pair(row, col)) != snake.end();
    }
    
    pair<int,int> get_next_head(pair<int,int> current, char direction){
        pair<int, int> next; 
        if(direction =='r'){
            next = make_pair(current.first,(current.second+1) % BOARD_SIZE);
        } else if (direction=='l') {
            next = make_pair(current.first, current.second==0?BOARD_SIZE-1:current.second-1);
        } else if(direction =='d'){
            next = make_pair((current.first+1)%BOARD_SIZE,current.second);
        } else if (direction=='u'){
            next = make_pair(current.first==0?BOARD_SIZE-1:current.first-1, current.second);
        }
        return next;
    }
    
    void spawn_food() {
        try {
            int attempts = 0;
            const int max_attempts = BOARD_SIZE * BOARD_SIZE;
            
            do {
                food = make_pair(rand() % BOARD_SIZE, rand() % BOARD_SIZE);
                attempts++;
                
                if (attempts > max_attempts) {
                    throw runtime_error("Unable to spawn food after maximum attempts");
                }
            } while (is_snake_segment(food.first, food.second));
        } catch (const exception& e) {
            cerr << "Error spawning food: " << e.what() << endl;
            // Fallback to a simple position
            food = make_pair(BOARD_SIZE/2, BOARD_SIZE/2);
        }
    }
    
    void spawn_poison() {
        try {
            poison = make_pair(rand() % BOARD_SIZE, rand() % BOARD_SIZE);
        } catch (const exception& e) {
            cerr << "Error spawning poison: " << e.what() << endl;
            // Fallback to a simple position
            poison = make_pair(BOARD_SIZE/4, BOARD_SIZE/4);
        }
    }
    
    void update_high_scores() {
        try {
            high_scores.push_back(score);
            sort(high_scores.begin(), high_scores.end(), greater<int>());
            if (high_scores.size() > HIGH_SCORES_LIMIT) {
                high_scores.resize(HIGH_SCORES_LIMIT);
            }
        } catch (const exception& e) {
            cerr << "Error updating high scores: " << e.what() << endl;
        }
    }
    
    void display_high_scores() {
        try {
            cout << "\n=== Top 10 High Scores ===" << endl;
            for (size_t i = 0; i < high_scores.size(); i++) {
                cout << i + 1 << ". " << high_scores[i] << endl;
            }
            cout << "===========================" << endl;
        } catch (const exception& e) {
            cerr << "Error displaying high scores: " << e.what() << endl;
        }
    }
    
    void game_over(string reason) {
        try {
            system("clear");
            cout << "Game Over - " << reason << endl;
            cout << "Final Score: " << score << endl;
            update_high_scores();
            display_high_scores();
            exit(0);
        } catch (const exception& e) {
            cerr << "Error in game over: " << e.what() << endl;
            exit(1);
        }
    }
    
    bool check_self_collision(pair<int, int> head) {
        try {
            // Validate head coordinates
            if (head.first < 0 || head.first >= BOARD_SIZE || 
                head.second < 0 || head.second >= BOARD_SIZE) {
                return true; // Treat out-of-bounds as collision
            }
            
            return find(snake.begin(), snake.end(), head) != snake.end();
        } catch (const exception& e) {
            cerr << "Error checking self collision: " << e.what() << endl;
            return true; // Treat errors as collision for safety
        }
    }
    
    bool check_poison_collision(pair<int, int> head) {
        try {
            // Validate head coordinates
            if (head.first < 0 || head.first >= BOARD_SIZE || 
                head.second < 0 || head.second >= BOARD_SIZE) {
                return false; // Out-of-bounds is not poison collision
            }
            
            return head.first == poison.first && head.second == poison.second;
        } catch (const exception& e) {
            cerr << "Error checking poison collision: " << e.what() << endl;
            return false; // Treat errors as no collision
        }
    }
    
    bool check_food_collision(pair<int, int> head) {
        try {
            // Validate head coordinates
            if (head.first < 0 || head.first >= BOARD_SIZE || 
                head.second < 0 || head.second >= BOARD_SIZE) {
                return false; // Out-of-bounds is not food collision
            }
            
            return head.first == food.first && head.second == food.second;
        } catch (const exception& e) {
            cerr << "Error checking food collision: " << e.what() << endl;
            return false; // Treat errors as no collision
        }
    }
    
    void update_speed() {
        try {
            if (food_count % 10 == 0 && speed > MIN_SPEED) {
                speed -= SPEED_DECREMENT;
                // Ensure speed doesn't go below minimum
                if (speed < MIN_SPEED) {
                    speed = MIN_SPEED;
                }
            }
        } catch (const exception& e) {
            cerr << "Error updating speed: " << e.what() << endl;
        }
    }
    
    void move_snake_forward(pair<int, int> head) {
        try {
            snake.push_back(head);
            snake.pop_front();
        } catch (const exception& e) {
            cerr << "Error moving snake forward: " << e.what() << endl;
        }
    }
    
    void grow_snake(pair<int, int> head) {
        try {
            snake.push_back(head);
            food_count++;
            score += FOOD_SCORE;
            spawn_food();
            spawn_poison();
            update_speed();
        } catch (const exception& e) {
            cerr << "Error growing snake: " << e.what() << endl;
        }
    }
    
    void handle_collisions(pair<int, int> head) {
        try {
            // check self collision
            if (check_self_collision(head)) {
                game_over("You ran into yourself!");
            }
            
            // check poison collision
            if (check_poison_collision(head)) {
                game_over("You ate poison!");
            }
            
            // check food collision
            if (check_food_collision(head)) {
                grow_snake(head);
            } else {
                move_snake_forward(head);
            }
        } catch (const exception& e) {
            cerr << "Error handling collisions: " << e.what() << endl;
        }
    }
    
    void display_game_stats() {
        try {
            cout << "Length of snake: " << snake.size() 
                 << " | Score: " << score 
                 << " | Speed: " << speed << "ms" << endl;
        } catch (const exception& e) {
            cerr << "Error displaying game stats: " << e.what() << endl;
        }
    }
    
    void play() {
        try {
            system("clear");
            pair<int, int> head = make_pair(0, 1);
            
            while (true) {
                cout << "\033[H";
                head = get_next_head(head, direction);
                
                handle_collisions(head);
                
                // render board
                render_board();
                
                // show stats
                display_game_stats();
                
                // control speed
                sleep_for(chrono::milliseconds(speed));
            }
        } catch (const exception& e) {
            cerr << "Error in game loop: " << e.what() << endl;
            exit(1);
        }
    }
};

// For backward compatibility with existing main.cpp
char direction='r';
vector<int> high_scores;

void input_handler(){
    SnakeGame game;
    game.input_handler();
}

void game_play() {
    SnakeGame game;
    game.play();
}