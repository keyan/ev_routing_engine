#pragma once
#include <iostream>
#include <string>
#include <vector>

#include "utils.h"

struct Station {
  std::string name;
  double lat;
  double lon;
  KmPerHr rate;
};

extern std::vector<Station> network;
