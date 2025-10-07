#include "snake.h"

void display_menu(){
    system("clear");
    cout << "╔════════════════════════════════╗" << endl;
    cout << "║         SNAKE GAME CLI         ║" << endl;
    cout << "╚════════════════════════════════╝" << endl;
    cout << endl;
    cout << "Select Difficulty:" << endl;
    cout << "1. Easy   (Slow)" << endl;
    cout << "2. Medium (Normal)" << endl;
    cout << "3. Hard   (Fast)" << endl;
    cout << endl;
    cout << "Enter your choice (1-3): ";
}

int main(int argc, char *argv[]) {
    int difficulty = 1;
    bool play_again = true;
    
    while(play_again){
        // Show menu and get difficulty selection
        if(argc > 1){
            difficulty = atoi(argv[1]);
            if(difficulty < 1 || difficulty > 3) difficulty = 1;
        } else {
            display_menu();
            char choice;
            cin >> choice;
            difficulty = choice - '0';
            if(difficulty < 1 || difficulty > 3) difficulty = 1;
        }
        
        shared_ptr<InputManager> input_manager = make_shared<KeyboardInputManager>();
        Game game = Game(input_manager, difficulty);

        thread input_thread(input_handler, std::ref(game));
        thread game_thread(game_play, std::ref(game));   
        input_thread.join();
        game_thread.join();
        
        // After threads join, check if we want to play again
        // The game_play function handles the exit if user chooses not to play
        // If we reach here after game_play exits normally, user chose to play again
        play_again = true;
    }
    return 0;
}
