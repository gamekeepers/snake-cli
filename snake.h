#pragma once

#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <stdlib.h>
#include <map>
#include <deque>
#include <algorithm>
#include <fstream>
#include <conio.h>   
#include <windows.h> 
#include <atomic>
#include <ctime>

using namespace std;
using std::chrono::system_clock;
using namespace std::this_thread;

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

constexpr int GRID_SIZE = 10;

static std::atomic<char> direction{'n'}; 
static std::atomic<bool> paused{false};
static std::atomic<int> score{0};
static int speed = 200; 
static vector<int> highScores;
static std::atomic<char> last_direction{'n'}; 

// Load high scores from file
void load_scores();
// Save high scores to file
void save_scores();
// Handles keyboard input (direction, pause, quit)
void input_handler();
// Draws the game board and UI
void render_game(int size, deque<pair<int,int>>& snake, pair<int,int> food, pair<int,int> poison);
// Returns next head position based on direction
pair<int,int> get_next_head(pair<int,int> current, char dir);
// Generate random food/poison position
pair<int,int> generate_food(int size, deque<pair<int,int>> &snake);
// Main game loop
void game_play();

inline void load_scores(){
    highScores.clear();
    ifstream in("scores.txt");
    if(!in.is_open()) return;
    int s;
    while(in>>s) highScores.push_back(s);
    in.close();
}

inline void save_scores(){
    highScores.push_back((int)score.load());
    sort(highScores.rbegin(), highScores.rend());
    if(highScores.size()>10) highScores.resize(10);
    ofstream out("scores.txt");
    for(auto s:highScores) out<<s<<"\n";
    out.close();
}

inline bool is_opposite(char a, char b){
    if(a=='u' && b=='d') return true;
    if(a=='d' && b=='u') return true;
    if(a=='l' && b=='r') return true;
    if(a=='r' && b=='l') return true;
    return false;
}

inline void input_handler(){
    while(true){
        if(_kbhit()){
            int c = _getch();
            if(c == 0 || c == 224){ 
                int c2 = _getch();
                char new_dir = direction.load();
                if(c2 == 72) new_dir = 'u';      
                else if(c2 == 80) new_dir = 'd'; 
                else if(c2 == 75) new_dir = 'l'; 
                else if(c2 == 77) new_dir = 'r';

                char cur = last_direction.load();
                if(cur!='n' && is_opposite(cur, new_dir)) {
                } else {
                    direction.store(new_dir);
                    last_direction.store(new_dir);
                }
            } else {
                char input = (char)c;
                char new_dir = direction.load();
                if(input=='w'||input=='W'||input=='o'||input=='O') new_dir = 'u';
                else if(input=='s'||input=='S'||input=='p'||input=='P') new_dir = 'd';
                else if(input=='a'||input=='A'||input=='k'||input=='K') new_dir = 'l';
                else if(input=='d'||input=='D'||input=='l'||input=='L') new_dir = 'r';
                else if(input=='x'||input=='X'){ 
                    paused.store(!paused.load());
                }
                else if(input=='q'||input=='Q'){ 
                    save_scores();
                    ExitProcess(0);
                }

                if(new_dir=='u'||new_dir=='d'||new_dir=='l'||new_dir=='r'){
                    char cur = last_direction.load();
                    if(cur!='n' && is_opposite(cur, new_dir)){
                    } else {
                        direction.store(new_dir);
                        last_direction.store(new_dir);
                    }
                }
            }
        }
        Sleep(20);
    }
}

