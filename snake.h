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
#include <fstream>

using namespace std;
using std::chrono::system_clock;
using namespace std::this_thread;
char direction='r';
vector<int> top_scores;
bool paused = false;
void load_top_scores() {
    ifstream infile("top_scores.txt");
    int s;
    top_scores.clear();
    while (infile >> s) {
        top_scores.push_back(s);
    }
    infile.close();
    sort(top_scores.rbegin(), top_scores.rend());
    if (top_scores.size() > 10) top_scores.resize(10);
}

void save_top_scores() {
    ofstream outfile("top_scores.txt");
    for (int s : top_scores) {
        outfile << s << endl;
    }
    outfile.close();
}

void input_handler(){
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    map<char, char> keymap = {{'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}};


    while (true) {
        char input = getchar();
        if (keymap.find(input) != keymap.end()) {
            direction = keymap[input];
        }else if(input == 'p') {
            paused = !paused;
        } 
        else if (input == 'q'){
            exit(0);
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}


void render_game(int size, deque<pair<int, int>> &snake, pair<int, int> food, pair<int,int> poison) {
    for (size_t i = 0; i < size; i++) {
        for (size_t j = 0; j < size; j++) {
            if (i == food.first && j == food.second) {
                cout << "🍎";  
            } else if (i == poison.first && j == poison.second) {
                cout << "☠️";
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
    load_top_scores();

    system("clear");
    deque<pair<int, int>> snake;
    snake.push_back(make_pair(0, 0));

    pair<int, int> food = generate_food(10, snake);
    pair<int, int> poison = generate_poison(10, snake, food);


    int food_eaten = 0;
    int speed = 500;
    int score = 0;
    int poison_timer = 0;

pair<int,int> head = make_pair(0, 1);
while (true) {
    cout << "\033[H";

    while (paused) {
        system("clear");
        render_game(10, snake, food, poison);
        cout << "=== PAUSED ===" << endl;
        cout << "Press 'p' to resume..." << endl;
        sleep_for(std::chrono::milliseconds(200));
        system("clear");
    }

    head = get_next_head(head, direction);

 if (find(snake.begin(), snake.end(), head) != snake.end()) {
    system("clear");

    top_scores.push_back(score);
    sort(top_scores.rbegin(), top_scores.rend());
    if (top_scores.size() > 10) top_scores.resize(10);
    save_top_scores();

    cout << "Game Over! Snake hit itself." << endl;
    cout << "Top 10 Scores:" << endl;
    for (int s : top_scores) cout << s << endl;

    exit(0);
} 
else if (head == poison) {
    system("clear");

    // Update top scores and save
    top_scores.push_back(score);
    sort(top_scores.rbegin(), top_scores.rend());
    if (top_scores.size() > 10) top_scores.resize(10);
    save_top_scores();

    cout << "Game Over! Snake ate poison ☠️." << endl;
    cout << "Top 10 Scores:" << endl;
    for (int s : top_scores) cout << s << endl;

    exit(0);
}


    else if (head.first == food.first && head.second == food.second) {
        food = generate_food(10, snake);
        snake.push_back(head);
        food_eaten++;
        score += 10;

        if (food_eaten % 10 == 0 && speed > 100) {
            speed += 50;  
        }
    } else {
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