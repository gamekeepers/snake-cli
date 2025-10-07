#include "snake.h"

void handle_single_input(Game& game){
    map<char, char> keymap = {{'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}, {'q', 'q'}};
    char input = game.getInputManager()->getInput();
    if (keymap.find(input) != keymap.end()) {
        game.set_direction(keymap[input]);
    }else if (input == 'q'){
        exit(0);
    }else if (input == 'p' || input == 'P'){
        game.togglePause();
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

bool display_game_over(Game& game){
    system("clear");
    cout << endl;
    cout << "╔════════════════════════════════╗" << endl;
    cout << "║         GAME OVER!            ║" << endl;
    cout << "╚════════════════════════════════╝" << endl;
    cout << endl;
    cout << "Final Score: " << game.getScore() << endl;
    cout << "Snake Length: " << game.getSize() << endl;
    cout << "Difficulty: ";
    if(game.getDifficulty() == 1) cout << "Easy";
    else if(game.getDifficulty() == 2) cout << "Medium";
    else cout << "Hard";
    cout << endl << endl;
    cout << "Thanks for playing! 🐍" << endl;
    cout << endl;
    cout << "Would you like to play again?" << endl;
    cout << "1. Play Again" << endl;
    cout << "2. Exit" << endl;
    cout << endl;
    cout << "Enter your choice (1-2): ";
    
    char choice;
    cin >> choice;
    return choice == '1';
}

void game_play(Game& game){
    system("clear");

    game.set_direction('r');
    while(!game.isGameOver()){
        reset_cursor();
        game.update();
        game.render();
    }
    
    // Display game over and return whether to play again
    bool play_again = display_game_over(game);
    if(!play_again){
        exit(0);
    }
}
