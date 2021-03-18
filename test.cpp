// A collection of useful code snippets that are often shared between projects.
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <functional>
#include <stdexcept>

#include "router.h"
#include "network.h"

////////////////////////////////////////////////////////////////////////////////
// Barebones unit testing "framework".
////////////////////////////////////////////////////////////////////////////////
#define STRING(x) #x
#define REQUIRE(statement) \
  if (!(statement)) { \
    printf("FAILED: assertion: (%s), test: %s, file: %s, line: %d.\n", \
      STRING(statement), __PRETTY_FUNCTION__, __FILE__, __LINE__); return 0; }
#define PASSED_TEST() printf("PASSED: %s\n", __func__); return 1;
////////////////////////////////////////////////////////////////////////////////

int test_create_router() {
  std::vector<row> network_rows = {
    {"S", 42.710356, -73.819109, 131.0},
    {"S", 42.710356, -73.819109, 131.0},
  };
  try {
    Router routing_engine();
  } catch(const std::exception& e) {
    REQUIRE(false);
  }

  PASSED_TEST();
}

int main_test_runner() {
  int total = 0;
  int passed = 0;

  std::vector<std::function<int()> > all_tests({
    test_factory_constructor,
  });

  for (auto test_fxn : all_tests) {
    total++;
    passed += test_fxn();
  }

  std::cout << "\nPASSED " << passed << " of " << total << " TESTS" << std::endl;
}
