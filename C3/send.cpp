#include <iostream>
#include <mosquitto.h>
#include <string>
//#include <chrono>
//#include <thread>

int mosq_pub(struct mosquitto* mosq) {

    if (!mosq) {
        std::cerr << "Error creating mosquitto instance." << std::endl;
        return 1;
    }

    mosquitto_username_pw_set(mosq, "Joe", "mama");
    mosquitto_connect(mosq, "192.168.137.10", 1883, 60);


    return 0;
}

int send(struct mosquitto* mosq,std::string data) {
    //while (true) {
    // Generate your environment data (e.g., temperature, pressure, humidity)
    const char* payload = data.c_str();
    mosquitto_publish(mosq, nullptr, "C3", strlen(payload), payload, 0, false);
    // Sleep or perform other tasks
//}
    //std::this_thread::sleep_for(std::chrono::milliseconds(10));

    return 0;
}

void mosquitto_end(struct mosquitto* mosq) {
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
}

//void process_message(const char* payload) {
//    // Process the received payload (e.g., print it)
//    std::cout << "Received: " << payload << std::endl;
//}
//
//void message_callback(struct mosquitto* mosq, void* userdata, const struct mosquitto_message* message) {
//    if (message->payloadlen > 0) {
//        process_message(static_cast<const char*>(message->payload));
//    }
//}
//
//int send() {
//	mosquitto_lib_init();
//	struct mosquitto* mosq = mosquitto_new(nullptr, true, nullptr);
//    if (!mosq) {
//        std::cerr << "Error creating mosquitto instance." << std::endl;
//        return 1;
//    }
//
//    mosquitto_connect(mosq, "localhost", 1883, 60);
//    mosquitto_subscribe(mosq, nullptr, "home/+/ambient_data", 0);
//    mosquitto_message_callback_set(mosq, message_callback);
//
//    mosquitto_loop_start(mosq); // Start the MQTT loop
//
//    // Keep the program running
//    std::cout << "Press Enter to exit..." << std::endl;
//    std::cin.get();
//
//    mosquitto_disconnect(mosq);
//    mosquitto_destroy(mosq);
//    mosquitto_lib_cleanup();
//	return 0;
//}