#include "snake.h"

void handle_single_input(Game& game){
    map<char, char> keymap = {{'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}, {'q', 'q'}};
    char input = game.getInputManager()->getInput();
    if (keymap.find(input) != keymap.end()) {
        game.set_direction(keymap[input]);
    }else if (input == 'q'){
        exit(0);
    }
}

void input_handler(Game& game){
    while (true) {
        handle_single_input(game);
    }
}

void reset_cursor(){
    cout << "[H";
}

void move_snake(deque<Cell> &snake, Cell new_head){
    snake.push_back(new_head);
    snake.pop_front();
}

void game_play(Game& game){
    system("clear");

    game.set_direction('r');
    while(true){
        reset_cursor();
        game.update();
        game.render();
        
    }
}
