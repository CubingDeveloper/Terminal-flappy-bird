#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <locale>
#include <thread>

#include <io.h>
#include <fcntl.h>
#include <conio.h> //for keyboard input 🔥

bool can_run = false;

#ifdef _WIN32
    #include <windows.h> //indent here is such a python thing of me to do lol
#endif

using namespace std;

int bird_y = 6;
int current_frame = 0;

bool should_game_over = false;
int score = 0;

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
        pipes.push_back(vector<int> {48, (rand() % 8) - 4});
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

void render_current_frame(vector<wstring> frame) {
    wstring frame_buffer = L"";
    for (wstring line_to_render : frame ) {

        frame_buffer += (line_to_render + L"\n");
        
    }
    system("cls");
    wcout << frame_buffer;

    
}

void move_bird(){
    bird_y = min(10, bird_y + ((_kbhit() && _getch() == ' ') ? 2 : -1));
    if (bird_y == 0 || bird_y == 10) {
        should_game_over = true;
    }
    for (vector<int> pipe : pipes) {
        if (pipe[0] != 10) continue;
        int offset_y = bird_y + pipe[1];
        
        if (offset_y < 4 || offset_y > 6 ) {
            should_game_over = true;
        } else {
            score++;
        }
        
    }
}

int main() {
    #ifdef _WIN32

    can_run = true;

    #endif    

    if (!can_run) {
        cout << "This program is only meant to be ran on windows operating systems.";
        return 0;
    }

    //Enable UTF 8 support in the console (thanks ChatGPT)
    SetConsoleOutputCP(CP_UTF8);
    _setmode(_fileno(stdout), _O_U8TEXT);

    cout << "Welcome :imp:";
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
            wcout << L"GAME OVER" << endl << L"SCORE: " << score << "\n\n\n" << L"Press enter to quit. ";
            cin.get();
            break;
        }

    }

    return 1;
}
