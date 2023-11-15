#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <curses.h>
#include <string.h>

#define gotoxy(x,y) printf("%c[%d;%df",0x1B,y,x)

#define MAP_SIZE_X 20
#define MAP_SIZE_Y 13
#define MAX_SNAKE_SIZE (MAP_SIZE_X - 3) * (MAP_SIZE_Y - 2)

enum snakeDir
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
};

struct snake
{
    char c;
    uint8_t x[MAX_SNAKE_SIZE];
    uint8_t y[MAX_SNAKE_SIZE];
    uint8_t length;
    enum snakeDir dir;
};

struct fruit{
    char c;
    uint8_t x;
    uint8_t y;
};

int highScore = 0;

void initSnake(struct snake *snake) {
    snake->c = 'o';
    snake->length = 2;
    snake->x[0] = MAP_SIZE_X / 2;
    snake->y[0] = MAP_SIZE_Y / 2;
    snake->dir = UP;
}

bool isRunning = true;

const char originalMap[MAP_SIZE_Y][MAP_SIZE_X + 1] = {
    "###################\n",
    "#                 #\n",
    "#                 #\n",
    "#                 #\n",
    "#                 #\n",
    "#                 #\n",
    "#                 #\n",
    "#                 #\n",
    "#                 #\n",
    "#                 #\n",
    "#                 #\n",
    "#                 #\n",
    "###################\n"};

char map[MAP_SIZE_Y][MAP_SIZE_X + 1] = {
    "###################\n",
    "#                 #\n",
    "#                 #\n",
    "#                 #\n",
    "#                 #\n",
    "#                 #\n",
    "#                 #\n",
    "#                 #\n",
    "#                 #\n",
    "#                 #\n",
    "#                 #\n",
    "#                 #\n",
    "###################\n"};

void drawMap(struct snake *snake){
    clear();

    map[0][0] = '#';

    for (int i = 0; i < MAP_SIZE_Y; i++)
            printw("%s", map[i]);

    printw("Score: %d\tHigh Score: %d", snake->length - 2, highScore);
    refresh();
}

void setSnakePositionOnMap(struct snake* snake){
    for (uint8_t i = 0; i < snake->length; ++i){
        if(i == 0)
            map[snake->y[i]][snake->x[i]] = 'O';
        else 
            map[snake->y[i]][snake->x[i]] = snake->c;
    }
}

void clearSnakePositionOnMap(struct snake* snake){
    for (uint8_t i = 0; i < snake->length; ++i)
        map[snake->y[i]][snake->x[i]] = ' ';
}

void moveSnake(struct snake * snake){
    for (int i = snake->length - 1; i > 0; i--) {
        snake->x[i] = snake->x[i - 1];
        snake->y[i] = snake->y[i - 1];
    }

    switch (snake->dir) {
    case UP:
        --snake->y[0];
        break;
    case DOWN:
        ++snake->y[0];
        break;
    case LEFT:
        --snake->x[0];
        break;
    case RIGHT:
        ++snake->x[0];
        break;
    }
}

void generateFruit(struct fruit *fruit){
    fruit->x = rand() % (MAP_SIZE_X - 3);
    fruit->y = rand() % (MAP_SIZE_Y - 2);
    if(fruit->y == 0 || fruit->y == MAP_SIZE_Y || fruit->x == 0 || fruit->x == MAP_SIZE_X - 1)
        generateFruit(fruit);
    map[fruit->y][fruit->x] = fruit->c;
}

int main()
{
    struct snake *snake = (struct snake*)malloc(sizeof(struct snake));
    struct fruit *fruit = (struct fruit*)malloc(sizeof(struct fruit));
    fruit->c = '@';
    initSnake(snake);

    initscr(); 
    noecho();  
    nodelay(stdscr, TRUE); 
    keypad(stdscr, TRUE);  

    int ch;

    generateFruit(fruit);
    int time = clock();

    while(1){
        while (isRunning)
        {
            ch = getch();
            switch (ch)
            {
            case KEY_UP:
            case 'w':
            case 'W':
                if(snake->dir != DOWN)
                    snake->dir = UP;
                break;
            case KEY_DOWN:
            case 's':
            case 'S':
               if(snake->dir != UP)
                    snake->dir = DOWN;
                break;
            case KEY_LEFT:
            case 'a':
            case 'A':
               if(snake->dir != RIGHT)
                    snake->dir = LEFT;
                break;
            case KEY_RIGHT:
            case 'd':
            case 'D':
               if(snake->dir != LEFT)
                    snake->dir = RIGHT;
                break;
            case 27:
                isRunning = false;
                break;
            default:
                break;
            }

            if((clock() - time) / CLOCKS_PER_SEC >= 0.2){
                time = clock();
                moveSnake(snake);

                for (uint8_t i = 1; i < snake->length; ++i) {         
                    if(snake->x[0] == snake->x[i] && snake->y[0] == snake->y[i]){
                        isRunning = false;
                        break;
                    }
                }

                if(snake->x[0] == fruit->x && snake->y[0] == fruit->y){
                    ++snake->length;
                    generateFruit(fruit);
                }

                if(snake->x[0] == 0 || snake->y[0] == 0 || snake->x[0] == MAP_SIZE_X - 2 || snake->y[0] == MAP_SIZE_Y - 1){
                    if(snake->length - 1 >= highScore)
                        highScore = snake->length - 2;
                    isRunning = false;
                }

                setSnakePositionOnMap(snake);
                drawMap(snake);
                clearSnakePositionOnMap(snake);
            }
        }

        gotoxy(0, 14);
        printf("Game Over:\n");
        gotoxy(0, 15);
        printf("Score: %d\tHigh Score: %d\n", snake->length - 2, highScore);
        gotoxy(0, 16);
        printf("Do you want to restart? (y/n): ");

        char userInput = getchar();
    
        if (userInput == 'y' || userInput == 'Y') {
            isRunning = true;
            initSnake(snake);

            for (int i = 0; i < MAP_SIZE_Y; ++i) 
                strcpy(map[i], originalMap[i]);

            generateFruit(fruit);
        } else {
            break;
        }
    }

    endwin(); 
    free(snake);
    free(fruit);

    return 0;
}