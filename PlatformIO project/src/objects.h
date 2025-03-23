#ifndef OBJECTS_H
#define OBJECTS_H

#include <parameters.h>
#include <bot_faces.h>

#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//=================

//SCREEN
#define i2c_Address 0x27 //initialize with the I2C addr 0x3C Typically eBay OLED's
#define SCREEN_WIDTH 20 
#define SCREEN_HEIGHT 4 

// Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
LiquidCrystal_I2C display(i2c_Address, SCREEN_WIDTH, SCREEN_HEIGHT);

//==========================================================

struct PassiveBuzzer{
    int pin = 0;

    PassiveBuzzer(){}

    void init(int pin, int channel){
        this->pin = pin;
        pinMode(pin, OUTPUT);    
    }

    void beep(unsigned int frec, unsigned int dur){
        tone(pin, frec);
        delay(dur);
        noTone(pin);
    }


    void startupBeep(){
        beep(700, 100);
        beep(900, 100);
    }

    void actionBeep(){
        beep(700, 100);
    }

    void alarmBeep(){
        beep(1000, 200);
        delay(100);
        beep(800, 300);
    }

    void successBeep(){
        beep(700, 100);
        delay(50);
        beep(1000, 100);
        delay(50);
        beep(1300, 100);
    }

    void gamblingBeep(){
        beep(700, 100);
        delay(50);
        beep(1000, 100);
        delay(200);
    }

    void sadBeep(){
        beep(1300, 100); 
        delay(50);
        beep(1000, 100); 
        delay(50);  
        beep(700, 100);
        delay(50);
        beep(500, 200);
    }

    void celebrationBeep(){
        beep(1000, 200);
        delay(300);
        beep(800, 300);
        delay(300);
        beep(600, 300);    
    }

    void angryBeep(){
        beep(600, 100);
        delay(50);
        beep(800, 100);
        delay(200);  
    }

};


struct ActiveBuzzer{
    int pin = 0;
    bool active = false;
    bool enabled = true;

    unsigned long ALARM_DELAY = 2000;
    unsigned long time_now=0;
    unsigned long last_alarm=0;

    void _update(){
        if(!enabled){
            digitalWrite(pin, LOW);
            return;
        }

        if(active)
            digitalWrite(pin, HIGH);
        else
            digitalWrite(pin, LOW);
    }

    ActiveBuzzer(){}

    void init(int pin, unsigned long alarm_delay=1000){
        this->pin = pin;
        ALARM_DELAY = alarm_delay;
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }

    void beep(unsigned int dur){
        digitalWrite(pin, HIGH);
        delay(dur);
        digitalWrite(pin, LOW);
    }

    void alarm_mode(){
        time_now = get_time();

        if(time_now-last_alarm <= ALARM_DELAY)
            return;
        
        last_alarm = time_now;
        active = !active;
        _update();
    }

    void enable(){
        enabled = true;
        _update();
    }

    void disarm(){
        enabled = false;
        _update();
    }

};


struct SwitchButton{
    int swpin, swState;
    PassiveBuzzer* spk;

    //Debounce
    unsigned long debounce = 50;
    unsigned long last_check = 0;
    unsigned long time_now = 0;
    bool last_state = HIGH;

    SwitchButton(){}


    void init(int sw, PassiveBuzzer &spk){
        swpin = sw;
        this->spk = &spk;

        pinMode(sw, INPUT_PULLUP);
        last_state = digitalRead(sw);
        last_check = millis();
    }

    //Is switch pressed?
    bool isPressed(){
        time_now = get_time();

        if(time_now-last_check <= debounce)
            return false;

        last_check = time_now;
        swState = digitalRead(swpin);

        if(swState == LOW && last_state == HIGH){
            spk->actionBeep();
            last_state = swState;
            return true;
        }

        last_state = swState;
        return false;
    }
};


struct LCDScreen{
    int centerX = 0;
    int centerY = 0;

    int cursorX = 0;
    int cursorY = 0;
    PassiveBuzzer* spk;

    LCDScreen(){}

    void init(PassiveBuzzer &spk){
        centerX = (SCREEN_WIDTH / 2) - 1;
        centerY = (SCREEN_HEIGHT / 2) - 1;
        this->spk = &spk;

        display.init();  
        display.clear();     
        display.backlight();
        display.display();
        delay(2000);
    }


    void off_mode(){
        display.noBacklight();
        display.clear();
        display.display();
    }

    void show(){
        display.backlight();
        display.display();
    }


    void clear(){
        display.clear();
    }


    void moveCursor(int x, int y){
        cursorX = x;
        cursorY = y;
        display.setCursor(x, y);
    }

    void header(String title){
        printCentered(title, 0);
    }

    void loading_screen(){
        for(int i=0; i<=100; i+=20){
            clear();
            printCenteredTextNumber("Cargando...", String(i) + "%");
            show();

            delay(800);
        }

        clear();
        printCentered("L.C.Y");
        show();
        spk->startupBeep();
    }


    void print(String message){
        if(cursorX + message.length() >= SCREEN_WIDTH)
            cursorY++;
        cursorX += message.length();
        display.print(message);
        moveCursor(cursorX, cursorY);
    }

    
    void printCentered(String message, int row=1){
        //Calculate center X
        int center = centerX - (message.length()/2);

        moveCursor(center, row);
        print(message);
    }


    void printCenteredTextNumber(String text, String number){
        clear();
        header(text);
        printCentered(number, 2);
        moveCursor(0, 3);
    }


    void printMessage(String from, String message){
        clear();
        header(from + ":");
        moveCursor(0, 2);
        print(message);
        show();
    }

};


struct GasSensor{
    int pin = 0;
    int THRESHOLD = 1000;
    float MAX_VOLTAGE = 3.3;
    float max_input = 4095;
    int gas_level = 0;
    int last_level = 0;
    unsigned long first_init_time = 0;
    bool warming_up = true;
    #define WARM_UP_TIME 120000


    void _update(){
        gas_level = analogRead(pin);
        gas_level = gas_level*0.8 + last_level*0.2;
        last_level = gas_level;
    }

    GasSensor(){}

    void init(int pin, int threshold=1000, float max_voltage=3.3, float max_input=4095){
        this->pin = pin;
        MAX_VOLTAGE = max_voltage;
        this->max_input = max_input;
        THRESHOLD = threshold;
        first_init_time = get_time();
        pinMode(pin, INPUT);
        _update();
    }

    int get_gas_level(){
        _update();
        return gas_level;
    }

    bool dangerous(){
        _update();

        if(warming_up){
            unsigned long time_now = get_time();
            if(time_now-first_init_time > WARM_UP_TIME)
                warming_up = false;
            return false;
        }

        return gas_level > THRESHOLD;
    }

    bool warning(){
        _update();
        return gas_level > THRESHOLD-100;
    }

};


#endif


