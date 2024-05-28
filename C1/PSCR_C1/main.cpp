#include <iostream>
#include <iomanip>
#include <fstream>
/*Czêœæ programu wykonanego w ramach projektu na przedmiot Programowanie Sytemów Czasu Rzeczywistego
Autor: Bart³omiej Radecki*/

#include <curl\curl.h>
#include <json/json.h>
#include "ApiCall.h"
#include <semaphore.h>
#include <thread>
#include <mosquitto.h>

sem_t semaphore;
std::string message;

struct mosquitto* mosq = mosquitto_new(nullptr, true, nullptr); 
Json::Value weather;

void createMessage() {
    sem_wait(&semaphore); // Wait (P) operation
    message = weather_call();
    sem_post(&semaphore); // Signal (V) operation
}

void sendMessage() {
    sem_wait(&semaphore); // Wait (P) operation
    send(mosq, message);
    sem_post(&semaphore); // Signal (V) operation
}



int main() {
    sem_init(&semaphore, 0, 1);

    mosquitto_lib_init();
    mosq_pub(mosq);

    std::thread t1(createMessage);
    std::thread t2(sendMessage);

    while (true)
    {
        t1.join();
        t2.join();

        std::this_thread::sleep_for(std::chrono::minutes(10));

        t1 = std::thread(createMessage);
        t2 = std::thread(sendMessage);
    }

    mosquitto_end(mosq);
    sem_destroy(&semaphore);

    return 0;
}