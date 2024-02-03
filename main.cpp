#include <iostream>
#include <thread>
#include <chrono>
#include <ncurses.h>
#include <cstdlib> 

using namespace std;

char board[20][20];
int snake_length = 3;

int apple_x = 0;
int apple_y = 0;

bool is_apple = false;
int score = 0;

struct bodyblock {
    int x, y;
    bodyblock* next;
};

enum Direction {
    snake_left,
    snake_right,
    snake_up,
    snake_down
};

Direction direction;
vector<bodyblock> snake;

void setup_colors() {
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK); // Snake color
    init_pair(2, COLOR_RED, COLOR_BLACK);   // Apple color
    init_pair(3, COLOR_WHITE, COLOR_BLACK); // Default color
}

void display_grid() {
    clear();

    printw("============== SNAKE GAME ============= \n"); //
    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 20; j++) {
            char symbol = board[i][j];
            if (symbol == '0') {
                attron(COLOR_PAIR(1));
                printw("%c ", symbol);
                attroff(COLOR_PAIR(1));
            } else if (symbol == '1') {
                attron(COLOR_PAIR(2));
                printw("%c ", symbol);
                attroff(COLOR_PAIR(2));
            } else {
                attron(COLOR_PAIR(3));
                printw("%c ", symbol);
                attroff(COLOR_PAIR(3));
            }
        }
        printw("\n");
    }

    printw("Score: %d", score); // Print the score
    refresh();
}

void render_snake() {
    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 20; j++) {
            if (board[i][j] != '1') {
                board[i][j] = '-';
            }
        }
    }
    for (vector<bodyblock>::iterator itr = snake.begin(); itr != snake.end(); itr++) {
        int x = (*itr).x;
        int y = (*itr).y;
        board[x][y] = '0';
    }
}

void get_input() {
    timeout(0); // Set a non-blocking timeout
    int ch = getch();

    if (ch != ERR) { // Check if a key is pressed
        switch (ch) {
            case KEY_UP:
                if (direction == snake_down) {
                    break;
                }
                direction = snake_up;
                break;
            case KEY_DOWN:
                if (direction == snake_up) {
                    break;
                }
                direction = snake_down;
                break;
            case KEY_LEFT:
                if (direction == snake_right) {
                    break;
                }
                direction = snake_left;
                break;
            case KEY_RIGHT:
                if (direction == snake_left) {
                    break;
                }
                direction = snake_right;
                break;
            case 'q':
                endwin();
                exit(EXIT_SUCCESS);
        }
    }
}

void move_snake() {
    for (int i = snake_length - 1; i > 0; i--) {
        snake[i].x = snake[i - 1].x;
        snake[i].y = snake[i - 1].y;
    }

    if (direction == snake_up) {
        if (snake[0].x == 0) {
            snake[0].x = 19;
        } else {
            snake[0].x--;
        }
    } else if (direction == snake_down) {
        if (snake[0].x == 19) {
            snake[0].x = 0;
        } else {
            snake[0].x++;
        }
    } else if (direction == snake_right) {
        if (snake[0].y == 19) {
            snake[0].y = 0;
        } else {
            snake[0].y++;
        }
    } else if (direction == snake_left) {
        if (snake[0].y == 0) {
            snake[0].y = 19;
        } else {
            snake[0].y--;
        }
    }

    this_thread::sleep_for(chrono::milliseconds(100));
}

void check_collision() {
    for (int i =1 ; i < snake_length; i++) {
        if(snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            attron(COLOR_PAIR(3)); // Set color to default
            mvprintw(22, 0, "Game Over!");
            attroff(COLOR_PAIR(3));
            refresh();
            
            exit(EXIT_SUCCESS);
        }
    }
}

void check_apples() {
    if (snake[0].x == apple_x && snake[0].y == apple_y) {
        bodyblock new_block;

        bodyblock last = snake.back();
        if (last.next->x < last.x) {
            new_block.x = last.x + 1;
            new_block.y = last.y;
        }
        if (last.next->x > last.x) {
            new_block.x = last.x - 1;
            new_block.y = last.y;
        }
        if (last.next->y > last.y) {
            new_block.x = last.x;
            new_block.y = last.y - 1;
        }
        if (last.next->y < last.y) {
            new_block.x = last.x;
            new_block.y = last.y + 1;
        }

        new_block.next = &(snake.back());  // Set next to nullptr to terminate the list
        snake.push_back(new_block);
        snake_length++;
        score++;
        is_apple = false;  // Reset is_apple after generating an apple
    }
}

void generate_apples() {
    if (!is_apple) {
        int rand_x = rand() % 20;
        int rand_y = rand() % 20;

        if (board[rand_x][rand_y] == '-') {
            board[rand_x][rand_y] = '1';
            apple_x = rand_x;
            apple_y = rand_y;
            is_apple = true;
        }
    }
    check_apples();
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    setup_colors();  // Call this function to set up colors

    bodyblock head;
    head.x = 10;
    head.y = 10;
    head.next = nullptr;
    snake.push_back(head);

    bodyblock neck;
    neck.x = 11;
    neck.y = 10;
    neck.next = &head;
    snake.push_back(neck);

    direction = snake_right;
   

    while (true) {
        get_input();
        move_snake();
        render_snake();
        display_grid();
        generate_apples();
        check_collision();
    }

    endwin(); // Cleanup and close ncurses

    return 0;
}
