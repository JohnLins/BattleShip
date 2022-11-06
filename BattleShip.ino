 #include "FastLED.h"

#define NUM_LEDS (256*2)
#define LED_PIN 11

CRGB leds[NUM_LEDS];
 
#define buzzer 7 //buzzer to arduino pin 9

#define X_JOY A0
#define Y_JOY A1
#define SW_JOY 6



CRGB last_value;
int last_index;

//////////////////////
#define dataPin 8
#define clockPin 10
#define latchPin 9
#define digit0pin 2
#define digit1pin 3
#define digit2pin 4
#define digit3pin 5
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


int cur_x = 0;
int cur_y = 0;

#define LEFT 0
#define RIGHT 1



bool active = LEFT;

int xy_to_index(int x, int y, bool side){
  int index = 0; 
  
  if(y % 2 == 0){
    index += 16 * y;
    index += x;
  }else {
    index += 16 * y + 15;
    index -= x;
  }

  if(side == RIGHT){
    index += 256;
  }

  if(index >=0 && index < 256*2){
    return index;
  }
  
  return 0;

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




typedef struct Cord {
  int index;
  CRGB color;
} Cord;
typedef struct Player {
  int ships[6];// = (int *)malloc(sizeof(int)*6);
  int score;// = 0;
  //Cord *shots;// = (Cord *)malloc(0);
  Cord shots[5];
  int num_shots;// = 0;
} Player;

Player left = (Player){{xy_to_index(1,0, LEFT),
                xy_to_index(0,1, LEFT),
                xy_to_index(0,2, LEFT),
                xy_to_index(0,3, LEFT),
                xy_to_index(0,4, LEFT),
                xy_to_index(0,5, LEFT)}, 0, /*(Cord *)malloc(0)*/{0,0}, 0};
Player right = (Player){{xy_to_index(1,0, RIGHT),
                xy_to_index(0,1, RIGHT),
                xy_to_index(0,2, RIGHT),
                xy_to_index(0,3, RIGHT),
                xy_to_index(0,4, RIGHT),
                xy_to_index(0,5, RIGHT)}, 0, /*(Cord *)malloc(0)*/{0,0}, 0};

void plus_score(bool active){
  if(active == LEFT){
    left.score++;
  } else {
    right.score++;
  }
}

void add_shot(bool active, int index, bool hit){
  Serial.print("......\n");
Serial.print(hit);
  Serial.print("......\n");
if(active == LEFT){
  left.num_shots++;
  
  //left.shots = (Cord *)realloc(left.shots, sizeof(Cord)*left.num_shots);
  
  if(hit){
      left.shots[left.num_shots-1] = Cord{index, CRGB::Red};
  } else{
      left.shots[left.num_shots-1] = Cord{index, CRGB::Green};
    }
} else {
  right.num_shots++;
  //right.shots = (Cord *)realloc(right.shots, sizeof(Cord)*right.num_shots);

  if(hit){
      right.shots[right.num_shots-1] = Cord{index+256, CRGB::Red};
  } else{
      right.shots[right.num_shots-1] = Cord{index+256, CRGB::Green};
    }
}
}
  


Joy joy = (Joy){0,0,0};

void setup(){
  
//left.ships = {xy_to_index(1,0, LEFT),
//xy_to_index(0,1, LEFT),
//xy_to_index(0,2, LEFT),
//xy_to_index(0,3, LEFT),
//xy_to_index(0,4, LEFT),
//xy_to_index(0,5, LEFT)};
//
//
//right.ships   = ;


  
 
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

if(active == LEFT){
  for(int i = 256; i < 256*2;i++){
    leds[i] = CRGB::Blue;
  }
  for(int i = 0; i < 256; i++){
    leds[i] = 0x000000;
  }
  for(int i = 0; i < 6; i++){
    leds[right.ships[i]] = 0xFFA500;
  }
  for(int i = 0; i < left.num_shots/*sizeof(right.shots)/sizeof(Cord)*/; i++){
    Serial.print(":");
    Serial.print(right.shots[i].index);
    leds[left.shots[i].index] = left.shots[i].color;
  } Serial.print("\n");
  
} else {
  for(int i = 0; i < 256;i++){
    leds[i] = CRGB::Blue;
  }
  for(int i = 256; i < 256*2; i++){
    leds[i] = 0x000000;
  }
  for(int i = 0; i < 6; i++){
    leds[left.ships[i]] = 0xFFA500;
  }
  for(int i = 0; i < right.num_shots/*sizeof(right.shots)/sizeof(Cord)*/; i++){
    Serial.print(":");
    Serial.print(right.shots[i].index);
    leds[right.shots[i].index] = right.shots[i].color;
  } Serial.print("\n");
  
}

  


  if(joy.fire){
    for(int i = 0; i < 5000; i++){
  tone(buzzer, 2000);
    }
    noTone(buzzer);

    if(leds[xy_to_index(cur_x, cur_y, !active)] == (CRGB)(0xFFA500)){
      Serial.print("SHOT");
      plus_score(active);
      add_shot(active, xy_to_index(cur_x, cur_y, active), 1); 
      
      
    }else {
      add_shot(active, xy_to_index(cur_x, cur_y, active), 0); 
      Serial.print("MISS");
    }
    
  active = !active;
  } 
  
  else {
    leds[last_index] = last_value;
    last_value = leds[xy_to_index(cur_x,cur_y, active)];
    last_index = xy_to_index(cur_x,cur_y, active);
    
    leds[last_index] = CRGB::Red;
    
  }
 
   FastLED.show();


  
  joy.x = analogRead(X_JOY);
  joy.y = analogRead(Y_JOY);
  joy.fire = !digitalRead(SW_JOY);


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


    if(joy.y > 600 && cur_y > 0) {cur_y --; /*render[cursor.x][cursor.y-1] = already_on; */}
    if(joy.y < 400 && cur_y < 15) {cur_y ++; /*render[cursor.x][cursor.y+1] = already_on; */}
    if(joy.x > 600 && cur_x < 15) {cur_x ++; /*render[cursor.x-1][cursor.y] = already_on; */}
    if(joy.x < 400 && cur_x > 0) {cur_x --; /*render[cursor.x+1][cursor.y] = already_on; */}
//  

  
 


  delay(200);
}
