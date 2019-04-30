/*
Mason Klenklen
2018
*/


#include "U8glib.h"

// setup u8g object, please remove comment from one of the following constructor calls
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);

#define KEY_NONE 0
#define KEY_PREV 1
#define KEY_NEXT 2
#define KEY_SELECT 3
#define KEY_BACK 4

#define SHUTTERPIN 6

uint8_t uiKeyPrev = 5;
uint8_t uiKeyNext = 4;
uint8_t uiKeySelect = 3;
uint8_t uiKeyBack = 8;

uint8_t uiKeyCodeFirst = KEY_NONE;
uint8_t uiKeyCodeSecond = KEY_NONE;
uint8_t uiKeyCode = KEY_NONE;

boolean SubMenu = false;
boolean Expose = false;

uint8_t NumberOfPics;
uint8_t InitialDelay;
uint8_t MirLockupDelay;
uint8_t Exposure;
uint8_t Interval;
uint8_t CurentPic;

char buf[4];

unsigned long ms;
unsigned long msLast;

/*
START_IMAGING
INITIAL_DELAY
MIR_LOCKUP_DELAY
EXPOSURE
INTERVAL
NUMBER_OF_PICS 
/*
NumberOfPics
InitialDelay
MirLockupDelay
Exposure
Interval
CurentPic
*/

void uiSetup(void) {
  // configure input keys 
  
  pinMode(uiKeyPrev, INPUT_PULLUP);           // set pin to input with pullup
  pinMode(uiKeyNext, INPUT_PULLUP);           // set pin to input with pullup
  pinMode(uiKeySelect, INPUT_PULLUP);           // set pin to input with pullup
  pinMode(uiKeyBack, INPUT_PULLUP);           // set pin to input with pullup
}

void uiStep(void) {
  uiKeyCodeSecond = uiKeyCodeFirst;
  if ( digitalRead(uiKeyPrev) == LOW )
    uiKeyCodeFirst = KEY_PREV;
  else if ( digitalRead(uiKeyNext) == LOW )
    uiKeyCodeFirst = KEY_NEXT;
  else if ( digitalRead(uiKeySelect) == LOW )
    uiKeyCodeFirst = KEY_SELECT;
  else if ( digitalRead(uiKeyBack) == LOW )
    uiKeyCodeFirst = KEY_BACK;
  else 
    uiKeyCodeFirst = KEY_NONE;
  
  if ( uiKeyCodeSecond == uiKeyCodeFirst )
    uiKeyCode = uiKeyCodeFirst;
  else
    uiKeyCode = KEY_NONE;
}


#define MENU_ITEMS 6
const char *menu_strings[MENU_ITEMS] = { "Start Imaging", "Initial Delay", "Mir Lockup Delay", "Exposure", "Interval", "Number of Pics" };

#define START_IMAGING 0
#define INITIAL_DELAY 1
#define MIR_LOCKUP_DELAY 2
#define EXPOSURE 3
#define INTERVAL 4
#define NUMBER_OF_PICS 5

uint8_t menu_current = 0;
uint8_t menu_redraw_required = 0;
uint8_t last_key_code = KEY_NONE;



void Expose(void) {
  do
  {
/*
NumberOfPics
InitialDelay
MirLockupDelay
Exposure
Interval
CurentPic
*/
    //shutter fire
    digitalWrite(SHUTTERPIN, HIGH);
    digitalWrite(13, HIGH);
    
    //delay minimum shutter pulse
    delay(200);
    
    //shutter release
    digitalWrite(SHUTTERPIN, LOW);
    digitalWrite(13, LOW);

    //delay for MirLockupDelay
    delay(750);

    //shutter fire
    digitalWrite(SHUTTERPIN, HIGH);
    digitalWrite(13, HIGH);
    
    //delay for Exposure
    delay(1300);
    
    //shutter release
    digitalWrite(SHUTTERPIN, LOW);
    digitalWrite(13, LOW);
    
    //delay for Interval
    
    CurentPic++;
    //refresh display
    u8g.firstPage();
    drawSubMenu();
  }
  while(CurentPic < NumberOfPics);
  
  if(CurentPic >= NumberOfPics)
  {
    CurentPic = 0;
    Expose = false;
  }
}

