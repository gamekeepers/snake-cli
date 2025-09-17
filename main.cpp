#include "snake.h"

int main(int argc, char *argv[]) {
    while (true) {
        game_running = true;
        direction = 'r';
        paused = false;

        thread input_thread(input_handler);
        bool finished = game_play();
        game_running = false;
        input_thread.join(); // clean exit

        char choice;
        cout << "\nDo you want to play again? (y/n): ";
        cin >> choice;

        if (choice != 'y' && choice != 'Y') break;
    }

    cout << "\nThanks for playing! Goodbye." << endl;
    return 0;
}
