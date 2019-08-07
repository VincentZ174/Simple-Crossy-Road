#include <deque>
#include <ctime>
#include <vector>
#include <ncurses.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
using namespace std;
class cPlayer{
public:
    int x, y;
    cPlayer(int width){
        x = width/ 2;
        y = 0;
    }
};

class cLane{
private:
    deque<bool> cars;
public:
    cLane(int width){
        for(int i = 0; i < width; i++){
            cars.push_front(true);
        }
    }
    
    void Move(){
        if(rand() % 10 == 1){
            cars.push_front(true);
        }
        else{
            cars.push_front(false);
        }
        cars.pop_back();
    }
    
    bool CheckPos(int pos){
        return cars[pos];
    }
};

class cGame
{
private:
    bool quit;
    int numberOfLanes;
    int width;
    int score;
    cPlayer *player;
    vector<cLane*> map;
    
public:
    int kbhit(void)
    {
        struct termios oldt, newt;
        int ch;
        int oldf;
        
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
        
        ch = getchar();
        
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        fcntl(STDIN_FILENO, F_SETFL, oldf);
        
        if(ch != EOF)
        {
            ungetc(ch, stdin);
            return 1;
        }
        
        return 0;
    }

    cGame(int w = 20, int h = 10){
        numberOfLanes = h;
        width = w;
        quit = false;
        for(int i = 0; i < numberOfLanes; i++){
            map.push_back(new cLane(width));
        }
        player = new cPlayer(width);
    }
    void Draw(){
        erase();
        for(int i = 0; i < numberOfLanes; i++){
            for(int j = 0; j < width; j++){
                if(i == 0 && (j == 0 || j == width - 1)){
                    mvprintw(0,0,"|");
                    mvprintw(0,width-1, "|");
                    refresh();
                }
                if(i == 0 && (j == 0 || j == width - 1)){
                    mvprintw(numberOfLanes,0,"|");
                    mvprintw(numberOfLanes,width-1, "|");
                    refresh();
                }
                if(map[i]->CheckPos(j) && i != 0 && i != numberOfLanes-1){
                    usleep(50);
                    mvprintw(i,j,"#");
                    refresh();
                }
                else if(player->x == j && player->y == i){
                    mvprintw(i,j,"V");
                    refresh();
                }
                else{
                    usleep(50);
                    mvprintw(i,j," ");
                    refresh();
                }
            }
            printw("\n");
            mvprintw(numberOfLanes+1 ,0,"SCORE: %d", score);
            refresh();
        }
    }
    
    void Input(){
        if(kbhit()){
            char current = getchar();
            if(current == 'a'){
                player->x--;
            }
            if(current == 'd'){
                player->x++;
            }
            if(current == 'w'){
                player->y--;
            }
            if(current == 's'){
                player->y++;
            }
            if(current == 'q'){
                quit = true;
            }
        }
    }
    
    void Logic(){
        for(int i = 1; i < numberOfLanes-1; i++){
            if(rand() % 10 == 1){
                map[i]->Move();
            }
            
            if(map[i]->CheckPos(player->x) && player->y == i){
                quit = true;
            }
            if(player->y == numberOfLanes - 1){
                flash();
                score++;
                player->y = 0;
            }
        }
    }
    
    void Run(){
        while(!quit){
            Input();
            Draw();
            Logic();
        }
        erase();
        printw("GAME OVER!\n");
        printw("SCORE: %d", score);
        refresh();
        getchar();
        endwin();
    }
};

int main(){
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    scrollok(stdscr, TRUE);
    srand(time(NULL));
    
    cGame game(30, 5);
    game.Run();

    return 0;
}
