#include <iostream>
#include <queue>
#include <sstream>
#include <map>
#include "mysql.h"
#include "mosquitto.h"
#include <chrono>
#include <thread>

// Adres IP brokera MQTT
const char* mqttBrokerAddress = "192.168.191.100"; // Zast¹p "adres_IP" adresem IP brokera MQTT

// Deklaracja globalnych kolejek FIFO do przechowywania otrzymanych wiadomoœci
std::queue<std::string> messageQueueC2;
std::queue<std::string> messageQueueC3;

// Funkcja obs³uguj¹ca po³¹czenie z brokerem MQTT
void onConnect(struct mosquitto* mosq, void* userdata, int result) {
    if (result == MOSQ_ERR_SUCCESS) {
        std::cout << "Polaczono z brokerem MQTT." << std::endl;
        // Subskrypcja wiadomoœci na temacie
        mosquitto_subscribe(mosq, NULL, "C2", 0);
        mosquitto_subscribe(mosq, NULL, "C3", 0);
    }
    else {
        std::cerr << "Nie uda³o siê pol¹czyæ z brokerem MQTT. Kod b³edu: " << result << std::endl;
    }
}

// Funkcja obs³uguj¹ca otrzymane wiadomoœci
void onMessage(struct mosquitto* mosq, void* userdata, const struct mosquitto_message* message) {
    if (message->payloadlen) {
        std::cout << "Otrzymano wiadomosc na temacie " << message->topic << ": " << (char*)message->payload << std::endl;
        std::cout << std::endl;

        // Przetwarzanie wiadomoœci w formacie danych oddzielonych now¹ lini¹
        std::string payload = (char*)message->payload;

        // Umieszczenie danych w odpowiedniej kolejce FIFO
        if (std::string(message->topic) == "C2") {
            messageQueueC2.push(payload);
            std::cout << "Obecny stan kolejki C2: " << messageQueueC2.size() << " wiadomosci" << std::endl;
            std::cout << std::endl;
        }
        else if (std::string(message->topic) == "C3") {
            messageQueueC3.push(payload);
            std::cout << "Obecny stan kolejki C3: " << messageQueueC3.size() << " wiadomosci" << std::endl;
            std::cout << std::endl;
        }
        else {
            std::cerr << "Nieznany temat: " << message->topic << std::endl;
        }
    }
    else {
        std::cout << "Otrzymano pusta wiadomosc na temacie " << message->topic << std::endl;
    }
}

// Funkcja do wysy³ania danych z kolejki do tabeli w bazie danych MySQL
void sendToMySQL(MYSQL* connection, std::queue<std::string>& messageQueue, const std::string& tableName, const std::vector<std::string>& columns) {
    if (!messageQueue.empty()) {
        // Pobranie pierwszego elementu z kolejki
        std::string message = messageQueue.front();
        messageQueue.pop(); // Usuniêcie elementu z kolejki

        // Parsowanie otrzymanej wiadomoœci
        std::istringstream iss(message);
        std::vector<std::string> values;
        std::string value;
        while (std::getline(iss, value)) {
            values.push_back(value);
        }

        // Sprawdzenie, czy liczba przekazanych wartoœci jest zgodna z oczekiwan¹ liczb¹ kolumn w tabeli
        if (values.size() != columns.size()) {
            std::cerr << "Bl¹d: Nieprawidlowa liczba przekazanych wartoœci." << std::endl;
            return;
        }

        // Budowanie zapytania SQL
        std::string query = "INSERT INTO " + tableName + " (";
        for (size_t i = 0; i < columns.size(); ++i) {
            query += columns[i];
            if (i != columns.size() - 1) {
                query += ", ";
            }
        }
        query += ") VALUES (";
        for (size_t i = 0; i < values.size(); ++i) {
            query += "'" + values[i] + "'";
            if (i != values.size() - 1) {
                query += ", ";
            }
        }
        query += ")";

        // Wykonanie zapytania SQL
        if (mysql_query(connection, query.c_str())) {
            std::cerr << "Bl¹d przy wysylaniu danych do MySQL: " << mysql_error(connection) << std::endl;
        }
        else {
            std::cout << "Dane zostaly pomyslnie wyslane do MySQL." << std::endl;
        }
    }
}

int main() {
    // Inicjalizacja biblioteki Mosquitto
    mosquitto_lib_init();

    // Utworzenie klienta MQTT
    struct mosquitto* mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq) {
        std::cerr << "Nie uda³o siê utworzyæ klienta MQTT." << std::endl;
        return 1;
    }

    // Konfiguracja klienta MQTT
    mosquitto_connect_callback_set(mosq, onConnect);
    mosquitto_message_callback_set(mosq, onMessage); // Dodana funkcja obs³ugi otrzymanych wiadomoœci

    // Po³¹czenie z brokerem MQTT
    int rc = mosquitto_connect_async(mosq, mqttBrokerAddress, 1883, 60);
    if (rc != MOSQ_ERR_SUCCESS) {
        std::cerr << "B³¹d po³¹czenia z brokerem MQTT. Kod b³êdu: " << rc << std::endl;
        return 1;
    }

    // Po³¹czenie z baz¹ danych MySQL
    MYSQL* connection = mysql_init(NULL);
    if (connection == NULL) {
        std::cerr << "B³¹d inicjalizacji po³¹czenia z baz¹ danych MySQL." << std::endl;
        return 1;
    }
    if (mysql_real_connect(connection, "localhost", "root", "aksamitka", "moja_baza", 0, NULL, 0) == NULL) {
        std::cerr << "B³¹d po³¹czenia z baz¹ danych MySQL: " << mysql_error(connection) << std::endl;
        return 1;
    }

    // Pêtla obs³ugi klienta MQTT
    mosquitto_loop_start(mosq);

    // Pêtla do wysy³ania danych z kolejki do MySQL co minutê
    while (true) {
        // Wys³anie danych z kolejki C2 do tabeli C2 w MySQL
        sendToMySQL(connection, messageQueueC2, "C2", { "Sr_temperatura", "Sr_zachmurzenie", "Sr_wiatr" });

        // Wys³anie danych z kolejki C3 do tabeli C3 w MySQL
        sendToMySQL(connection, messageQueueC3, "C3", { "Zapotrzebowanie", "Generacja", "el_cieplne", "el_wodne", "el_wiatrowe", "el_fotowoltaiczne", "el_inne_odnawialne", "Saldo_wymiany_calkowitej", "Czestotliwosc", "Szwecja", "Niemcy", "Czechy", "Slowacja", "Ukraina", "Litwa" });

        // Oczekiwanie 60 sekund
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    // Zwolnienie zasobów i zakoñczenie programu
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}
