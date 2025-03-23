#ifndef CHATBOT_H
#define CHATBOT_H

#include <parameters.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Preferences.h>

//=====================================

// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "YOUR TOKEN HERE"
#define BOT_REFRESH_DELAY 1500

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// Data about chats
Preferences data;

struct ChatBot{
    unsigned long bot_lastcheck = 0;
    unsigned long time_now = 0;

    int numNewMessages = 0;
    String last_message_name = "";
    String chat_id = "";

    ChatBot(){}

    void init(){
        configTime(0, 0, "pool.ntp.org"); // Get UTC time via NTP
        client.setCACert(TELEGRAM_CERTIFICATE_ROOT); //Add certificate
        data.begin("LCY");
    }

    void update(){
        time_now = get_time();
        if(time_now - bot_lastcheck <= BOT_REFRESH_DELAY)
            return;

        bot_lastcheck = time_now;
        numNewMessages = bot.getUpdates(bot.last_message_received+1);
    }


    void sendMessage(String chat_id, String message){
        bot.sendMessage(chat_id, message);
    }


    String receiveMessage(){        
        auto msg = bot.messages[0];
        chat_id = msg.chat_id;
        last_message_name = getSenderByID(chat_id);

        return msg.text;
    }


    void updateSenderID(String name, String ID){
        char name_val[name.length()]; 
        name.toCharArray(name_val, name.length()+1);

        char id_val[ID.length()];
        ID.toCharArray(id_val, ID.length());

        data.putString(id_val, name_val);
    }

    String getSenderByID(String ID){
        char id_val[ID.length()];
        ID.toCharArray(id_val, ID.length());

        String name = data.getString(id_val);
        if(name == "")
            return "No registrado";
        else
            return name;
    }

};


#endif