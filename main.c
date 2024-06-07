
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int HEIGHT = 5, WIDTH = 10;

// Function to print the chessboard
void print_chessboard(WINDOW* win, int highlight_row, int highlight_col, int mode) {

    int row, col;
    for(row=0;row<8;++row){
        for(col=0;col<8;++col){
            int y = row*HEIGHT;
            int x = col*WIDTH;

            // alternate which color
            int color = (row + col) % 2 != 0 ? 1 : 2;

            if(mode!=2 && (row == highlight_row && col == highlight_col)) { wattron(win, COLOR_PAIR(3)); }
            else{ wattron(win, COLOR_PAIR(color)); }

            int i,j;
            for(i=0;i<HEIGHT;++i){
                for(j=0;j<WIDTH;++j){
                    mvwprintw(win, y + i, x + j, " ");
                }
            }

            if(mode!=2 && (row == highlight_row && col == highlight_col)) { wattroff(win,COLOR_PAIR(3)); }
            else{ wattroff(win, COLOR_PAIR(color)); }
        }
    }
    wrefresh(win);
}

void runLoop(WINDOW** win, int max_y, int mode){
    int highlight_row, highlight_col, correct, input_col, input_row, streak=0, completed=0, incorrect=0;
    char input[3];
    
    //positioning calculations:
    int game_stats_pos = WIDTH*8+5;
    int prompt_pos = HEIGHT*8;
    while (1) {
        highlight_row = rand() % 8;
        highlight_col = rand() % 8;

        // Print the chessboard with the highlighted square
        print_chessboard(*win, highlight_row, highlight_col, mode);

        // Prompt the user for input. Keep prompting until correct
        int level = 5;
        while(1){
            mvprintw(5, game_stats_pos, "Streak: %d", streak);
            mvprintw(6, game_stats_pos, "Completed: %d", completed);
            mvprintw(7, game_stats_pos, "Incorrect: %d", incorrect);

            if(mode == 1){
                mvprintw(prompt_pos+level, 0, "Enter the coordinates (e.g., a1): ");
                getstr(input);
                // for caps to be allowed
                //for(int i=0;input[i];++i){ input[i]=tolower(input[i]); }

                // Convert chess notation to row-column indices
                input_col=input[0]-'a'; int input_row=7-(input[1]-'1'); // 0,0 is at the bottom left
                correct = (input_row == highlight_row && input_col == highlight_col);
            }else{
                mvprintw(prompt_pos+level, 0, "Square to click: %c%c", 'a'+highlight_col, '1'+(7-highlight_row));
                MEVENT event;
                int ch;
                while ((ch = getch()) != KEY_MOUSE);
                if (getmouse(&event) == OK) {
                    correct = (event.y / HEIGHT == highlight_row && event.x / WIDTH == highlight_col);
                }else{ correct = 0; }
            }

            if(correct){
                mvprintw(prompt_pos+level+1, 0, "Correct!");
                ++streak;
                ++completed;
            } else {
                mvprintw(prompt_pos+level+1, 0, "Incorrect.");
                streak = 0;
                ++incorrect;
            }

            refresh();

            level+=2;
            if(HEIGHT*8+level >= max_y) break;

            if(correct){ break; }
        }
        usleep(330000);
        clear();
    }
}

int main() {
    int game_mode;
    printf("Type the coordinate (1) or Click the square (2): ");
    scanf("%d", &game_mode);

    WINDOW* win = initscr();
    int max_y, max_x;
    getmaxyx(win, max_y, max_x);
    max_y-=2;
    // keep the board size within the frame of the terminal window
    for(;HEIGHT*8 >= max_y; --HEIGHT){ WIDTH=HEIGHT*2; }

    // Assign colors
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_BLACK, COLOR_WHITE);
    init_pair(3, COLOR_WHITE, COLOR_RED); // for marking the square

    srand(time(NULL));

    if(game_mode == 2){
        noecho();
        curs_set(0);
        keypad(win, 1);      
        mousemask(ALL_MOUSE_EVENTS, NULL);
    }
    runLoop(&win,max_y,game_mode);

    endwin();
    return 0;
}
