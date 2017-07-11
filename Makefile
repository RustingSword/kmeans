UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Darwin)
	CXX = g++-7
else
	CXX = g++
endif
CXXFLAGS = -fopenmp -std=c++11 -Wall -Wfatal-errors -march=native -O2
INCLUDEFLAGS = -I include

INCLUDE_DIR = include
BIN_DIR = bin
SRC_DIR = src
TEST_DIR = tests

HEADERS = $(wildcard $(INCLUDE_DIR)/*.h)
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst %.cpp,%.o, $(SRCS))

TEST_SRCS = $(wildcard $(TEST_DIR)/*.cpp)
TEST_BINS = $(patsubst $(TEST_DIR)/%.cpp, $(BIN_DIR)/%, $(TEST_SRCS))

.PHONY: all binary clean test

all: binary

binary: $(OBJS) $(TEST_BINS)

$(OBJS): %.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDEFLAGS) -c $< -o $@

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TEST_BINS): $(BIN_DIR)/%: $(TEST_DIR)/%.cpp $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDEFLAGS) $(OBJS) $< -o $@

test: $(TEST_BINS)
	@$(foreach test_bin,$(TEST_BINS),$(test_bin);)

clean:
	rm -rf $(OBJS) $(BIN_DIR)
