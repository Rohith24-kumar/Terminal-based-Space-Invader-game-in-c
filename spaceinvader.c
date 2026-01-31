#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define BASE_ENEMIES 3
#define MAX_ENEMIES 20
#define MAX_LEVEL 5
#define MAX_BULLETS 10
#define MAX_EBULLETS 30
#define START_LIVES 3

typedef struct {
    int x, y;
    int alive;
    int dx;
    int moveCounter;
} Enemy;

typedef struct {
    int x, y;
    int active;
} Bullet;

typedef struct {
    int x, y;
    int active;
} EBullet;

Bullet bullets[MAX_BULLETS];
EBullet ebullets[MAX_EBULLETS];

// Forward declarations
int start_menu();
int game_over_menu(int level);
int level_clear_menu(int level);
int hit_menu(int level, int lives);

// =====================
// ASCII TITLE
// =====================
void draw_title(int y, int x) {
    mvprintw(y - 4, x - 30, "   ____                        ___                 _             ");
    mvprintw(y - 3, x - 30, "  / ___| __ _ _ __ ___   ___  |_ _|_ __ ___  _   _| | _____ _ __ ");
    mvprintw(y - 2, x - 30, " | |  _ / _` | '_ ` _ \\ / _ \\  | || '_ ` _ \\| | | | |/ / _ \\ '__|");
    mvprintw(y - 1, x - 30, " | |_| | (_| | | | | | |  __/  | || | | | | | |_| |   <  __/ |   ");
    mvprintw(y + 0, x - 30, "  \\____|\\__,_|_| |_| |_|\\___| |___|_| |_| |_|\\__,_|_|\\_\\___|_|   ");
}

// =====================
// START MENU
// =====================
int start_menu() {
    int highlight = 0;
    const char *items[] = {"Start Game", "Quit Game"};

    while (1) {
        clear();
        int y, x;
        getmaxyx(stdscr, y, x);

        draw_title(y/2 - 5, x/2);
        mvprintw(y/2 + 1, x/2 - 10, "=== SPACE INVADER ===");

        for (int i = 0; i < 2; i++) {
            if (i == highlight) attron(A_REVERSE);
            mvprintw(y/2 + 4 + i, x/2 - 6, "%s", items[i]);
            if (i == highlight) attroff(A_REVERSE);
        }

        int ch = getch();
        if (ch == KEY_UP) highlight = (highlight + 1) % 2;
        if (ch == KEY_DOWN) highlight = (highlight + 1) % 2;
        if (ch == '\n') return highlight;

        refresh();
        usleep(9000);
    }
}

// =====================
// GAME OVER MENU
// =====================
int game_over_menu(int level) {
    int highlight = 0;
    const char *items[] = {"Retry Level", "Main Menu", "Quit Game"};

    while (1) {
        clear();
        int y, x; getmaxyx(stdscr, y, x);

        mvprintw(y/2 - 3, x/2 - 8, "=== GAME OVER ===");
        mvprintw(y/2 - 1, x/2 - 9, "You died at Level %d", level);

        for (int i = 0; i < 3; i++) {
            if (highlight == i) attron(A_REVERSE);
            mvprintw(y/2 + 2 + i, x/2 - 9, "%s", items[i]);
            if (highlight == i) attroff(A_REVERSE);
        }

        int ch = getch();
        if (ch == KEY_UP) highlight = (highlight + 2) % 3;
        if (ch == KEY_DOWN) highlight = (highlight + 1) % 3;
        if (ch == '\n') return highlight;

        refresh();
        usleep(8000);
    }
}

// =====================
// LEVEL CLEAR MENU
// =====================
int level_clear_menu(int level) {
    int highlight = 0;
    const char *items[] = {"Next Level", "Main Menu", "Quit Game"};

    while (1) {
        clear();
        int y, x; getmaxyx(stdscr, y, x);
        mvprintw(y/2 - 3, x/2 - 12, "=== LEVEL %d CLEARED! ===", level);

        for (int i = 0; i < 3; i++) {
            if (highlight == i) attron(A_REVERSE);
            mvprintw(y/2 + 2 + i, x/2 - 10, "%s", items[i]);
            if (highlight == i) attroff(A_REVERSE);
        }

        int ch = getch();
        if (ch == KEY_UP) highlight = (highlight + 2) % 3;
        if (ch == KEY_DOWN) highlight = (highlight + 1) % 3;
        if (ch == '\n') return highlight;

        refresh();
        usleep(8000);
    }
}

