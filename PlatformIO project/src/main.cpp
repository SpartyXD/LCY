#include <WiFi.h>
#include <objects.h>
#include <chatbot.h>

//=====================================
//WIFI
#define WIFI_SSID "Your wifi name"
#define WIFI_PASSWORD "Your wifi password"

#define DEBG_MODE false

//===================================
//OBJECTS
LCDScreen screen;
GasSensor sensor;
PassiveBuzzer mini_spk;
ActiveBuzzer alarm_spk;
SwitchButton btn;
ChatBot chatbot;

//===================================
//GLOBALS

// 0-Idle | 1-Message | 2-Gas
int CURRENT_MENU = 0;
String CURRENT_MESSAGE = "";

//===================================

//Functions

void handleMessages(){
    String message = chatbot.receiveMessage();
    chatbot.sendMessage(chatbot.chat_id, "Mensaje recibido 🤖");
    CURRENT_MESSAGE = message;

    //Change menu
    CURRENT_MENU = 1;
}

//===================================

//Menus

struct IdleMode{
    unsigned long idleDelay = 60000; //1 minute
    unsigned long last_change = get_time();
    int CURRENT_FACE = 0;
    bool first_time = true;

    IdleMode(){}

    void run(){
        //Check for button press
        if(btn.isPressed()){
            CURRENT_MENU = 2; //gas
            first_time = true;
            return;
        }


        screen.off_mode();
    }

};


struct MessageMode{
    //Notification
    unsigned long unreadDelay = 60000; //60 seconds
    unsigned long last_notification = get_time();
    unsigned long time_now = get_time();

    MessageMode(){}

    void run(){
        //Check button press
        if(btn.isPressed()){
            CURRENT_MENU = 0;
            mini_spk.successBeep();
            chatbot.sendMessage(chatbot.chat_id, "Mensaje leido 😉");
            last_notification = 0;
            return;
        }

        //Is a command?
        if(CURRENT_MESSAGE[0] == '/'){
            chatbot.sendMessage(chatbot.chat_id, "Comando recibido ✅");
            handleCommand(CURRENT_MESSAGE.substring(1));
            return;
        }

        //Handle notification
        time_now = get_time();
        if(time_now-last_notification > unreadDelay){
            last_notification = time_now;
            mini_spk.alarmBeep();
        }

        screen.printMessage(chatbot.last_message_name, CURRENT_MESSAGE);  
    }


    void handleCommand(String command){
        if(command.substring(0, 8) == "register"){
            chatbot.updateSenderID(command.substring(9), chatbot.chat_id);
            chatbot.sendMessage(chatbot.chat_id, "Usuario registrado: " + command.substring(9));
        }

        CURRENT_MENU = 0;
    }

};


struct GasMode{
    bool snoozed = false;
    bool GAS_LEAK = false;

    unsigned long last_snooze = get_time();
    unsigned long SNOOZE_TIME = 120000; //2 minutes
    unsigned long time_now = get_time();

    int gas_level = 0;
    
    GasMode(){}

    void showGasInfo(String message="Nivel de gas:"){
        //Get info
        gas_level = sensor.get_gas_level();

        //Show
        screen.clear();
        screen.printCenteredTextNumber(message, String(gas_level) + " ppm");
        
        if(sensor.warning())
            screen.printCentered("(cuidado)", 3);

        screen.show();
    }


    void run(){
        //Check if buttonPressed
        if(btn.isPressed()){
            CURRENT_MENU = 0;
            return;
        }

        //Get status
        showGasInfo();
    }


    void alarmMode(){
        //Anti bugs
        if(!snoozed)
            alarm_spk.enable();

        //Check if snoozed
        if(snoozed){
            //Get out of snooze
            time_now = get_time();
            if(time_now-last_snooze > SNOOZE_TIME){
                snoozed = false;
                alarm_spk.enable();
            }

            showGasInfo("CUIDADO CON GAS:");
            return;
        }

        //Alert user
        showGasInfo("CUIDADO CON GAS:");


        //Snooze button
        if(btn.isPressed()){
            snoozed = true;
            alarm_spk.disarm();
            last_snooze = get_time();

            screen.clear();
            screen.printCenteredTextNumber("Silenciado:", String(int(SNOOZE_TIME/60000)) + " minutos");
            screen.show();
            delay(2000);
        }
    }


};

IdleMode idleScreen;
MessageMode messageScreen;
GasMode gasScreen;

//===================================

void setup(){
    if(DEBG_MODE)
        Serial.begin(115200);

    mini_spk.init(SPK_1, 0);
    alarm_spk.init(SPK_2);

    sensor.init(GAS, 950); //Set this number to your calibrated threshold

    screen.init(mini_spk);
    btn.init(SWPIN, mini_spk);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    chatbot.init();

    screen.loading_screen();
    delay(1000);
}


unsigned long messageCheckDelay = 5000; //5 seconds
unsigned long time_now = get_time();
unsigned long last_check = get_time();

void loop(){
    //Check gas sensor
    if(sensor.dangerous()){
        gasScreen.alarmMode();
        alarm_spk.alarm_mode();
        delay(100);
        return;
    }
    else{
        alarm_spk.disarm();
    }


    //Check for messages
    time_now = get_time();
    if(time_now-last_check > messageCheckDelay){
        last_check = time_now;
        chatbot.update();
    }

    //Handle incoming mesages
    if(chatbot.numNewMessages){
        screen.clear();
        screen.printCentered("Nuevos mensajes...");
        screen.show();

        delay(800);
        handleMessages();
    }
    

    //Corresponding menu
    switch (CURRENT_MENU){
      case 0:
        idleScreen.run();
        break;

      case 1:
        messageScreen.run();
        break;

      case 2:
        gasScreen.run();
        break; 
        
      default:
        break;
    }

    delay(100);
}



//===================================
