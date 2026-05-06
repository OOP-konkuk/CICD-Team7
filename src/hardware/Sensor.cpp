#include "hardware/Sensor.h"

Sensor::Sensor() : isDetected(false) {}

bool Sensor::requestStatus() { return isDetected; }

bool Sensor::getIsDetected() { return isDetected; }
