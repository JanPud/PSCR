#include <iostream>
#include <curl/curl.h>
#include <string>

// Callback function for cURL
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

// Function to extract JavaScript variable value using regex
//std::string ExtractJSVariable(const std::string& htmlContent, const std::string& variableName) {
//    std::regex regexObj(variableName + "\\s*=\\s*(.*?);");
//    std::smatch match;
//    if (std::regex_search(htmlContent, match, regexObj)) {
//        return match[1];
//    }
//    return "";
//}

std::string scrap_message() {
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Error initializing cURL." << std::endl;
        std::string empty;
        return empty;
    }
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64)");
    curl_easy_setopt(curl, CURLOPT_URL, "https://www.pse.pl/transmissionMapService");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    std::string message;
    //while (true)
    //{
        std::string htmlContent;

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &htmlContent);
        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "cURL error: " << curl_easy_strerror(res) << std::endl;
            std::string empty;
            return empty;
        }
        else
        {
            // Find the position of the data in the response
            size_t pos = htmlContent.find("\"przesyly\":");
            if (pos != std::string::npos) {
                // Extract the data
                pos = htmlContent.find(" [{", pos);
                size_t end_pos = htmlContent.find("}}}", pos);
                //if (end_pos != std::string::npos) {
                std::string data = htmlContent.substr(pos + 1, end_pos - pos - 1);
                //std::cout << "Data scraped: " << data << std::endl;

                /*message = "Dane: \n";
                message.append("Zapotrzebowanie [MW]: \n");*/
                //message.append("Generacja [MW]: \n");
                //message.append("\tel. cieplne: \n");
                //message.append("\tel. wodne: \n");
                //message.append("\tel. wiatrowe: \n");
                //message.append("\tel. fotowoltaiczne: \n");
                //message.append("\tel. inne odnawialne: \n");
                //message.append("Saldo wymiany calkowitej [MW]: \n");
                //message.append("Czestotliwosc [Hz]: \n");
                //message.append("\n");
                //message.append("Wymiana z innymi krajami: \n");
                //message.append("\tSzwecja: \n");
                //message.append("\tNiemcy: \n");
                //message.append("\tCzechy: \n");
                //message.append("\tSlowacja: \n");
                //message.append("\tUkraina: \n");
                //message.append("\tLitwa: \n");

                pos = htmlContent.find("\"zapotrzebowanie\": ");
                end_pos = htmlContent.find(",", pos);
                data = htmlContent.substr(pos + 19, end_pos - pos - 19);
                //size_t insert_pos = message.find("Zapotrzebowanie [MW]:");
                //message.insert(insert_pos + 22, data);
                message.append(data);
                message.append("\n");

                pos = htmlContent.find("\"generacja\": ");
                end_pos = htmlContent.find(",", pos);
                data = htmlContent.substr(pos + 13, end_pos - pos - 13);
                //insert_pos = message.find("Generacja [MW]:");
                //message.insert(insert_pos + 16, data);
                message.append(data);
                message.append("\n");

                pos = htmlContent.find("\"cieplne\": ");
                end_pos = htmlContent.find("}", pos);
                data = htmlContent.substr(pos + 11, end_pos - pos - 11);
                //insert_pos = message.find("el. cieplne: ");
                //message.insert(insert_pos + 13, data);
                message.append(data);
                message.append("\n");

                pos = htmlContent.find("\"wodne\": ");
                end_pos = htmlContent.find(",", pos);
                data = htmlContent.substr(pos + 9, end_pos - pos - 9);
                //insert_pos = message.find("el. wodne: ");
                //message.insert(insert_pos + 11, data);
                message.append(data);
                message.append("\n");

                pos = htmlContent.find("\"wiatrowe\": ");
                end_pos = htmlContent.find(",", pos);
                data = htmlContent.substr(pos + 12, end_pos - pos - 12);
                //insert_pos = message.find("el. wiatrowe: ");
                //message.insert(insert_pos + 14, data);
                message.append(data);
                message.append("\n");

                pos = htmlContent.find("\"PV\": ");
                end_pos = htmlContent.find(",", pos);
                data = htmlContent.substr(pos + 6, end_pos - pos - 6);
                //insert_pos = message.find("el. fotowoltaiczne: ");
                //message.insert(insert_pos + 20, data);
                message.append(data);
                message.append("\n");

                pos = htmlContent.find("\"inne\": ");
                end_pos = htmlContent.find(",", pos);
                data = htmlContent.substr(pos + 8, end_pos - pos - 8);
                //insert_pos = message.find("el. inne odnawialne: ");
                //message.insert(insert_pos + 21, data);
                message.append(data);
                message.append("\n");

                pos = htmlContent.find("\"czestotliwosc\": ");
                end_pos = htmlContent.find(",", pos);
                data = htmlContent.substr(pos + 17, end_pos - pos - 17);
                //insert_pos = message.find("Czestotliwosc [Hz]: ");
                //message.insert(insert_pos + 20, data);
                message.append(data);
                message.append("\n");

                pos = htmlContent.find("\"SE\"");
                pos = htmlContent.rfind("\"wartosc\": ", pos);
                end_pos = htmlContent.find(",", pos);
                std::string data_se = htmlContent.substr(pos + 11, end_pos - pos - 11);
                //insert_pos = message.find("Szwecja: ");
                //message.insert(insert_pos + 9, data_se);
                message.append(data_se);
                message.append("\n");

                pos = htmlContent.find("\"DE\"");
                pos = htmlContent.rfind("\"wartosc\": ", pos);
                end_pos = htmlContent.find(",", pos);
                std::string data_de = htmlContent.substr(pos + 11, end_pos - pos - 11);
                //insert_pos = message.find("Niemcy: ");
                //message.insert(insert_pos + 8, data_de);
                message.append(data_de);
                message.append("\n");

                pos = htmlContent.find("\"CZ\"");
                pos = htmlContent.rfind("\"wartosc\": ", pos);
                end_pos = htmlContent.find(",", pos);
                std::string data_cz = htmlContent.substr(pos + 11, end_pos - pos - 11);
                //insert_pos = message.find("Czechy: ");
                //message.insert(insert_pos + 8, data_cz);
                message.append(data_cz);
                message.append("\n");

                pos = htmlContent.find("\"SK\"");
                pos = htmlContent.rfind("\"wartosc\": ", pos);
                end_pos = htmlContent.find(",", pos);
                std::string data_sk = htmlContent.substr(pos + 11, end_pos - pos - 11);
                //insert_pos = message.find("Slowacja: ");
                //message.insert(insert_pos + 10, data_sk);
                message.append(data_sk);
                message.append("\n");

                pos = htmlContent.find("\"UA\"");
                pos = htmlContent.rfind("\"wartosc\": ", pos);
                end_pos = htmlContent.find(",", pos);
                std::string data_ua = htmlContent.substr(pos + 11, end_pos - pos - 11);
                //insert_pos = message.find("Ukraina: ");
                //message.insert(insert_pos + 9, data_ua);
                message.append(data_ua);
                message.append("\n");

                pos = htmlContent.find("\"LT\"");
                pos = htmlContent.rfind("\"wartosc\": ", pos);
                end_pos = htmlContent.find(",", pos);
                std::string data_lt = htmlContent.substr(pos + 11, end_pos - pos - 11);
                //insert_pos = message.find("Litwa: ");
                //message.insert(insert_pos + 7, data_lt);
                message.append(data_lt);
                message.append("\n");

                data = std::to_string(std::stoi(data_se) + std::stoi(data_cz) + std::stoi(data_de) + std::stoi(data_sk) + std::stoi(data_ua) + std::stoi(data_lt));
                //insert_pos = message.find("Saldo wymiany calkowitej [MW]: ");
                message.append(data);

                //std::cout << "Wiadomosc do wyslania: \n" << message << std::endl;
                //send(mosq, message);
            //}
            }
        }
    //}
    //std::string variableName = "#zapotrzebowanie-mw.data-to-display.important-data";
    //std::string variableValue = ExtractJSVariable(htmlContent, variableName);
    //if (!variableValue.empty()) {
    //    std::cout << "Value of JavaScript variable " << variableName << ": " << variableValue << std::endl;
    //}
    //else {
    //    std::cerr << "JavaScript variable " << variableName << " not found." << std::endl;
    //}

    //std::cout << "Fetched HTML content:\n" << htmlContent << std::endl;
    curl_easy_cleanup(curl);

    return message;
}