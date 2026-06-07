#pragma once
#include "Cody.h"

IDataProvider* Cody::dataProvider = nullptr;
IHardwareProvider* Cody::hardwareProvider = nullptr;
PursuitData Cody::pathData = PursuitData();
PursuitData Cody::toolheadPathData = PursuitData();