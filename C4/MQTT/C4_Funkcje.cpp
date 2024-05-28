#include "C4_Funkcje.h"


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