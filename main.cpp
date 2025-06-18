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

const int FPS = 10;

#ifdef _WIN32
    #include <windows.h> //indent here is such a python thing of me to do lol
#endif

using namespace std;

int bird_y = 6;
int current_frame = 0;


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
║ █        █        █        █        █        ║ │
║ █        █        █        █                 ║ │
║          █                 █                 ║ │
║         ☻                  █        █        ║ 11
║ █                 █                 █        ║ │
║ █        █        █                 █        ║ │
║ █        █        █        █        █        ║ │
║ █        █        █        █        █        ║ │
╚══════════════════════════════════════════════╝ ↓

*/

void move_and_spawn_pipes(){
    if (current_frame % 7 == 0) {
        pipes.push_back(vector {48, 1});
    }

    for (int i=0;i < pipes.size();i++) {
        pipes[i][0] -= 1; //moves each pipe towards the end scaryy ik
    }


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
    frame_template[10-bird_y][10] = L'☻';
    return frame_template;
}

void render_current_frame(vector<wstring> frame) {

    system("cls");
    for (wstring line_to_render : frame ) {

        wcout << line_to_render << endl;
        
    }
    
}

void move_bird(){
    bird_y += (_kbhit() && _getch() == ' ') ? 2 : -1;
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
        this_thread::sleep_for(1s);
        
        //Step 1: Handle movement
        move_and_spawn_pipes();
        move_bird();

        //Step 2: Make and render the frame
        vector<wstring> frame_to_render = make_current_frame();
        render_current_frame(frame_to_render);
    }

    return 1;
}