// =====================
// HIT POPUP MENU
// =====================
int hit_menu(int level, int lives) {
    int highlight = 0;
    const char *items[] = {"Retry Level", "Main Menu"};

    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    int box_w = 32, box_h = 8;
    int box_y = (rows - box_h) / 2;
    int box_x = (cols - box_w) / 2;

    nodelay(stdscr, FALSE);

    while (1) {
        clear();

        // Draw popup box
        for (int y = 0; y < box_h; y++) {
            for (int x = 0; x < box_w; x++) {
                int ry = box_y + y, rx = box_x + x;
                if (y == 0 || y == box_h - 1) mvaddch(ry, rx, '-');
                else if (x == 0 || x == box_w - 1) mvaddch(ry, rx, '|');
                else mvaddch(ry, rx, ' ');
            }
        }

        mvprintw(box_y + 1, box_x + 8, "YOU GOT HIT!");
        mvprintw(box_y + 2, box_x + 4, "Level: %d   Lives: %d", level, lives);

        for (int i = 0; i < 2; i++) {
            if (highlight == i) attron(A_REVERSE);
            mvprintw(box_y + 4 + i, box_x + 8, "%s", items[i]);
            if (highlight == i) attroff(A_REVERSE);
        }

        int ch = getch();
        if (ch == KEY_UP) highlight = (highlight + 1) % 2;
        if (ch == KEY_DOWN) highlight = (highlight + 1) % 2;
        if (ch == '\n') {
            nodelay(stdscr, TRUE);
            return highlight;
        }

        refresh();
    }
}

// =====================
// MAIN GAME LOOP
// =====================
int main() {
    srand(time(NULL));

    Enemy enemies_arr[MAX_ENEMIES];

    int term_y, term_x;
    int box_top, box_left, box_bottom, box_right;
    int box_height, box_width;

    int player_x;
    int level = 1;
    int frame = 0;
    int score = 0;
    int lives = START_LIVES;

    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);

start_menu_label:
    nodelay(stdscr, FALSE);
    int start_choice = start_menu();
    if (start_choice == 1) { endwin(); return 0; }

    nodelay(stdscr, TRUE);
    level = 1;
    score = 0;
    lives = START_LIVES;

