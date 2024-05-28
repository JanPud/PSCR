#include <iostream>
#include <iomanip>
#include <fstream>
#include <curl\curl.h>
#include <json/json.h>
#include <mosquitto.h>
#include <string>


// Funkcja zwracaj¹ca treœæ odpowiedzi HTTP do bufora
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* buffer) {
    size_t realsize = size * nmemb;
    buffer->append((char*)contents, realsize);
    return realsize;
}


std::string weather_call2() {
    return "adfsefsfsefcvx";
}


std::string weather_call () {
    std::string apiKey = "89d70aa4d8cd0c95949e2acdcf8dd35c";

    std::string lats[12] = { "54.36", "54.09", "53.42", "53.15", "52.48", "52.21", "51.54", "51.27", "51.00", "50.33", "50.06", "49.39" };
    std::string lons[21] = { "23.39", "23.09", "22.39", "22.09", "21.39", "21.09", "20.39", "20.09", "19.39", "19.09", "18.39", "18.09", "17.39", "17.09", "16.39", "16.09", "15.39", "15.09", "14.39", "14.09", "13.39" };

    // URL bazowy OpenWeather API
    std::string baseUrl = "http://api.openweathermap.org/data/2.5/weather?";

    // Tworzenie pustej struktury JSON
    Json::Value data;

    int i = 0;
    // Pêtla pobieraj¹ca informacje o pogodzie dla ka¿dej lokalizacji
    for (const auto& lat : lats) {
        for (const auto& lon : lons) {
            // Tworzenie pe³nego adresu URL dla danej lokalizacji
            std::string url = baseUrl + "lat=" + lat + "&lon=" + lon + "&APPID=" + apiKey + "&units=metric";

            // Inicjalizacja biblioteki libcurl
            CURL* curl = curl_easy_init();
            if (curl) {
                // Ustawienie adresu URL
                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

                // Bufor do przechowywania odpowiedzi HTTP
                std::string buffer;

                // Ustawienie funkcji zwrotnej do zapisywania odpowiedzi HTTP w buforze
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

                // Wykonanie zapytania HTTP
                CURLcode res = curl_easy_perform(curl);
                if (res == CURLE_OK) {
                    // Parsowanie odpowiedzi JSON
                    Json::Value root;
                    Json::Reader reader;
                    bool parsingSuccessful = reader.parse(buffer, root);
                    if (parsingSuccessful) {
                        // Dodawanie informacji o pogodzie do struktury JSON
                        Json::Value weather;
                        weather["lat"] = lat;
                        weather["lon"] = lon;
                        weather["temperature"] = root["main"]["temp"].asFloat();
                        weather["wind"] = root["wind"]["speed"].asFloat();
                        weather["clouds"] = root["clouds"]["all"].asInt();

                        data.append(weather);
                    }
                    else {
                        std::cerr << "Failed to parse JSON response." << std::endl;
                    }
                }
                else {
                    std::cerr << "Failed to fetch data from OpenWeather API. Error code: " << res << std::endl;
                }

                // Zwolnienie zasobów CURL
                curl_easy_cleanup(curl);
            }
            else {
                std::cerr << "Failed to initialize CURL." << std::endl;
            }
            i++;
        }
    }
    std::cout << i << std::endl;

    //// Zapisanie danych do pliku JSON
    //std::ofstream outputFile("weather_data.json");
    //if (outputFile.is_open()) {
    //    outputFile << std::setw(4) << data << std::endl;
    //    outputFile.close();
    //    std::cout << "Data saved to weather_data.json" << std::endl;
    //}
    //else {
    //    std::cerr << "Failed to open weather_data.json for writing." << std::endl;
    //}

    return data.toStyledString();
}



int mosq_pub(struct mosquitto* mosq) {

    if (!mosq) {
        std::cerr << "Error creating mosquitto instance." << std::endl;
        return 1;
    }

    mosquitto_username_pw_set(mosq, "_", "_");
    //mosquitto_connect(mosq, "localhost", 1883, 60);
    mosquitto_connect(mosq, "192.168.191.100", 1883, 1200);

    return 0;
}

int send(struct mosquitto* mosq, std::string data) {
    //while (true) {
    // Generate your environment data (e.g., temperature, pressure, humidity)
    const char* payload = data.c_str();
    mosquitto_publish(mosq, nullptr, "C1", strlen(payload), payload, 0, false);
    // Sleep or perform other tasks
//}
    //std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::cout << "sent :" << payload << std::endl;
    return 0;
}

void mosquitto_end(struct mosquitto* mosq) {
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
}