void drawMenu(void) {
  uint8_t i, h;
  u8g_uint_t w, d;

  u8g.setFont(u8g_font_6x13);
  u8g.setFontRefHeightText();
  u8g.setFontPosTop();
  
  h = u8g.getFontAscent()-u8g.getFontDescent();
  w = u8g.getWidth();
  for( i = 0; i < MENU_ITEMS; i++ ) {
    d = (w-u8g.getStrWidth(menu_strings[i]))/2;
    u8g.setDefaultForegroundColor();
    if ( i == menu_current ) {
      u8g.drawBox(0, i*h+1, w, h);
      u8g.setDefaultBackgroundColor();
    }
    u8g.drawStr(d, i*h, menu_strings[i]);
  }
}


void drawSubMenu(void)
{
  uint8_t i, h;
  u8g_uint_t w, d;

  u8g.setFont(u8g_font_6x13);
  u8g.setFontRefHeightText();
  u8g.setFontPosTop();
  
  h = u8g.getFontAscent()-u8g.getFontDescent();
  w = u8g.getWidth();
  
  d = (w-u8g.getStrWidth(menu_strings[menu_current]))/2;
  u8g.setDefaultForegroundColor();
    
  u8g.drawStr(d, h, menu_strings[menu_current]);
    
  switch (menu_current)
  {
    case START_IMAGING:
       
       itoa(CurentPic, buf, 10); //0
       u8g.drawStr(20, 40, buf);
       u8g.drawStr(50, 40, " of ");
       itoa(NumberOfPics, buf, 10); //100
       u8g.drawStr(90, 40, buf);
      break;
    case INITIAL_DELAY :
      
      itoa(InitialDelay, buf, 10);
      u8g.drawStr(50, 40, buf);
      break;
    case MIR_LOCKUP_DELAY:
      
      itoa(MirLockupDelay, buf, 10);
      u8g.drawStr(50, 40, buf);
      break;
    case EXPOSURE:
      
      itoa(Exposure, buf, 10);
      u8g.drawStr(50, 40, buf);
      break;
    case INTERVAL:
      
      itoa(Interval, buf, 10);
      u8g.drawStr(50, 40, buf);
      break;
    case NUMBER_OF_PICS:
      
      itoa(NumberOfPics, buf, 10);
      u8g.drawStr(50, 40, buf);
      break;

  }

}

