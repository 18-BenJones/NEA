#include <iostream>

void transpose(int* index, int len){
    int swap;
    for(int i = 0; i < len; i++){
        for(int j = i; j < len; j++){
            if(i==j) {
                continue;
            }
            else{
                swap = *(index + (len*j)+ i);
                *(index + (len*j)+ i) = *(index + (len*i)+ j);
                *(index + (len*i)+ j) = swap;
            }
        }
    }
}

void r1(int* index, int){
    int len = 4;
    int swap;
    printf("%p", index);
    printf("\n");
    for(int i; i < len; i++){
                printf("ff\n");
                for(int j = 0; j < (len)/2; j++){
                    swap = *(index + (len - j) + (len * i));
                    //*(index + (len - j) + (len * i)) = *(index+j+(len*i));
                    *(index+j+(len*i)) = swap;
                }    
            }

}

void reverse(int* index, int len, bool dir){
    int swap;
    if(dir == true){           // Reverse the columns
        if(len % 2 == 1) {
            for(int j = 0; j < (len-1)/2; j++){
                swap = *(index + len - j);
                *(index + len - j) = *(index+j);
                *(index+j) = swap;
                }
        }else{
            for(int i; i < 4; i++){
                printf("break");
                for(int j = 0; j < (4)/2; j++){
                    swap = *(index + (len - j) + (len * i));
                    *(index + (len - j) + (len * i)) = *(index+j+(len*i));
                    *(index+j+(len*i)) = swap;
                }    
            }
        }
    }else{                  // Reverse the rows
    return;
    }
}

/*void rotate(int* index, bool dir){
    // 1 is clockwise 90, 0 is counterclockwise
    transpose(int* index)
    reverse(int* index, bool dir)    
}*/

void seg(int* scp){
    *(scp+1) = 5;
}

int main(){
    int sc[5];
    int * scp = &sc[0];
    printf("alive\n");
    seg(scp);
    printf("%d", sc[1]);
}

/*
int main(){
    int tst[4][4] = {
        {11,12,13,14},
        {21,22,23,24},
        {31,32,33,34},
        {41,42,43,44}
    };
    int len = 4;
    int* tptr = &tst[0][0];

    
        printf("orig. Matrix:");
        printf("\n\n");

    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            printf("%d", tst[i][j]);
            printf(", ");
        }
        printf("\n");
    }
    printf("\n");
    printf("Altered Matrix:");
    printf("\n\n");
    transpose(tptr, 4);
    r1(tptr, 4);

    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            printf("%d", tst[i][j]);
            printf(", ");
        }
        printf("\n");
    }
}*/