#include<stdio.h>
#include<stdlib.h>
#include<time.h>

// test the efficacy of the rand() function

int main(){
    srand(time(NULL));
    float sum = 0;
    float sum2 = 0;
    int tmp = 0;
    for (int i = 0; i < 1000; i++){
        tmp = rand()%7;
        sum = sum + (tmp+1);
        //printf("%d", tmp);
        //printf("\n");
    }
    sum = sum/1000;
    printf("%f", sum);
}