#pragma once
#include "Navigation.h"

NavigationTarget Navigation::drive = NavigationTarget();
NavigationTarget Navigation::toolhead = NavigationTarget();
NavigationTarget Navigation::wheels = NavigationTarget();
NavigationTarget Navigation::mill = NavigationTarget();

PID Navigation::orientationPid = PID(3.5, 0, 0);
PID Navigation::distancePid = PID(0.5, 0, 0);
PID Navigation::xAxisPid = PID(3, 0, 0);
PID Navigation::zAxisPid = PID(3, 0, 0);
PID Navigation::wheelsPid = PID(3, 0, 0);
PID Navigation::millPid = PID(3, 0, 0);
