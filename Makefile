## Compilation flags
# -g -> generate debug info
# -Werror -> treat every warning as an error
# G++ = g++ -g -std=c++17 -Werror

ALL_BINARIES = $(wildcard *.cpp)
SOURCE_BINARIES = $(filter-out test.cpp, $(ALL_BINARIES))

# run: build

build:
	# $(G++) -o calc $(SOURCE_BINARIES) main.cpp
	g++ -std=c++11 -O1 $(SOURCE_BINARIES) -o candidate_solution

test: build
	./candidate_solution Council_Bluffs_IA Cadillac_MI | xargs -I{} ./checker_osx {} || true

# .PHONY: test
# test: build
# 	$(G++) -o test_calc $(SOURCE_BINARIES) test.cpp
# 	./test_calc
# 	@make clean

# clean:
# 	@rm calc &> /dev/null || true
# 	@rm test_calc &> /dev/null || true
# 	@rm -rf *.dSYM &> /dev/null || true
