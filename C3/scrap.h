#pragma once
#include <iostream>
#include <curl/curl.h>
#include <string>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);
std::string scrap_message();