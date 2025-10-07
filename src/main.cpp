#include "snake.h"

int main(int argc, char *argv[]) {
    shared_ptr<InputManager> input_manager = make_shared<KeyboardInputManager>();
    Game game = Game(input_manager);

    thread input_thread(input_handler, std::ref(game));
    thread game_thread(game_play, std::ref(game));   
    input_thread.join();
    game_thread.join();
    return 0;
}
