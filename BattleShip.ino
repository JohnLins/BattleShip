 #include "FastLED.h"

#define NUM_LEDS (256*2)
#define LED_PIN 11
#define NUM_SHIP_PIX 22

CRGB leds[NUM_LEDS];
 
#define buzzer 7 

#define X_JOY A0
#define Y_JOY A1
#define SW_JOY 6




CRGB last_value;
int last_index;

CRGB last_value_inactive;
int last_index_inactive;

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


void fill_side(bool side, CRGB color){

  int start = 0;
  if(side == RIGHT){
    start = 256;
  }

  for(int i = start; i < start + 256; i++){
      leds[i] = color;
    
  }
  
}

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
    digitalWrite ( digit0pin , HIGH );
    digitalWrite ( digit1pin , HIGH );
    digitalWrite ( digit2pin , HIGH );
    digitalWrite ( digit3pin , HIGH );
    digitalWrite ( latchPin , LOW );
    shiftOut ( dataPin , clockPin , MSBFIRST , digit2segments [ value ]);
    digitalWrite ( latchPin , HIGH );
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
    delay (10);
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
  int ships[NUM_SHIP_PIX];// = (int *)malloc(sizeof(int)*6);
  int score;// = 0;
  //Cord *shots;// = (Cord *)malloc(0);
  Cord shots[20];
  int num_shots;// = 0;
} Player;

Player left = (Player){{xy_to_index(0,1, LEFT),
                          xy_to_index(0,2, LEFT),
                          xy_to_index(0,3, LEFT),
                          xy_to_index(0,4, LEFT),
                          xy_to_index(0,5, LEFT),
                          xy_to_index(2,9, LEFT),
                          xy_to_index(2,10, LEFT),
                          xy_to_index(2,11, LEFT),
                          xy_to_index(2,12, LEFT),
                          xy_to_index(5,8, LEFT),
                          xy_to_index(6,8, LEFT),
                          xy_to_index(7,8, LEFT),
                          xy_to_index(7,0, LEFT),
                          xy_to_index(8,0, LEFT),
                          xy_to_index(9,0, LEFT),
                          xy_to_index(11,3, LEFT),
                          xy_to_index(11,4, LEFT),
                          xy_to_index(12,12, LEFT),
                          xy_to_index(13,12, LEFT),
                          xy_to_index(14,12, LEFT),
                          xy_to_index(14,13, LEFT),
                          xy_to_index(14,14, LEFT)}, 0, /*(Cord *)malloc(0)*/{0,0}, 0};
Player right = (Player){{xy_to_index(0,6, RIGHT),
xy_to_index(1,6, RIGHT),
xy_to_index(2,6, RIGHT),
xy_to_index(1,0, RIGHT),
xy_to_index(1,1, RIGHT),
xy_to_index(4,13, RIGHT),
xy_to_index(5,13, RIGHT),
xy_to_index(6,13, RIGHT),
xy_to_index(5,12, RIGHT),
xy_to_index(5,11, RIGHT),
xy_to_index(8,9, RIGHT),
xy_to_index(8,10, RIGHT),
xy_to_index(8,11, RIGHT),
xy_to_index(8,12, RIGHT),
xy_to_index(12,11, RIGHT),
xy_to_index(12,12, RIGHT),
xy_to_index(12,13, RIGHT),
xy_to_index(10,2, RIGHT),
xy_to_index(11,2, RIGHT),
xy_to_index(12,2, RIGHT),
xy_to_index(13,2, RIGHT),
xy_to_index(14,2, RIGHT)}, 0, /*(Cord *)malloc(0)*/{0,0}, 0};

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
      right.shots[right.num_shots-1] = Cord{index, CRGB::Red};
  } else{
      right.shots[right.num_shots-1] = Cord{index, CRGB::Green};
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


int t = left.score * 100 + right.score;
int value;
value = t % 10; 
writeDigit (value ,3);
value = (t / 10) % 10; 
writeDigit (value ,2);
value = (t / 100) % 10; 
writeDigit (value ,1);
value = (t / 1000) % 10; 
writeDigit (value ,0);

  
if(active == LEFT){
  for(int i = 256; i < 256*2;i++){
    leds[i] = CRGB::Blue;
  }
  for(int i = 0; i < 256; i++){
    leds[i] = 0x000000;
  }
  for(int i = 0; i < NUM_SHIP_PIX; i++){
    leds[right.ships[i]] = 0xFFA500;
  }
  for(int i = 0; i < left.num_shots/*sizeof(right.shots)/sizeof(Cord)*/; i++){
    leds[left.shots[i].index]/*, leds[left.shots[i].index+256]*/ = left.shots[i].color;
  } 
  
} else {
  for(int i = 0; i < 256;i++){
    leds[i] = CRGB::Blue;
  }
  for(int i = 256; i < 256*2; i++){
    leds[i] = 0x000000;
  }
  for(int i = 0; i < NUM_SHIP_PIX; i++){
    leds[left.ships[i]] = 0xFFA500;
  }
  for(int i = 0; i < right.num_shots/*sizeof(right.shots)/sizeof(Cord)*/; i++){
    leds[right.shots[i].index]/*, leds[right.shots[i].index - 256]*/  = right.shots[i].color;
  } 
  
}

  


  if(joy.fire){
    if(leds[xy_to_index(cur_x, cur_y, !active)] == (CRGB)(0xFFA500)){
      Serial.print("SHOT");
      plus_score(active);
      add_shot(active, xy_to_index(cur_x, cur_y, active), 1); 
      
        for(int i = 0; i < 5000; i++){
          tone(buzzer, 2000);
        }
        noTone(buzzer);
    }else {
      add_shot(active, xy_to_index(cur_x, cur_y, active), 0); 
      Serial.print("MISS");
    }
    
  active = !active;
  cur_x = 0;
  cur_y = 0;
  } 
  
  else {
    leds[last_index] = last_value;
    last_value = leds[xy_to_index(cur_x,cur_y, active)];
    last_index = xy_to_index(cur_x,cur_y, active);
    leds[last_index] = (CRGB)(0xFFFFFF);

    leds[last_index_inactive] = last_value_inactive;
    last_value_inactive = leds[xy_to_index(cur_x,cur_y, !active)];
    last_index_inactive = xy_to_index(cur_x,cur_y, !active);
    leds[last_index_inactive] = (CRGB)(0xFFFFFF);
    
  }


  
  if(right.score == 10 || left.score == 10){
     for(int i = 0; i < 5000; i++){
          tone(buzzer, 2000);
        }
        noTone(buzzer);
    fill_side(!active, CRGB::Red);
    fill_side(active, CRGB::Green);
   
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

  
 

delay(100);
 
}
