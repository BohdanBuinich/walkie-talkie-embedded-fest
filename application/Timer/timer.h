#pragma once

#include <string>

#include "esp_timer.h"

unsigned long timeStampOffset = 0;
unsigned long requestTimestamp;

static unsigned long millis() {
    return (unsigned long) (esp_timer_get_time() / 1000ULL);
}

std::string getStrTimestamp() {
    requestTimestamp = millis() + timeStampOffset;
    return std::to_string(requestTimestamp);
}