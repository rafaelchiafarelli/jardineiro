#ifndef RELAYS_H
#define RELAYS_H
#include <stdlib.h>
#include <string>
#include <Arduino.h>
#include <mutex>
#define RELAYS_INSTALLED 4
#define TURN_ON 0
#define TURN_OFF 1
// 1000*60
#define UP_TIME 120000 

typedef struct{
  int r;
  bool st;
}relayState;

class Relays{
    private:
        int dPin[RELAYS_INSTALLED] = {33,14,27,26};
        int mains = 25;
        int msTimer[RELAYS_INSTALLED] = {0,0,0,0};
        int mainsTimer = 0;
        std::mutex mu;
    public:
        std::string Names[RELAYS_INSTALLED] = {"R1","R2","R3", "R4"};
        void handler();
        void restart(int r);
        bool getRelayState(int r){
          return msTimer[r]>0;
        }
        int getRelaysInstalled(){
          return RELAYS_INSTALLED;
        }
        void setState(relayState st);

        Relays(){
            pinMode(mains,OUTPUT);
            digitalWrite(mains,TURN_ON); //mains relay has inverted logic

            for(int p : dPin)
              {
                pinMode(p, OUTPUT);
                digitalWrite(p,TURN_OFF);
              }
        }

};

#endif