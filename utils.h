#pragma once
#include <math.h>

// Distance types
using Kilometers = double;
using Milliseconds = uint64_t;
using KmPerHr = double;

// Graph types
using NodeID = uint16_t;
using Weight = Milliseconds;

// Constants from the spec
const double EARTH_RADIUS_KM = 6356.752;
const double ROAD_SPEED_KM_HR = 105;
const Kilometers MAX_CHARGE = 320;

const int MS_IN_SEC = 1000;
const int MS_IN_MINUTE = 60 * MS_IN_SEC;
const int MS_IN_HOUR = 60 * MS_IN_MINUTE;

inline double degree_to_radian(double angle) { return angle * M_PI / 180.0; }

// Return the great-circle distance between two points in KM.
inline Kilometers haversine_dist(double lat1, double lng1, double lat2, double lng2) {
  double lat_rad1 = degree_to_radian(lat1);
  double lng_rad1 = degree_to_radian(lng1);
  double lat_rad2 = degree_to_radian(lat2);
  double lng_rad2 = degree_to_radian(lng2);

  double diff_lat = lat_rad2 - lat_rad1;
  double diff_lng = lng_rad2 - lng_rad1;

  double u = sin(diff_lat / 2.0);
  double v = sin(diff_lng / 2.0);

  double computation = asin(sqrt(u * u + cos(lat_rad1) * cos(lat_rad2) * v * v));

  return 2.0 * EARTH_RADIUS_KM * computation;
}

inline Milliseconds convert_km_to_ms_travel(Kilometers distance_km) {
  return int(((distance_km / ROAD_SPEED_KM_HR) * MS_IN_HOUR) + 0.5);
}

inline Milliseconds time_to_partial_charge(Kilometers state_of_charge, Kilometers desired_charge,
                                           KmPerHr rate) {
  return int((((desired_charge - state_of_charge) / rate) * MS_IN_HOUR) + 0.5);
}

inline Milliseconds time_to_full_charge(Kilometers state_of_charge, KmPerHr rate) {
  return time_to_partial_charge(state_of_charge, MAX_CHARGE, rate);
}

inline double ms_to_hours(Milliseconds ms) { return double(ms) / double(MS_IN_HOUR); }
