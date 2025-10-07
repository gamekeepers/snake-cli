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

    Cell food = game.generate_random_cell();
    game.set_direction('r');
    while(true){
        reset_cursor();
        // check self collision
        if (game.checkCollission(game.snake.get_next_position())) {
            system("clear");
            cout << "Game Over" << endl;
            exit(0);
        }else if (game.snake.contains(food)) {
            food = game.generate_random_cell();
            game.snake.grow();
            
        }else{
            game.snake.move();
        }

        game.render(food);
        
    }
}
