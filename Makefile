ALL_BINARIES = $(wildcard *.cpp)
SOURCE_BINARIES = $(filter-out test.cpp, $(ALL_BINARIES))
TEST_BINARIES = $(filter-out main.cpp, $(ALL_BINARIES))

build:
	g++ -std=c++11 -O1 $(SOURCE_BINARIES) -o candidate_solution

# Executes routing a few hundred times and writes a bash script to run results against the checker.
test:
	g++ -std=c++11 -O1 $(TEST_BINARIES) -o write_checker_script
	./write_checker_script
	chmod 777 run_checker.sh
	./run_checker.sh
	rm ./write_checker_script
	rm ./run_checker.sh

format:
	clang-format -i main.cpp router.cpp router.h utils.h test.cpp
