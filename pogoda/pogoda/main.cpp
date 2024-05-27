#include <iostream>
#include <cstring>
#include <mosquitto.h>
#include <thread>
#include <mutex>
#include "nlohmann/json.hpp"


using json = nlohmann::json;

std::mutex data_mutex;
json weather_data_buffer = json::array();

void calculate_and_send_averages(struct mosquitto* mosq_c4) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10));

        std::lock_guard<std::mutex> lock(data_mutex);

        if (!weather_data_buffer.empty()) {
            try {

                double cloudsSum = 0, temperatureSum = 0, windSum = 0;

                for (const auto& weatherData : weather_data_buffer) {
                    cloudsSum += weatherData["clouds"];
                    temperatureSum += weatherData["temperature"];
                    windSum += weatherData["wind"];
                }

                double numEntries = static_cast<double>(weather_data_buffer.size());
                double avgClouds = cloudsSum / numEntries;
                double avgTemperature = temperatureSum / numEntries;
                double avgWind = windSum / numEntries;

                std::cout << "Srednia chmur: " << avgClouds << std::endl;
                std::cout << "Srednia temperatura: " << avgTemperature << std::endl;
                std::cout << "Srednia wiatr: " << avgWind << std::endl;

                std::string message = std::to_string(avgTemperature) + '\n' + std::to_string(avgClouds) + '\n' + std::to_string(avgWind);

                mosquitto_publish(mosq_c4, nullptr, "C2", message.size(), message.c_str(), 0, false);
            }
            catch (const std::exception& e) {
                std::cerr << "B³¹d podczas przetwarzania danych JSON: " << e.what() << std::endl;
            }


            weather_data_buffer.clear();
        }
    }
}

void message_callback(struct mosquitto* mosq, void* userdata, const struct mosquitto_message* message) {
    if (message->payloadlen) {
        try {

            json jsonData = json::parse(static_cast<const char*>(message->payload));


            if (jsonData.is_array()) {
                std::lock_guard<std::mutex> lock(data_mutex);
                for (const auto& data : jsonData) {
                    weather_data_buffer.push_back(data);
                }
            }
            else {
                std::cerr << "Otrzymane dane nie s¹ tablic¹ JSON." << std::endl;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "B³¹d podczas przetwarzania danych JSON: " << e.what() << std::endl;
        }
    }
}

int main() {

    mosquitto_lib_init();

    struct mosquitto* mosq_c1 = mosquitto_new(nullptr, true, nullptr);
    if (!mosq_c1) {
        std::cerr << "B³¹d inicjalizacji klienta MQTT (C1)." << std::endl;
        return 1;
    }

    int rc_c1 = mosquitto_connect(mosq_c1, "192.168.191.100", 1883, 1200);
    if (rc_c1) {
        std::cerr << "B³¹d po³¹czenia z brokerem MQTT (C1)." << std::endl;
        return 1;
    }

    struct mosquitto* mosq_c4 = mosquitto_new(nullptr, true, nullptr);
    if (!mosq_c4) {
        std::cerr << "B³¹d inicjalizacji klienta MQTT (C4)." << std::endl;
        return 1;
    }

    int rc_c4 = mosquitto_connect(mosq_c4, "192.168.191.100", 1883, 1200);
    if (rc_c4) {
        std::cerr << "B³¹d po³¹czenia z brokerem MQTT (C4)." << std::endl;
        return 1;
    }

    mosquitto_subscribe(mosq_c1, nullptr, "C1", 0);
    mosquitto_message_callback_set(mosq_c1, message_callback);


    std::thread calculate_thread(calculate_and_send_averages, mosq_c4);

    while (true) {
        mosquitto_loop(mosq_c1, -1, 1);
    }

    calculate_thread.join();
    mosquitto_destroy(mosq_c1);
    mosquitto_destroy(mosq_c4);
    mosquitto_lib_cleanup();

    return 0;
}
