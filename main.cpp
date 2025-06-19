#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <locale>
#include <thread> //for waiting

#include <windows.h>
#include <fcntl.h> //for utf-8 command line
#include <conio.h> //for keyboard input 🔥

#include "parrot.hpp"

bool can_run = false;

using namespace std;

int bird_y = 6;
int current_frame = 0;
int score = 0;

bool should_game_over = false;
bool should_win = false; //YES!!!

WORD colors[] = {
    FOREGROUND_RED | FOREGROUND_INTENSITY,
    FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    FOREGROUND_RED | FOREGROUND_GREEN, // yellow-ish
    FOREGROUND_GREEN | FOREGROUND_BLUE, // cyan-ish
    FOREGROUND_RED | FOREGROUND_BLUE, // magenta-ish
    FOREGROUND_RED,
    FOREGROUND_GREEN,
    FOREGROUND_BLUE,
    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, // white (default)
};

HANDLE h_out = GetStdHandle(STD_OUTPUT_HANDLE);
COORD top_left = {0, 0};
CONSOLE_CURSOR_INFO cursor_info;

vector<vector<int>> pipes; //2D array, {Pipe: {X position, Y offset}}

//═║╔╗╚╝ box unicode
//☻ - bird?
//pipe █

//Screen template:
/*
←→−
↓↑│
←──────────────────────48──────────────────────→
╔══════════════════════════════════════════════╗ ↑
║ █        █        █        █        █        ║ │
║ █        █        █        █                 ║ │
║          █                 █                 ║ │
║                            █                 ║ │
║         ☻                           █        ║ 11
║ █                 █                 █        ║ │
║ █        █        █                 █        ║ │
║ █        █        █        █        █        ║ │
║ █        █        █        █        █        ║ │
╚══════════════════════════════════════════════╝ ↓

*/

void move_and_spawn_pipes(){
    if (current_frame % 9 == 0) {
        pipes.push_back(vector<int> {48, (rand() % 5) - 2});
    }

    bool should_delete_front = false;
    for (int i=0;i < pipes.size();i++) {
        int new_x = pipes[i][0] - 1;
        if (new_x == 0) should_delete_front = true;
        pipes[i][0] = new_x; //moves each pipe towards the end scaryy ik
    }
    if (should_delete_front) pipes.erase(pipes.begin());


}

vector<wstring> make_current_frame(){
    vector<wstring> frame_template = {
    L"╔══════════════════════════════════════════════╗",
    L"║                                              ║",
    L"║                                              ║",
    L"║                                              ║",
    L"║                                              ║",
    L"║                                              ║",
    L"║                                              ║",
    L"║                                              ║",
    L"║                                              ║",
    L"║                                              ║",
    L"╚══════════════════════════════════════════════╝"
    };
    

    for (int i = 1;i<10;i++) {
        for (vector<int> pipe : pipes) {

            if (pipe[0] == 0 || pipe[0] == 47) continue;
            int offset_i = i + pipe[1];
            if (offset_i >= 4 && offset_i <= 6) continue;
            frame_template[i][pipe[0]] = L'█';


        }
    }

    frame_template[10-bird_y][10] = L'☻';

    return frame_template;
}

void hide_cursor() {
    GetConsoleCursorInfo(h_out, &cursor_info);
    cursor_info.bVisible = FALSE;
    SetConsoleCursorInfo(h_out, &cursor_info);
}

void clear_console_text(HANDLE h_console) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD count;
    DWORD cellCount;
    COORD homeCoords = {0, 0};

    if (!GetConsoleScreenBufferInfo(h_console, &csbi)) return;
    cellCount = csbi.dwSize.X * csbi.dwSize.Y;

    // Fill the screen with spaces (erase text)
    FillConsoleOutputCharacter(h_console, L' ', cellCount, homeCoords, &count);

    // Keep existing attributes (colors)
    FillConsoleOutputAttribute(h_console, csbi.wAttributes, cellCount, homeCoords, &count);

    // Reset cursor position to top-left
    SetConsoleCursorPosition(h_console, homeCoords);
}


void render_current_frame(vector<wstring> frame) {
    hide_cursor();
    wstring frame_buffer = L"";
    for (wstring line_to_render : frame ) {

        frame_buffer += (line_to_render + L"\n");
        
    }
    SetConsoleCursorPosition(h_out, top_left);

    wcout << frame_buffer << endl << "SCORE: " << score << endl;
}

void move_bird(){
    bird_y = min(10, bird_y + ((_kbhit() && _getch() == ' ') ? 2 : -1));
    if (bird_y == 0 || bird_y == 10) {
        should_game_over = true;
    }
    for (vector<int> pipe : pipes) {
        if (pipe[0] != 10) continue;
        int offset_y = bird_y - pipe[1];
        
        if (offset_y < 4 || offset_y > 6 ) {
            should_game_over = true;
        } else {
            score++;
            if (score == 25) should_win = true;
        }
        
    }
}

int main() {
    //Enable UTF 8 support in the console (thanks ChatGPT)
    SetConsoleOutputCP(CP_UTF8);
    _setmode(_fileno(stdout), _O_U8TEXT);

    srand(time(0)); //Make a random seed based on the current UNIX time (I think idfk)
    
    while (true) {
        //Step 0: wait 💤💤💤
        this_thread::sleep_for(250ms);

        //Step 1: Handle movement
        move_and_spawn_pipes();
        move_bird();


        //Step 2: Make and render the frame
        vector<wstring> frame_to_render = make_current_frame();
        render_current_frame(frame_to_render);

        //Step 3: Increment current frame count
        current_frame++;

        //Step 4: Check if we should quit the game.
        if (should_game_over) {
            wcout << L"GAME OVER!\n\n\n" << L"Press enter to quit. ";
            cin.get();
            break;
        }
        if (should_win) {
            int current_parrot_frame = 0;
            this_thread::sleep_for(500ms);
            clear_console_text(h_out);
            while (true) {
                SetConsoleCursorPosition(h_out, top_left);
                SetConsoleTextAttribute(h_out, colors[current_parrot_frame%10]);
                wcout << L"YOU WON!!!!!!" << endl << L"(you do that by getting a score of 25)" << endl << L"very secret ik" << endl;
                wcout << get_frame(current_parrot_frame);
                wcout << endl << L"Press any key to quit";
                current_parrot_frame++;
                this_thread::sleep_for(200ms);
                if (_kbhit()) break;
            }
            break;
        }

    }

    return 1;
}