inline void render_game(int size, deque<pair<int, int>> &snake, pair<int, int> food, pair<int,int> poison){
    COORD coord = {0,0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

    cout<<BOLD<<CYAN<<"===== 🐍 Snake CLI Game ====="<<RESET<<endl;

    cout << "+";
    for(int c=0;c<size;c++) cout << "-";
    cout << "+" << endl;

    for(int i=0;i<size;i++){
        cout << "|";
        for(int j=0;j<size;j++){
            if (i == food.first && j == food.second){
                cout << GREEN << "🍎" << RESET;
            } else if (i == poison.first && j == poison.second){
                cout << RED << "💀" << RESET;
            } else if (find(snake.begin(), snake.end(), make_pair(i, j)) != snake.end()) {
                if(snake.back().first==i && snake.back().second==j)
                    cout << YELLOW << "🐍" << RESET;
                else
                    cout << YELLOW << "o" << RESET;
            } else {
                cout << " ";
            }
        }
        cout << "|" << endl;
    }

    cout << "+";
    for(int c=0;c<size;c++) cout << "-";
    cout << "+" << endl;

    cout << BOLD << "Score: " << score.load() << RESET
         << " | Length: " << snake.size()
         << " | Speed (ms): " << speed
         << endl;

    if(direction.load()=='n') cout<<MAGENTA<<"Press O/P/K/L (or arrows/WASD) to start!"<<RESET<<endl;
    if(paused.load()) cout<<MAGENTA<<"[PAUSED] Press 'x' to resume"<<RESET<<endl;
    cout<<CYAN<<"Controls: O=Up, P=Down, K=Left, L=Right, X=Pause, Q=Quit"<<RESET<<endl;
}

inline pair<int,int> get_next_head(pair<int,int> current, char dir){
    if(dir=='n') return current;  
    pair<int, int> next = current;
    if(dir =='r'){
        next = make_pair(current.first, (current.second + 1) % GRID_SIZE);
    } else if (dir=='l'){
        next = make_pair(current.first, current.second==0 ? GRID_SIZE-1 : current.second-1);
    } else if(dir =='d'){
        next = make_pair((current.first + 1) % GRID_SIZE, current.second);
    } else if(dir =='u'){
        next = make_pair(current.first==0 ? GRID_SIZE-1 : current.first-1, current.second);
    }
    return next;
}

inline pair<int,int> generate_food(int size, deque<pair<int,int>> &snake){
    pair<int,int> f;
    do {
        f = make_pair(rand()%size, rand()%size);
    } while(find(snake.begin(), snake.end(), f)!=snake.end());
    return f;
}

inline void game_play(){
    system("cls");

    srand((unsigned)time(nullptr));
    load_scores();

    deque<pair<int,int>> snake;
    int mid = GRID_SIZE/2;
    snake.push_back(make_pair(mid, mid-2));
    snake.push_back(make_pair(mid, mid-1));
    snake.push_back(make_pair(mid, mid));

    pair<int, int> food = generate_food(GRID_SIZE, snake);
    pair<int, int> poison = generate_food(GRID_SIZE, snake);

    score.store(0);
    direction.store('n'); 
    last_direction.store('n');

    while(_kbhit()) _getch();

    render_game(GRID_SIZE, snake, food, poison);

    while(direction.load() == 'n') {
        Sleep(30);
    }

    while(true){
        if(paused.load()){
            render_game(GRID_SIZE, snake, food, poison);
            Sleep(100);
            continue;
        }

        char cur_dir = direction.load();
        pair<int,int> head = get_next_head(snake.back(), cur_dir);

        if(find(snake.begin(), snake.end(), head)!=snake.end()){
            system("cls"); 
            cout << RED << BOLD << "💥 Game Over! Final Score: " << score.load() << RESET << endl;
            save_scores();
            cout<<GREEN<<BOLD<<"🏆 High Scores:"<<RESET<<endl;
            for(size_t i=0;i<highScores.size();i++)
                cout<<i+1<<". "<<highScores[i]<<endl;
            return;
        }

        if(head==food){
            score.fetch_add(10);
            snake.push_back(head); // grow
            food = generate_food(GRID_SIZE, snake);
            poison = generate_food(GRID_SIZE, snake);
            if(score.load()%30==0 && speed>60) speed -= 30;
        }
        else if(head==poison){
            system("cls");
            cout << RED << BOLD << "☠️ You ate poison! Game Over!" << RESET << endl;
            save_scores();
            cout<<GREEN<<BOLD<<"🏆 High Scores:"<<RESET<<endl;
            for(size_t i=0;i<highScores.size();i++)
                cout<<i+1<<". "<<highScores[i]<<endl;
            return;
        }
        else {
            snake.push_back(head);
            snake.pop_front();
        }

        render_game(GRID_SIZE, snake, food, poison);
        last_direction.store(cur_dir);
        Sleep(speed);
    }
}
