 #include "FastLED.h"

#define NUM_LEDS (256*2)
#define LED_PIN 11

CRGB leds[NUM_LEDS];
 
#define buzzer 7 //buzzer to arduino pin 9

#define X_JOY A0
#define Y_JOY A1
#define SW_JOY 6



//////////////////////
const int dataPin =8;
const int clockPin =10;
const int latchPin =9;
const int digit0pin =2;
const int digit1pin =3;
const int digit2pin =4;
const int digit3pin =5;
int digit2segments []={
0b00111111 , // d i g i t 0
0b00000110 , // d i g i t 1
0b01011011 , // d i g i t 2
0b01001111 , // d i g i t 3
0b01100110 , // d i g i t 4
0b01101101 , // d i g i t 5
0b01111101 , // d i g i t 6
0b00000111 , // d i g i t 7
0b01111111, // d i g i t 8
0b01101111 }; // d i g i t 9


int xy_to_index(int x, int y){
  return x*16 + y;

}



void writeDigit ( int value , int digit )
{
    // t u r n o f f a l l d i g i t s
    digitalWrite ( digit0pin , HIGH );
    digitalWrite ( digit1pin , HIGH );
    digitalWrite ( digit2pin , HIGH );
    digitalWrite ( digit3pin , HIGH );
    // w r i t e d a t a t o s h i f t r e g i s t e r
    digitalWrite ( latchPin , LOW );
    shiftOut ( dataPin , clockPin , MSBFIRST , digit2segments [ value ]);
    digitalWrite ( latchPin , HIGH );
    // t u r n o n o n l y t h e d e s i r e d d i g i t
    switch ( digit ) {
    case 0:
    digitalWrite ( digit0pin , LOW );
    break ;
    case 1:
    digitalWrite ( digit1pin , LOW );
    break ;
    case 2:
    digitalWrite ( digit2pin , LOW );
    break ;
    case 3:
    digitalWrite ( digit3pin , LOW );
    break ;
    }
    // k e e p d i g i t o n f o r 1 0 m i l l i s e c o n d s
    delay (5);
}





typedef struct Joy {
  int x;
  int y;
  bool fire;
} Joy;


//typedef struct Player {
//  int x;
//  int y;
//}

int cur_x = 0;
int cur_y = 0;



Joy joy = (Joy){0,0,0};

void setup(){
 
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output


  pinMode(SW_JOY, INPUT);
  digitalWrite(SW_JOY, HIGH);
  
  Serial.begin(9600);

  
  FastLED.addLeds<WS2812B, LED_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setBrightness(50); //Number 0-255
  FastLED.clear();

  //////////////////
  pinMode ( latchPin , OUTPUT );
  pinMode ( dataPin , OUTPUT );
  pinMode ( clockPin , OUTPUT );
  pinMode ( digit0pin , OUTPUT );
  pinMode ( digit1pin , OUTPUT );
  pinMode ( digit2pin , OUTPUT );
  pinMode ( digit3pin , OUTPUT );

}

void loop(){


   int value  = 1000;
  int temp = value % 10;
  writeDigit ( temp,0);
  temp = (value/10)%10; 
  writeDigit ( temp,1);
  temp = (value/100)%10; 
  writeDigit ( temp,2);
  temp = (value/1000)%10; 
  writeDigit ( temp,3);

  for(int i = 0; i < 256;i++){
    leds[i] = CRGB::Blue;
  
  }
   for(int i = 256; i < 256*2;i++){
    leds[i] = CRGB::Red;
  
  }

  if(joy.fire){
    leds[xy_to_index(cur_x,cur_y)] = CRGB::Green;
  } else {
    leds[xy_to_index(cur_x,cur_y)] = CRGB::Red;
  }
  Serial.print("-----");
  Serial.print(cur_x);
  Serial.print(cur_y);
  
   FastLED.show();


  
  joy.x = analogRead(X_JOY);
  joy.y = analogRead(Y_JOY);
  joy.fire = !digitalRead(SW_JOY);

  Serial.print("X left: ");
  Serial.print(joy.x);
  Serial.print("\n");
  
  Serial.print("Y left: ");
  Serial.print(joy.y);
  Serial.print("\n");
  

  
  Serial.print("FIRE: ");
  Serial.print(joy.fire);
  Serial.print("\n");

//     for(int i = 0; i < 30000; i++){
//    
//      
//      tone(buzzer,500 + (int)(i/500));
//      
//     }
    if(false){
     for(int i = 0; i < 5000; i++){
    
     tone(buzzer, 400);
     }
     delay(2000);
      for(int i = 0; i < 5000; i++){
          tone(buzzer, 2000);
     }
     
     noTone(buzzer);
    }


    if(joy.y > 600/* && cur_y < 7*/) {cur_y ++; /*render[cursor.x][cursor.y-1] = already_on; */}
    if(joy.y < 400/* && cur_y > 0*/) {cur_y --; /*render[cursor.x][cursor.y+1] = already_on; */}
    if(joy.x > 600/* && cur_x < 7*/) {cur_x ++; /*render[cursor.x-1][cursor.y] = already_on; */}
    if(joy.x < 400/* && cur_x > 0*/) {cur_x --; /*render[cursor.x+1][cursor.y] = already_on; */}
//  

  
 


  delay(200);
}
