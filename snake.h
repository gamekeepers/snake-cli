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
#include <fstream> // New header for file handling

using namespace std;
using std::chrono::system_clock;
using namespace std::this_thread;
char direction='r';
int score = 0; 
pair<int, int> poisonous_food;
bool is_paused = false;

// New function to update and display high scores
void update_high_scores() {
    vector<int> high_scores;
    ifstream infile("high_scores.txt");
    int s;
    while (infile >> s) {
        high_scores.push_back(s);
    }
    infile.close();

    high_scores.push_back(score);
    sort(high_scores.rbegin(), high_scores.rend());

    ofstream outfile("high_scores.txt");
    for (size_t i = 0; i < high_scores.size() && i < 10; ++i) {
        outfile << high_scores[i] << endl;
    }
    outfile.close();

    cout << "--- Top 10 High Scores ---" << endl;
    for (int current_score : high_scores) {
        cout << current_score << endl;
    }
}


void input_handler(){
    // change terminal settings
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    // turn off canonical mode and echo
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    map<char, char> keymap = {{'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}, {'q', 'q'}, {'p', 'p'}};
    while (true) {
        char input = getchar();
        if (keymap.find(input) != keymap.end()) {
            if (input == 'p') {
                is_paused = !is_paused;
            } else {
                direction = keymap[input];
            }
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
            } else if (i == poisonous_food.first && j == poisonous_food.second) {
                cout << "💀";
            } else if (find(snake.begin(), snake.end(), make_pair(int(i), int(j))) != snake.end()) {
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

    // The logic to spawn the food in a free spot
    pair<int, int> food;
    do {
        food = make_pair(rand() % 10, rand() % 10);
    } while (find(snake.begin(), snake.end(), food) != snake.end());

    // Initial sleep duration (500ms)
    chrono::milliseconds sleep_duration(500); 

    // Spawn poisonous food in a free spot, not on the snake or the normal food
    do {
        poisonous_food = make_pair(rand() % 10, rand() % 10);
    } while (find(snake.begin(), snake.end(), poisonous_food) != snake.end() || (poisonous_food.first == food.first && poisonous_food.second == food.second));


    for(pair<int, int> head=make_pair(0,1);; head = get_next_head(head, direction)){
        // send the cursor to the top
        cout << "\033[H";
        if (is_paused) {
            cout << "Game is Paused! Press 'p' to resume." << endl;
            sleep_for(100ms); // Small delay to prevent busy-waiting
            continue;
        }
        // check self collision
        if (find(snake.begin(), snake.end(), head) != snake.end()) {
            system("clear");
            cout << "Game Over" << endl;
            update_high_scores();
            exit(0);
        } else if (head.first == poisonous_food.first && head.second == poisonous_food.second) {
            // New check for poisonous food
            system("clear");
            cout << "Game Over - You ate poisonous food!" << endl;
            update_high_scores();
            exit(0);
        } else if (head.first == food.first && head.second == food.second) {
            // grow snake
            // Regenerate food in a new, free spot
            do {
                food = make_pair(rand() % 10, rand() % 10);
            } while (find(snake.begin(), snake.end(), food) != snake.end() || (food.first == poisonous_food.first && food.second == poisonous_food.second));

            snake.push_back(head);
            score += 10; // Increase score when food is eaten
            // Decrease the sleep duration by 10ms for every food eaten
            // Ensure the sleep duration doesn't drop below 50ms
            if (sleep_duration.count() > 50) {
                sleep_duration = chrono::milliseconds(sleep_duration.count() - 10);
            }
        } else {
            // move snake
            snake.push_back(head);
            snake.pop_front();
        }
        render_game(10, snake, food);
        cout << "length of snake: " << snake.size() << endl;
        cout << "Score: " << score << endl; // Display the current score
    
        // Use the dynamic sleep duration
        sleep_for(sleep_duration);
    }
}