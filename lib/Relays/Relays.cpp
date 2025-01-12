#include <Relays.h>


void Relays::handler(){
    std::lock_guard<std::mutex> guard(mu);
    //mains relay has inverted logic
    if(mainsTimer > 0){
        mainsTimer--;
        digitalWrite(mains,TURN_OFF);
    }else
    {
        digitalWrite(mains,TURN_ON);
    }

    for(int i = 0; i< RELAYS_INSTALLED; i++)
    {
        if(msTimer[i] > 0){
            msTimer[i]--;
            digitalWrite(dPin[i],TURN_ON);
        }
        else
        {
            digitalWrite(dPin[i],TURN_OFF);
        }
    }
}
void Relays::setState(relayState st){
    std::lock_guard<std::mutex> guard(mu);
    if(st.r>=0 && st.r<RELAYS_INSTALLED){
        if(st.st == true)
        {
            msTimer[st.r] = UP_TIME;
            mainsTimer = UP_TIME + 1000;
        }else
        {
            bool mainsStayOn = false;
            msTimer[st.r] = 0;
            for(int i = 0; i< RELAYS_INSTALLED; i++)
            {
                if(msTimer[i] > 0){
                    mainsStayOn = true;
                    break;
                }
            }
            if(!mainsStayOn)
            {
                mainsTimer = 1000;
            }
        }
    }    
}
void Relays::restart(int r){
    std::lock_guard<std::mutex> guard(mu);
    if(r>=0 && r<RELAYS_INSTALLED){
        msTimer[r] = UP_TIME;
        mainsTimer = UP_TIME + 1000;
    }
}