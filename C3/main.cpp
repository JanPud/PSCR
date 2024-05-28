#include <iostream>
#include "scrap.h"
#include "send.h"
#include <semaphore.h>
#include <thread>

sem_t semaphore;
std::string message;

struct mosquitto* mosq = mosquitto_new(nullptr, true, nullptr);

void createMessage() {
    sem_wait(&semaphore); // Wait (P) operation
	message=scrap_message();
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