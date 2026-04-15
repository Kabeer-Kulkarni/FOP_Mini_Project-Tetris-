 #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#include <fcntl.h>

#define WIDTH 10
#define HEIGHT 20

int field[HEIGHT][WIDTH] = {0};
int score = 0;

int shapes[7][4][4] = {
    {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}}, // I
    {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}, // O
    {{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}, // T
    {{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}, // L
    {{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}, // J
    {{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}, // S
    {{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}  // Z
};

int current[4][4];
int posX, posY;

void clearScreen() {
    printf("\033[2J\033[H");
}

void copyShape(int type) {
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            current[i][j] = shapes[type][i][j];
}

int collision(int x, int y) {
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            if(current[i][j]) {
                if(y+i >= HEIGHT || x+j < 0 || x+j >= WIDTH)
                    return 1;
                if(field[y+i][x+j])
                    return 1;
            }
    return 0;
}

void merge() {
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            if(current[i][j])
                field[posY+i][posX+j] = 1;
}

void newShape() {
    copyShape(rand()%7);
    posX = WIDTH/2 - 2;
    posY = 0;
}

void draw() {
    clearScreen();

    for(int i=0;i<HEIGHT;i++){
        for(int j=0;j<WIDTH;j++){
            int block = field[i][j];

            for(int x=0;x<4;x++)
                for(int y=0;y<4;y++)
                    if(current[x][y])
                        if(i==posY+x && j==posX+y)
                            block = 1;

            printf(block ? "[]" : "  ");
        }
        printf("\n");
    }
    printf("\nScore: %d\n", score);
    printf("Controls: A=Left  D=Right  S=Down  W=Rotate  Q=Quit\n");
}

void rotate() {
    int temp[4][4];

    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            temp[i][j] = current[3-j][i];

    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            current[i][j] = temp[i][j];

    if(collision(posX,posY))
        for(int i=0;i<4;i++)
            for(int j=0;j<4;j++)
                current[i][j] = shapes[0][i][j]; // simple fallback
}

void enableRawMode() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);

    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

int main() {
    srand(time(NULL));
    enableRawMode();
    newShape();

    while(1){
        draw();
        usleep(120000);

        char ch = getchar();
        if(ch=='q') break;
        if(ch=='a' && !collision(posX-1,posY)) posX--;
        if(ch=='d' && !collision(posX+1,posY)) posX++;
        if(ch=='s' && !collision(posX,posY+1)) posY++;
        if(ch=='w') rotate();

        if(!collision(posX,posY+1))
            posY++;
        else{
            merge();
            score += 10;
            newShape();
            if(collision(posX,posY)){
                clearScreen();
                printf("GAME OVER!\nFinal Score: %d\n", score);
                break;
            }
        }
    }

    return 0;
}
