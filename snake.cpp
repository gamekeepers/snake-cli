#include "snake.h"

// Snake Class Implementation
Snake::Snake(int board_size) : board_size(board_size), direction('r') {
    body.push_front(Point{0, 0});
}

void Snake::move() {
    Point next_head = get_next_head();
    body.push_front(next_head);
    body.pop_back();
}

void Snake::grow() {
    Point next_head = get_next_head();
    body.push_front(next_head);
}

bool Snake::check_collision() {
    Point head = get_head();
    for (deque<Point>::const_iterator it = body.begin() + 1; it != body.end(); ++it) {
        if (head == *it) {
            return true;
        }
    }
    return false;
}

Point Snake::get_head() {
    return body.front();
}

const deque<Point>& Snake::get_body() const {
    return body;
}

void Snake::set_direction(char new_direction) {
    // FIX: Only prevent reversal if the snake is longer than 1 segment
    if (body.size() > 1 &&
       ((direction == 'r' && new_direction == 'l') ||
        (direction == 'l' && new_direction == 'r') ||
        (direction == 'u' && new_direction == 'd') ||
        (direction == 'd' && new_direction == 'u'))) {
        return;
    }
    direction = new_direction;
}

char Snake::get_direction() {
    return direction;
}

Point Snake::get_next_head() {
    Point current_head = get_head();
    Point next_head = current_head;
    if (direction == 'r') {
        next_head.y = (current_head.y + 1) % board_size;
    } else if (direction == 'l') {
        next_head.y = (current_head.y == 0) ? board_size - 1 : current_head.y - 1;
    } else if (direction == 'd') {
        next_head.x = (current_head.x + 1) % board_size;
    } else if (direction == 'u') {
        next_head.x = (current_head.x == 0) ? board_size - 1 : current_head.x - 1;
    }
    return next_head;
}

// Food Class Implementation
Food::Food(int board_size, const deque<Point>& snake_body) : board_size(board_size) {
    generate_new_food(snake_body);
}

void Food::generate_new_food(const deque<Point>& snake_body) {
    while (true) {
        position = Point{rand() % board_size, rand() % board_size};
        bool on_snake = false;
        for (deque<Point>::const_iterator it = snake_body.begin(); it != snake_body.end(); ++it) {
            if (position == *it) {
                on_snake = true;
                break;
            }
        }
        if (!on_snake) {
            break;
        }
    }
}

Point Food::get_position() {
    return position;
}

// Game Class Implementation
Game::Game(int size) : board_size(size), snake(size), food(size, snake.get_body()), game_over(false), input_thread(&Game::process_input, this) {}

void Game::run() {
    while (!game_over) {
        cout << "\033[H";
        update();
        render();
        cout << "Length of snake: " << snake.get_body().size() << endl;
        sleep_for(chrono::milliseconds(500));
    }

    input_thread.join();
    system("clear");
    cout << "Game Over" << endl;
}

void Game::render() {
    system("clear");
    for (int i = 0; i < board_size; ++i) {
        for (int j = 0; j < board_size; ++j) {
            Point p = {i, j};
            if (p == food.get_position()) {
                cout << "🍎";
            } else {
                bool is_snake_part = false;
                const deque<Point>& snake_body = snake.get_body();
                for (deque<Point>::const_iterator it = snake_body.begin(); it != snake_body.end(); ++it) {
                    if (p == *it) {
                        is_snake_part = true;
                        break;
                    }
                }
                if (is_snake_part) {
                    cout << "🐍";
                } else {
                    cout << "⬜";
                }
            }
        }
        cout << endl;
    }
}

void Game::update() {
    if (snake.check_collision()) {
        game_over = true;
        return;
    }

    if (snake.get_head() == food.get_position()) {
        snake.grow();
        food.generate_new_food(snake.get_body());
    } else {
        snake.move();
    }
}

void Game::process_input() {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    map<char, char> keymap;
    keymap['d'] = 'r';
    keymap['a'] = 'l';
    keymap['w'] = 'u';
    keymap['s'] = 'd';

    while (!game_over) {
        char input = getchar();
        if (keymap.count(input)) {
            snake.set_direction(keymap[input]);
        } else if (input == 'q') {
            game_over = true;
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}