start_level:
    frame = 0;
    flushinp();

    // Reset bullets and enemies
    for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = 0;
    for (int i = 0; i < MAX_EBULLETS; i++) ebullets[i].active = 0;
    for (int i = 0; i < MAX_ENEMIES; i++) enemies_arr[i].alive = 0;

    clear();
    refresh();
    nodelay(stdscr, TRUE);

    getmaxyx(stdscr, term_y, term_x);
    box_height = term_y * 0.75;
    box_width  = term_x * 0.75;

    box_top = (term_y - box_height) / 2;
    box_left = (term_x - box_width) / 2;
    box_bottom = box_top + box_height - 1;
    box_right  = box_left + box_width - 1;

    player_x = (box_left + box_right) / 2;

    int enemy_count = BASE_ENEMIES + level;
    if (enemy_count > MAX_ENEMIES) enemy_count = MAX_ENEMIES;

    int enemy_move_delay = 10 - level * 2;
    if (enemy_move_delay < 2) enemy_move_delay = 2;

    int global_speed = 90000 - level * 8000;
    if (global_speed < 35000) global_speed = 35000;

    int fallChance = 20 - level * 3;
    if (fallChance < 5) fallChance = 5;

    int shootChance = 40 - level * 5;
    if (shootChance < 6) shootChance = 6;

    // Spawn enemies
    for (int i = 0; i < enemy_count; i++) {
        enemies_arr[i].alive = 1;
        enemies_arr[i].x = rand() % (box_right - box_left - 6) + box_left + 3;
        enemies_arr[i].y = rand() % (box_bottom - box_top - 12) + box_top + 2;
        enemies_arr[i].dx = (rand() % 3) - 1;
        enemies_arr[i].dx = enemies_arr[i].dx == 0 ? 1 : enemies_arr[i].dx;
        enemies_arr[i].moveCounter = rand() % 8;
    }

    // =====================
    // GAME LOOP WITH FIX FOR INPUT LAG
    // =====================
    struct timespec last_update, current_time;
    clock_gettime(CLOCK_MONOTONIC, &last_update);
    const long frame_time_ns = (long)global_speed * 1000; // convert µs to ns

    while (1) {
        // Read input every iteration → NO DELAY
        int ch = getch();
        if (ch == KEY_LEFT && player_x > box_left + 3) player_x--;
        if (ch == KEY_RIGHT && player_x < box_right - 3) player_x++;
        if (ch == ' ') {
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (!bullets[i].active) {
                    bullets[i].active = 1;
                    bullets[i].x = player_x;
                    bullets[i].y = box_bottom - 3;
                    break;
                }
            }
        }

        // Check if it's time to update game state
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        long elapsed = (current_time.tv_sec - last_update.tv_sec) * 1000000000L +
                       (current_time.tv_nsec - last_update.tv_nsec);

        if (elapsed < frame_time_ns) {
            usleep(5000); // short sleep to reduce CPU, but keep input fast
            continue;
        }

        // Time to update game logic (at original speed)
        last_update = current_time;
        frame++;

        clear();

        // Draw border
        for (int x = box_left; x <= box_right; x++) {
            mvaddch(box_top, x, '-');
            mvaddch(box_bottom, x, '-');
        }
        for (int y = box_top; y <= box_bottom; y++) {
            mvaddch(y, box_left, '|');
            mvaddch(y, box_right, '|');
        }

        int center_x = (box_left + box_right) / 2;

        mvprintw(box_top - 1, center_x - 6, " LEVEL %d ", level);
        mvprintw(box_top - 1, box_left + 2, "Score: %d", score);
        mvprintw(box_top - 1, box_right - 14, "Lives: %d", lives);

        // Player
        mvprintw(box_bottom - 2, player_x - 1, "/\\");

        // Player bullets
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullets[i].active) {
                if (frame % 2 == 0) bullets[i].y--;
                if (bullets[i].y <= box_top + 1) bullets[i].active = 0;
                else mvprintw(bullets[i].y, bullets[i].x, "!");
            }
        }

        // Enemy bullets
        for (int i = 0; i < MAX_EBULLETS; i++) {
            if (ebullets[i].active) {
                ebullets[i].y++;
                if (ebullets[i].y >= box_bottom - 1)
                    ebullets[i].active = 0;
                else
                    mvprintw(ebullets[i].y, ebullets[i].x, "v");
            }
        }

        // Draw enemies
        int alive_left = 0;
        for (int i = 0; i < enemy_count; i++) {
            if (enemies_arr[i].alive) {
                mvprintw(enemies_arr[i].y, enemies_arr[i].x - 1, "[V]");
                alive_left++;
            }
        }

        // =====================
        // LEVEL CLEARED
        // =====================
        if (alive_left == 0) {
            if (level >= MAX_LEVEL) {
                clear();
                mvprintw(term_y/2, term_x/2 - 10, "CONGRATULATIONS!");
                mvprintw(term_y/2 + 1, term_x/2 - 12, "YOU BEAT ALL LEVELS!");
                mvprintw(term_y/2 + 3, term_x/2 - 14, "Press any key...");
                refresh();
                nodelay(stdscr, FALSE);
                getch();
                goto start_menu_label;
            }

            nodelay(stdscr, FALSE);
            int opt = level_clear_menu(level);
            if (opt == 0) { level++; nodelay(stdscr, TRUE); goto start_level; }
            if (opt == 1) goto start_menu_label;
            endwin();
            return 0;
        }

        // =====================
        // Enemy Movement + Smart Shooting (Option C)
        // =====================
        for (int i = 0; i < enemy_count; i++) {
            if (!enemies_arr[i].alive) continue;

            enemies_arr[i].moveCounter++;
            if (enemies_arr[i].moveCounter > enemy_move_delay) {
                enemies_arr[i].x += enemies_arr[i].dx;
                if (enemies_arr[i].x <= box_left + 2 || enemies_arr[i].x >= box_right - 2)
                    enemies_arr[i].dx *= -1;

                if (rand() % fallChance == 0)
                    enemies_arr[i].y++;

                enemies_arr[i].moveCounter = 0;
            }

            // If enemy reaches player level => HIT
            if (enemies_arr[i].y >= box_bottom - 3) {
                int choice = hit_menu(level, lives);

                if (choice == 0) {
                    lives--;
                    if (lives <= 0) {
                        int go = game_over_menu(level);
                        if (go == 0) { lives = START_LIVES; goto start_level; }
                        if (go == 1) goto start_menu_label;
                        endwin(); return 0;
                    }
                    goto start_level;
                } else {
                    goto start_menu_label;
                }
            }

            // Smart shooting: aligned with player?
            if (abs(enemies_arr[i].x - player_x) <= 1) {
                if (rand() % shootChance == 0) {
                    for (int b = 0; b < MAX_EBULLETS; b++) {
                        if (!ebullets[b].active) {
                            ebullets[b].active = 1;
                            ebullets[b].x = enemies_arr[i].x;
                            ebullets[b].y = enemies_arr[i].y + 1;
                            break;
                        }
                    }
                }
            }
        }

        // =====================
        // Enemy Bullet Collision with Player
        // =====================
        for (int b = 0; b < MAX_EBULLETS; b++) {
            if (!ebullets[b].active) continue;

            if (ebullets[b].y >= box_bottom - 2 &&
                ebullets[b].x >= player_x - 1 &&
                ebullets[b].x <= player_x + 1) {

                ebullets[b].active = 0;

                int choice = hit_menu(level, lives);

                if (choice == 0) {
                    lives--;
                    if (lives <= 0) {
                        int go = game_over_menu(level);
                        if (go == 0) { lives = START_LIVES; goto start_level; }
                        if (go == 1) goto start_menu_label;
                        endwin(); return 0;
                    }
                    goto start_level;

                } else {
                    goto start_menu_label;
                }
            }
        }

        // =====================
        // Player Bullet Hit Enemy
        // =====================
        for (int b = 0; b < MAX_BULLETS; b++) {
            if (!bullets[b].active) continue;

            for (int e = 0; e < enemy_count; e++) {
                if (enemies_arr[e].alive &&
                    bullets[b].y == enemies_arr[e].y &&
                    bullets[b].x >= enemies_arr[e].x - 1 &&
                    bullets[b].x <= enemies_arr[e].x + 1) {

                    enemies_arr[e].alive = 0;
                    bullets[b].active = 0;
                    score += 10;
                }
            }
        }

        refresh();
    }

    endwin();
    return 0;
}
