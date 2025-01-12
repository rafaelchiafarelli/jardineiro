#ifndef UI_H
#define UI_H
#include <gf.h>
#include <mutex>

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 160

class UI{
    private:
        void Test();
        bool clearIP;
        String IP;
        String RL[4] = {"off","off","off","off"};
        bool currState[4] = {false,false,false,false};
        bool lastState[4] = {false,false,false,false};
        std::mutex mu;
        std::mutex mu1;
    public:
    void Start();
        void handler();
        void setRelayState(int r, bool st);
        void setIP(String IP);

        UI(){
            UI(DISPLAY_WIDTH, DISPLAY_HEIGHT,10);
            IP = "0,0,0,0";
            clearIP = false;
        }
        UI(int w, int h, int refRate){
            begin();
            fillScreen();
        }

};

#endif