void updateMenu(void) {
  if ( uiKeyCode != KEY_NONE && last_key_code == uiKeyCode ) {
    return;
  }
  last_key_code = uiKeyCode;
  
  switch ( uiKeyCode ) {
    case KEY_NEXT:
      if(!SubMenu)
      {
        menu_current++;
        if ( menu_current >= MENU_ITEMS )
          menu_current = 0;
        menu_redraw_required = 1;
      }
      else
      {
        //submenu action
        switch ( menu_current )
        {
          case START_IMAGING:
            SubMenu = false;
            //quit taking photos
            break;
            
          case INITIAL_DELAY:
            if( InitialDelay == 0 )
              InitialDelay = 0;
            else
              InitialDelay--;
            menu_redraw_required = 1;
            break;
            
          case MIR_LOCKUP_DELAY:
            if( MirLockupDelay == 0 )
              MirLockupDelay = 0;
            else
              MirLockupDelay--;
            menu_redraw_required = 1;
            break;
            
          case EXPOSURE:
            if( Exposure == 0 )
              Exposure = 0;
            else
              Exposure--;
            menu_redraw_required = 1;
            break;
            
          case INTERVAL:
            if( Interval == 0 )
              Interval = 0;
            else
              Interval--;
            menu_redraw_required = 1;
            break;
            
          case NUMBER_OF_PICS:
            if( NumberOfPics == 1 )
              NumberOfPics = 1;
            else
              NumberOfPics--;
            menu_redraw_required = 1;
            break;
        } 
      }
      break;
    case KEY_PREV:
      if(!SubMenu)
      {
        
        if ( menu_current == 0 )
          menu_current = MENU_ITEMS;
        menu_current--;
        menu_redraw_required = 1;
      }
      else
      {
        //submenu action
        switch ( menu_current )
        {
          case START_IMAGING:
            SubMenu = false;
            //quit taking photos
            break;
            
          case INITIAL_DELAY:
            InitialDelay++;
            if ( InitialDelay >= 120 )
              InitialDelay = 120;
            menu_redraw_required = 1;
            break;
            
          case MIR_LOCKUP_DELAY:
            MirLockupDelay++;
            if ( MirLockupDelay >= 25 )
              MirLockupDelay = 25;
            menu_redraw_required = 1;
            break;
            
          case EXPOSURE:
            Exposure++;
            if ( Exposure >= 254 )
              Exposure = 254;
            menu_redraw_required = 1;
            break;
            
          case INTERVAL:
            Interval++;
            if ( Interval >= 254 )
              Interval = 255;
            menu_redraw_required = 1;
            break;
            
          case NUMBER_OF_PICS:
            NumberOfPics++;
            if ( NumberOfPics >= 254 )
              NumberOfPics = 254;
            menu_redraw_required = 1;
            break;
            
        }
      }
      break;
    case KEY_SELECT:
      switch ( menu_current )
      {
        case START_IMAGING:
          
          if(SubMenu == true){
            SubMenu = false;
          }
          else {
            SubMenu = true;
          } 
          /*digitalWrite(13, HIGH);
          delay(120);
          digitalWrite(13,LOW); */
          Expose = true;
          menu_redraw_required = 1;
          break;
        case INITIAL_DELAY:
        
          if(SubMenu == true){
            SubMenu = false;
          }
          else {
            SubMenu = true;
          }
          /*digitalWrite(13, HIGH);
          delay(60);
          digitalWrite(13,LOW);
          delay(50); */
          menu_redraw_required = 1;
          break;
        case MIR_LOCKUP_DELAY:
          
          if(SubMenu == true){
            SubMenu = false;
          }
          else {
            SubMenu = true;
          }
          /*digitalWrite(13, HIGH);
          delay(30);
          digitalWrite(13,LOW);
          delay(50);
          digitalWrite(13, HIGH);
          delay(60);
          digitalWrite(13,LOW);
          delay(50); */
          menu_redraw_required = 1;
          break;
        case EXPOSURE:
          
          if(SubMenu == true){
            SubMenu = false;
          }
          else {
            SubMenu = true;
          }
          /*digitalWrite(13, HIGH);
          delay(30);
          digitalWrite(13,LOW);
          delay(50);
          digitalWrite(13, HIGH);
          delay(30);
          digitalWrite(13,LOW);
          delay(50);
          digitalWrite(13, HIGH);
          delay(30);
          digitalWrite(13,LOW);
          delay(50); */
          menu_redraw_required = 1;
          break;
        case INTERVAL:
          
          if(SubMenu == true){
            SubMenu = false;
          }
          else {
            SubMenu = true;
          }
          /*digitalWrite(13, HIGH);
          delay(30);
          digitalWrite(13,LOW);
          delay(50);
          digitalWrite(13, HIGH);
          delay(30);
          digitalWrite(13,LOW);
          delay(50);
          digitalWrite(13, HIGH);
          delay(30);
          digitalWrite(13,LOW);
          delay(50);
          digitalWrite(13, HIGH);
          delay(30);
          digitalWrite(13,LOW);
          delay(50); */
          menu_redraw_required = 1;
          break;
        case NUMBER_OF_PICS:
          
          if(SubMenu == true){
            SubMenu = false;
          }
          else {
            SubMenu = true;
          }
          /*digitalWrite(13, HIGH);
          delay(30);
          digitalWrite(13,LOW);
          delay(50);
          digitalWrite(13, HIGH);
          delay(30);
          digitalWrite(13,LOW);
          delay(50);
          digitalWrite(13, HIGH);
          delay(30);
          digitalWrite(13,LOW);
          delay(50);
          digitalWrite(13, HIGH);
          delay(30);
          digitalWrite(13,LOW);
          delay(50);
          digitalWrite(13, HIGH);
          delay(30);
          digitalWrite(13,LOW);
          delay(50); */
          menu_redraw_required = 1;
          break;
      }
      break;
  }
}


void setup() {
  // rotate screen, if required
  // u8g.setRot180();
  
  uiSetup();                                // setup key detection and debounce algorithm
  menu_redraw_required = 1;     // force initial redraw

  pinMode(13, OUTPUT); 
  SubMenu = false;

  NumberOfPics = 3;
  InitialDelay = 5;
  MirLockupDelay = 2;
  Exposure = 5;
  Interval = 1;
  CurentPic = 0;
}

void loop() {  

  uiStep();                                     // check for key press
    
  if (  menu_redraw_required != 0 ) {
    u8g.firstPage();
    do  {
      if(SubMenu){
        drawSubMenu();
      }
      else {
        drawMenu();
      }
    } while( u8g.nextPage() );
    menu_redraw_required = 0;
  }
  
  Expose();
  
  updateMenu();                            // update menu bar
  
}
