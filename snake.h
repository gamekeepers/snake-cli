#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <map>
#include <deque>
#include <algorithm>
#include <ctime>
#include <fstream>
using namespace std;
char direction='r';
int score = 0;
int game_speed = 500;
bool paused = false;
vector<int> high_scores;


void load_scores() {
    ifstream file("scores.txt");
    int s;
    while (file >> s && high_scores.size() < 10) {
        high_scores.push_back(s);
    }
}

void save_score(int score) {
    high_scores.push_back(score);
    sort(high_scores.rbegin(), high_scores.rend());
    if (high_scores.size() > 10) high_scores.resize(10);
    
    ofstream file("scores.txt");
    for (int s : high_scores) {
        file << s << "\n";
    }
}

void show_scores() {
    cout << "\nTOP 10 SCORES:\n";
    for (size_t i = 0; i < high_scores.size(); i++) {
        cout << i+1 << ". " << high_scores[i] << "\n";
    }
}

void check_input() {
    if (_kbhit()) {
        char input = _getch();
        map<char, char> keymap = {{'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}};
        if (keymap.find(input) != keymap.end() && !paused) {
            direction = keymap[input];
        } else if (input == 'p') {
            paused = !paused;
        } else if (input == 'q') {
            exit(0);
        }
    }
}


pair<int, int> spawn_food(const deque<pair<int, int>>& snake) {
    pair<int, int> food;
    do {
        food = make_pair(rand() % 10, rand() % 10);
    } while (find(snake.begin(), snake.end(), food) != snake.end());
    return food;
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
    srand(time(0));
    load_scores();
    system("cls");
    deque<pair<int, int>> snake;
    snake.push_back(make_pair(0,0));

    pair<int, int> food = spawn_food(snake);
    pair<int, int> poison = spawn_food(snake);
    
    for(pair<int, int> head=make_pair(0,1);; ){
        check_input();
        
        if (paused) {
            COORD coord = {0, 12};
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
            cout << "PAUSED - Press 'p' to resume";
            Sleep(100);
            continue;
        }
        
        head = get_next_head(head, direction);
        COORD coord = {0, 0};
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
        
        if (find(snake.begin(), snake.end(), head) != snake.end()) {
            system("cls");
            cout << "Game Over! Final Score: " << score << endl;
            save_score(score);
            show_scores();
            exit(0);
        }else if (head.first == poison.first && head.second == poison.second) {
            system("cls");
            cout << "Poisoned! Game Over! Final Score: " << score << endl;
            save_score(score);
            show_scores();
            exit(0);
        }else if (head.first == food.first && head.second == food.second) {
            score += 10;
            game_speed = max(100, game_speed - 20);
            food = spawn_food(snake);
            if (rand() % 3 == 0) poison = spawn_food(snake);
            snake.push_back(head);            
        }else{
            snake.push_back(head);
            snake.pop_front();
        }
        render_game(10, snake, food, poison);
        cout << "Score: " << score << " | Length: " << snake.size() << " | Speed: " << (600-game_speed) << " | Press 'p' to pause" << endl;
    
        Sleep(game_speed);
    }
}
