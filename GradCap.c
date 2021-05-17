
#include<stdint.h>

/* pin name layout:
 *  
 *   TP
 * TL   TR
 *   MD
 * BL   BR
 *   BT
 */

#define TP 22
#define TL 23
#define TR 24
#define MD 25
#define BL 26
#define BR 27
#define BT 28

#define STARTPIN 22
#define NUMDISPS 4

#define COUNTERSTART 62

const uint8_t digits[][8] = {
  {TP, TL, TR, BL, BR, BT},     //0
  {TR, BR},                     //1
  {TP, TR, MD, BL, BT},         //2
  {TP, TR, MD, BR, BT},         //3
  {TL, TR, MD, BR},             //4
  {TP, TL, MD, BR, BT},         //5
  {TP, TL, MD, BL, BR, BT},     //6
  {TP, TR, BR},                 //7
  {TP, TL, TR, MD, BL, BR, BT}, //8
  {TP, TL, TR, MD, BR, BT},     //9
  {TP, TL, TR, MD, BL, BR},     //A
  {TL, MD, BL, BR, BT},         //b
  {TP, TL, BL, BT},             //C
  {TR, MD, BL, BR, BT},         //d
  {TP, TL, MD, BL, BT},         //E
  {TP, TL, MD, BL},             //F
  {MD}                          //None
};

const uint8_t digitLens[] = {6, 2, 5, 5, 4, 5, 6, 3, 7, 6, 6, 5, 4, 5, 5, 4, 1};

void setup() {
  Serial.begin(9600);
  for(int i = STARTPIN; i < STARTPIN + NUMDISPS * 7; i++)
    pinMode(i, OUTPUT);
  for(int i = COUNTERSTART; i < 8; i++)
    pinMode(i, OUTPUT);
  pinMode(A0, OUTPUT);
}


//BBP Method
int nthDigitOfPi(int n){
  return (4/(8*n+1)) - (2/(8*n+4)) - (1/(8*n+5)) - (1/(8*n+6));
}

uint8_t screen[4] = {8, 8, 8, 8};
uint8_t dSegment[4] = {0, 0, 0, 0}, lastDsegment[4] = {0, 0, 0, 0};
void displayDigits(bool change){
  for(int i = 0; i < 4; i++){
    digitalWrite(digits[screen[i]][lastDsegment[i]] + i * 7, LOW);
    digitalWrite(digits[screen[i]][dSegment[i]] + i * 7, HIGH);
    lastDsegment[i] = dSegment[i];
    if(dSegment[i] >= digitLens[screen[i]])
      dSegment[i] = 0;
    else
      dSegment[i]++;
  }
}

long currentPrime = 2;
long timeLastPrime = 0;
bool computeNextPrime(){
  unsigned long start = millis();
  static long checking = 2;
  static long rootChecking = 2;
  static long checkingAgainst = 2;
  bool newPrime = false;
  if(currentPrime > 9999){
    currentPrime = 2;
    checking = 2;
    rootChecking = 2;
    checkingAgainst = 2;
  }
  if(start - timeLastPrime > 100){
    while(start - millis() < 2){  //We are allowed at most to do 3 miliseconds of work per cycle
      if(checkingAgainst > rootChecking){ //We found a prime
        //Serial.println(checking);
        currentPrime = checking;
        newPrime = true;
        checking++;
        rootChecking = (long)sqrt(checking) + 1;
        checkingAgainst = 2;
        timeLastPrime = start;
      }
      if(checking % checkingAgainst == 0){
        checking++;
        rootChecking = (long)sqrt(checking) + 1;
        checkingAgainst = 2;
      }
      else
        checkingAgainst++;
    }
  }
  return newPrime;
}

void setScreen(uint8_t primeDigits[7]){
    for(int i = 0; i < 4; i++){ //Turn off the last displayed segments
      digitalWrite(digits[screen[i]][lastDsegment[i]] + i * 7, LOW);
      dSegment[i] = 0;
    }
    screen[0] = primeDigits[3];
    screen[1] = primeDigits[4];
    screen[2] = primeDigits[5];
    screen[3] = primeDigits[6];
}

bool formatPrime(bool newPrime){
  static uint8_t primeDigits[7] = {0, 0, 0, 0, 0, 0, 2};
  long primeCopy = currentPrime;
  if(newPrime){
    //Serial.println(screen[0]);
    for(int i = 0; i < 7; i++){ //extract the digits
      primeDigits[6 - i] = primeCopy % 10;
      primeCopy /= 10;
    }
    setScreen(primeDigits);
    //shiftPosition = 0;
    return true;
  }
  /*static unsigned long lastShiftTime = millis();
  if(millis() - lastShiftTime > 500){
    setScreen(primeDigits, shiftPosition++);
    lastShiftTime = millis();
    return true;
  }*/
  return false;
}

long lastLEDUpdateTime = 0;
uint8_t ledValue = 0;
void updateLeds(){
  long curTime = millis();
  if(curTime - lastLEDUpdateTime > 100){
    ledValue++;
    lastLEDUpdateTime = curTime;
  }
  static uint8_t currentLed = 0;
  analogWrite(COUNTERSTART + ((currentLed+6)%8), 0);
  analogWrite(COUNTERSTART + currentLed, (ledValue >> currentLed) & 1 ? 255 : 0);
  currentLed = (currentLed + 1) % 8;
  //delay(10);
}


void updateGlow(){
  static long lastGlowUpdateTime = millis();
  static uint8_t glowValue = 127;
  static uint8_t dir = 1;
  long curTime = millis();
  if(curTime - lastGlowUpdateTime > 10){
    glowValue = glowValue + dir;
    if(glowValue >= 255 || glowValue <= 127){
      dir = -dir;
    }
    lastGlowUpdateTime = curTime;
    analogWrite(A0, glowValue);
    Serial.println(glowValue);
  }
  //delay(10);
}

void loop(){
    updateGlow();
    updateLeds();
    bool newPrime = computeNextPrime();
    bool change = formatPrime(newPrime);
    displayDigits(change);
}
