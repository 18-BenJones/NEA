#include <iostream>

void transpose(int* fpntr, int len){
    // Transposes the matrix (swaps elements about the centre)
    int swap;
    for(int i = 0; i < len; i++){
        for(int j = i; j < len; j++){
            if(i==j) {
                continue;                                       // ignore central line
            }
            else{
                swap = *(fpntr + (len*j)+ i);
                *(fpntr + (len*j)+ i) = *(fpntr + (len*i)+ j);
                *(fpntr + (len*i)+ j) = swap;
            }
        }
    }
}

void reverse(int* fpntr, int len, bool ccw){
    // Reverses either the rows or columns of a matrix
    int swap;
    if(ccw == 1){                                               // Revese the columns
        if(len % 2 == 1){
            for(int i = 0; i < (len-1)/2; i++){                 // ignore middle row
                for(int j = 0; j < len; j++){
                    swap = *(fpntr+j+((len*len)-(len*(i+1))));  //for a 4x4: f(0) = 12, f(1) = 8, thus f(i) = l^2-l(i+1) 
                    *(fpntr+j+((len*len)-(len*(i+1)))) = *(fpntr+j+(len*i));
                    *(fpntr+j+(len*i)) = swap;
                }
            }
        }else{
            for(int i = 0; i < len/2; i++){
                for(int j = 0; j < len; j++){
                    swap = *(fpntr+j+((len*len)-(len*(i+1))));  // same as above
                    *(fpntr+j+((len*len)-(len*(i+1)))) = *(fpntr+j+(len*i));
                    *(fpntr+j+(len*i)) = swap;
                }
            }
        }
    }
    else{                                                       // Reverse the rows
        if(len % 2 == 1){
            for(int i = 0; i < len; i++){
                for(int j = 0; j < ((len-1)/2)+1; j++){         // ignore middle column
                    swap = *(fpntr+(len-j-1)+(i*len));
                    *(fpntr+(len-j-1)+(i*len)) = *(fpntr+j+(len*i));
                    *(fpntr+j+(len*i)) = swap;
                }
            }
        }
        else{
            for(int i = 0; i < len; i++){
                for(int j = 0; j < (len)/2; j++){   
                    swap = *(fpntr+(len-j-1)+(i*len));
                    *(fpntr+(len-j-1)+(i*len)) = *(fpntr+j+(len*i));
                    *(fpntr+j+(len*i)) = swap;
                }    
            }
        }
    }
}

void rotate(int* fpntr, int len, bool ccw){
    // fpntr - front pointer, an int* to the first element [0][0]
    // len - length, the side length of the n x n matrix as an integer
    // ccw - counterclockwise, if 1 will rotate counterclockwise
    transpose(fpntr, len);
    reverse(fpntr, len, ccw);    
}

int main(){
    int matrix[4][4] = {    //      Should become:
        {11,12,13,14},      //      41 31 21 11
        {21,22,23,24},      //      42 32 22 12
        {31,32,33,34},      //      43 33 23 13
        {41,42,43,44}       //      44 34 24 14
    };
    int len = 4;
    int* mptr = &matrix[0][0];

    printf("orig. Matrix:\n");
    for (int i = 0; i < len; i++){
        for (int j = 0; j < len; j++){
            printf("%d", matrix[i][j]);
            printf(", ");
        }
        printf("\n");
    }
    printf("\n");
    
    rotate(mptr, len, 0);   // try 1 for counterclockwise

    printf("\nAltered Matrix:\n");
    for (int i = 0; i < len; i++){
        for (int j = 0; j < len; j++){
            printf("%d", matrix[i][j]);
            printf(", ");
        }
        printf("\n");
    }
}