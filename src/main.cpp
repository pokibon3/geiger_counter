//  GeigerCounter for Analog Meter
//  2024.02.25 new by K.Ohe
//
#include <Arduino.h>
#define TIMES        60
#define MODE_X1      51
#define MODE_X5     255
#define MODE_X10    510
#define CALIBRATE   215

volatile int interruptCounter = 0;

const int16_t interruptPin = 21;
const int16_t dacPin = 25;  // DAC出力用のピン指定
const int16_t selX10 = 14;  
const int16_t selX5  = 13;
const int16_t selX1  = 12;
int16_t     maxCPM   = MODE_X10;

int counts[TIMES]   = {0};  // 直近5秒間のカウントを保持する配列
int currentIndex    = 0;    // 現在のインデックス
unsigned long lastTime = 0;

void IRAM_ATTR handleInterrupt() {
    interruptCounter++;
}

int16_t calculateMovingAverage() {
    long sum = 0;
    for (int i = 0; i < TIMES; i++) {
        sum += counts[i];
    }
    return sum * 60 / TIMES * 2;
}

void setup()
{
    Serial.begin(115200);
    pinMode(selX10, INPUT_PULLUP);
    pinMode(selX5,  INPUT_PULLUP);
    pinMode(selX1,  INPUT_PULLUP);
    pinMode(interruptPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING);
    pinMode(dacPin, OUTPUT);
}

void testWrite(uint16_t cpm)
{
    double value = log10(cpm) * 100.0;
    int dacValue = map(int(value), 0, 420, 0, CALIBRATE);
    dacWrite(dacPin, dacValue);
}

#if 0 
void loop()
{

    testWrite(1);
    delay(1000);    
    testWrite(10);
    delay(1000);    
    testWrite(100);
    delay(1000);    
    testWrite(1000);
    delay(1000);    
    testWrite(10000);
    delay(1000);    
}
#endif

#if 1
void loop() {
    unsigned long currentTime = millis();

    if (currentTime - lastTime >= 1000 / 2) { // 1秒ごとに実行
        lastTime = currentTime;

        // 現在のカウントを配列に保存し、カウンタをリセット
        counts[currentIndex] = interruptCounter;
        interruptCounter = 0;

        // 移動平均を計算
        int16_t cpm = calculateMovingAverage();
/*
        // DAC値に変換して出力
        if          (digitalRead(selX1) == LOW) {      //  X1 mode
            maxCPM = MODE_X1;
        } else if   (digitalRead(selX5) == LOW) {      //  X5 mode
            maxCPM = MODE_X5;
        } else {                                        // X10 mode
            maxCPM = MODE_X10;
        }
        cpm = min(cpm, maxCPM);
*/
        double value = log10(cpm) * 100.0;

        int dacValue = map(int(value), 0, 420, 0, CALIBRATE);
        dacWrite(dacPin, dacValue);
        Serial.printf("cpm = %d, dacValue = %d\n", cpm, dacValue);

        // インデックスを更新
        currentIndex = (currentIndex + 1) % TIMES;
    }
}
#endif


