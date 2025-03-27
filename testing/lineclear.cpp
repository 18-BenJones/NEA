#include<stdlib.h>
#include<stdio.h>

int board[20][10] = 
{{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{2,2,2,2,2,2,2,2,2,2},
{2,2,2,2,2,2,2,2,2,2},
{2,2,2,2,2,2,2,2,2,2},
{2,2,2,2,2,2,2,2,2,2},};

void printboard(){
        for(int i = 0; i < 20; i++){    
        for(int j = 0; j < 10; j++){
            printf("%d", board[i][j]);
        }
        printf("\n");
    }
}


int linescleared = 0;

void lineclear(){                               // FIX ME checks if any lines need clearing and clears them and adds to the score
    //int linescleared = 0;
    int zerocount = 10;
    for(int i = 19; i > 0; i--){
        for(int j = 0; j < 10; j ++){
            if (board[i][j] != 0){
                zerocount = zerocount - 1;
            }
        }
        if (zerocount == 0){
            for (int n = 19; n > 0; n--){
                for(int m = 0; m < 10; m++){
                    board[n][m] = board[n-1][m];
                }
            }
            linescleared = linescleared + 1;
            i = i+1;
        }
        zerocount = 10;
    }
}


int main(){
    printf("Unaltered:\n");
    printboard();
    printf("Altered:\n");
    lineclear();
    printboard();
    printf("Lines cleared: %d", linescleared);
}