#include<stdio.h>
#ifdef _WIN32
    #include<windows.h>
#else
    #include<unistd.h>
#endif


void msleep(int msec){   // Sleep for the requested number of milliseconds. System dependant   
    #ifdef _WIN32
        Sleep(milliseconds);
    #else

        usleep(msec * 1000);
    #endif
}

int main(){
    printf("thing one\n");
    msleep(3000);
    printf("thing two\n");
}