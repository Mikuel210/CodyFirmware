#pragma once
#include "Cody.h"

IDataProvider* Cody::dataProvider = nullptr;
IHardwareProvider* Cody::hardwareProvider = nullptr;

Vector3 Cody::pointer = Vector3();
double Cody::orientation = 0;

PursuitData Cody::pathData = PursuitData();
PursuitData Cody::toolheadPathData = PursuitData();
PursuitData Cody::wheelsPathData = PursuitData();
PursuitData Cody::millPathData = PursuitData();