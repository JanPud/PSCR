#pragma once
#include <iostream>
#include <iomanip>
#include <fstream>
#include <curl\curl.h>
#include <json/json.h>
#include <mosquitto.h>
#include <string>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* buffer);
std::string weather_call();
int mosq_pub(struct mosquitto* mosq);
int send(struct mosquitto* mosq, std::string data);
void mosquitto_end(struct mosquitto* mosq);
std::string weather_call2();
