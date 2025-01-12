#include <UI.hpp>

//this function draws the current ui-state
void UI::handler(){

  if(clearIP){
    fillRectIP();
    clearIP = false;
  }
  drawtext(0,0,"IP: ",ST7735_RED,2);
  drawtext(0,20,(char *)IP.c_str(),ST7735_RED,1);  
  
  char msg[15];
  for(int i =0; i<4;i++)
  {
    if(currState[i] != lastState[i])
    {
      lastState[i] = currState[i];
      fillRectRL(i);
    }
    sprintf(msg, "rl%d: %s",i+1,RL[i].c_str());
    drawtext(0, 35 + 20*i ,msg,ST7735_RED,2);  
  }
}

void UI::setRelayState(int r, bool st){
  if(r>=0 && r < 4){
    currState[r] = st;
    if(st){
      RL[r] = "on";
    }
    else
    {
      RL[r] = "off";
    }
  }
}

void UI::setIP(String _IP){
IP = _IP;
clearIP = true;
}


void UI::Start(){
  drawtext(0,0,"IP: ",ST7735_RED,2);
  drawtext(0,20,"0,0,0,0",ST7735_RED,1);
  drawtext(0,35,"rl1: off",ST7735_RED,2);
  drawtext(0,55,"rl2: off",ST7735_RED,2);
  drawtext(0,75,"rl3: off",ST7735_RED,2);
  drawtext(0,95,"rl4: off",ST7735_RED,2);
}

void UI::Test(){
    
  beginGraphic();

  // line draw test
  testlines(ST77XX_YELLOW);
  delay(500);

  // optimized lines
  testfastlines(ST77XX_RED, ST77XX_BLUE);
  delay(500);

  testdrawrects(ST77XX_GREEN);
  delay(500);

  testfillrects(ST77XX_YELLOW, ST77XX_MAGENTA);
  delay(500);

  fillScreen();
  testfillcircles(10, ST77XX_BLUE);
  testdrawcircles(10, ST77XX_WHITE);
  delay(500);

  testroundrects();
  delay(500);

  testtriangles();
  delay(500);

  mediabuttons();
  delay(500);

  Serial.println("done");
  delay(1000);
}