#include <iostream>
#include <mosquitto.h>
#include <cstring>
#include <vector>
#include <mutex>

const char* C1_ADDRESS = "localhost";
const char* C4_ADDRESS = "localhost";
const int PORT = 1883;

const char* TOPIC_FROM_C1 = "C1";
const char* TOPIC_TO_C4 = "C2";

struct mosquitto* mosq = NULL;

std::mutex data_mutex;
std::vector<double> temperatures;
std::vector<double> sunshine;
std::vector<double> wind_speed;

void on_message_callback(struct mosquitto* mosq, void* userdata, const struct mosquitto_message* message) {
    if (message->payloadlen) {
        std::lock_guard<std::mutex> lock(data_mutex);
        // Przetwarzanie otrzymanych danych

        char* data = _strdup((char*)message->payload);
        char* next_token; // Deklaracja zmiennej przechowuj¹cej stan
        char* token = strtok_s(data, ",", &next_token);
        std::cout << "Otrzymano wiadomosc: " << data << std::endl;
        if (token != NULL) {
            temperatures.push_back(std::stod(token));
            token = strtok_s(NULL, ",", &next_token);
            if (token != NULL) {
                sunshine.push_back(std::stod(token));
                token = strtok_s(NULL, ",", &next_token);
                if (token != NULL) {
                    wind_speed.push_back(std::stod(token));
                }
            }
        }
        free(data);
    }
}

void calculate_averages() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        std::lock_guard<std::mutex> lock(data_mutex);
        if (!temperatures.empty() && !sunshine.empty() && !wind_speed.empty()) {

            double avg_temperature = 230.0;
            double avg_sunshine = 30.0;
            double avg_wind_speed = 50.0;
            for (double temp : temperatures) {
                avg_temperature += temp;
            }
            avg_temperature /= temperatures.size();
            for (double sun : sunshine) {
                avg_sunshine += sun;
            }
            avg_sunshine /= sunshine.size();
            for (double wind : wind_speed) {
                avg_wind_speed += wind;
            }
            avg_wind_speed /= wind_speed.size();

            std::cout << "Obliczone srednie wartosci: " << avg_temperature << " (temp), " << avg_sunshine << " (sun), " << avg_wind_speed << " (wind)" << std::endl;
            // Wysy³anie danych do C4
            std::string data = std::to_string(avg_temperature) + "," + std::to_string(avg_sunshine) + "," + std::to_string(avg_wind_speed);
            mosquitto_publish(mosq, NULL, TOPIC_TO_C4, data.size(), data.c_str(), 0, false);

            // Czyszczenie buforów danych
            temperatures.clear();
            sunshine.clear();
            wind_speed.clear();
        }
    }
}

void on_connect_callback(struct mosquitto* mosq, void* userdata, int rc) {
    if (rc == 0) {
        std::cout << "Polaczono z serwerem MQTT" << std::endl;
        mosquitto_subscribe(mosq, NULL, TOPIC_FROM_C1, 0);
    }
    else {
        std::cerr << "Blad polaczenia z serwerem MQTT: " << mosquitto_connack_string(rc) << std::endl;
    }
}

int main() {
    mosquitto_lib_init();

    mosq = mosquitto_new(NULL, true, NULL);
    mosquitto_message_callback_set(mosq, on_message_callback);
    mosquitto_connect_callback_set(mosq, on_connect_callback);
    //mosquitto_username_pw_set(mosq, "Joe", "mama");
    if (mosquitto_connect(mosq, C1_ADDRESS, PORT, 60) != MOSQ_ERR_SUCCESS) {
        std::cerr << "Blad polaczenia z serwerem C1" << std::endl;
        return 1;
    }

    mosquitto_loop_start(mosq);

    std::thread averages_thread(calculate_averages);

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    averages_thread.join();

    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}