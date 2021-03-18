ALL_BINARIES = $(wildcard *.cpp)
ALL_HEADERS = $(wildcard *.h)
SOURCE_BINARIES = $(filter-out test.cpp, $(ALL_BINARIES))
TEST_BINARIES = $(filter-out main.cpp, $(ALL_BINARIES))
MY_FILES = $(filter-out network.cpp, $(ALL_BINARIES)) $(filter-out network.h, $(ALL_HEADERS))

build:
	g++ -std=c++11 -O1 $(SOURCE_BINARIES) -o candidate_solution

build_test:
	g++ -std=c++11 -O1 $(TEST_BINARIES) -o write_checker_script

# Executes routing a few hundred times and comapres results against the checker.
test: build_test
	./write_checker_script
	chmod 777 run_checker.sh
	./run_checker.sh
	@make -s clean || true

# Same as `test`, except writes to a file, measures search time, and calculates
# speedup compared to the reference implementation.
bench: build_test
	./write_checker_script -r
	chmod 777 run_checker.sh
	./run_checker.sh > reference_run.log
	python3 ./bench.py reference_run.log
	@make -s clean || true

clean:
	@rm -f ./write_checker_script
	@rm -f ./run_checker.sh
	@rm -f *.log

format:
	clang-format -i $(MY_FILES)
