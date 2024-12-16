#include<vector>
#include<string>

int board[20][10];

class trt{
    private:
    std::string name;
    int state[4][4] = {
        {0,0,0,0},
        {0,0,0,0},
        {0,0,0,0},
        {0,0,0,0}
    };

    public:
    trt(std::string name, char piece) {

    }; 
    void setpeice() {};
    void rotate() {};

}