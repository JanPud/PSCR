#include <string>
#include <mosquitto.h>
#pragma once
int mosq_pub(struct mosquitto* mosq);
int send(struct mosquitto* mosq, std::string data);
void mosquitto_end(struct mosquitto* mosq);
