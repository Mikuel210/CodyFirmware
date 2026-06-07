#pragma once
#include "Fusion.h"

float Fusion::deltat;
SF Fusion::fusion;

SensorData Fusion::previousSensorData;
FusionData Fusion::previousFusionData;

ColorData Fusion::colorValues[5] = {
    {0, 0, 0},       // Black
    {255, 255, 255}, // White
    {35, 90, 170},   // Blue
    {26, 120, 66},   // Green
    {249, 239, 61}   // Yellow
};
