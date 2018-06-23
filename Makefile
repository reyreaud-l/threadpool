CXX=g++

INCLUDE_DIR=include/
LIB=$(INCLUDE_DIR)/threadpool.hpp

OBJS_DIR=objs

TESTS=tests/main.cpp
TESTS_OBJS=$(TESTS:%.cpp=$(OBJS_DIR)/%.o)
TESTS_DEP=$(TESTS_OBJS:%.o=%.d)
TESTS_BIN=check

CXXFLAGS=-Wall -Wextra -pedantic -std=c++17 -I$(INCLUDE_DIR) -O0 -g3 -fsanitize=thread
LDFLAGS=-lpthread

all: check

install:
	@echo "not implemented"

-include $(TESTS_DEP)

$(TESTS_BIN): $(TESTS_OBJS)
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $^ -o $@
	./$(TESTS_BIN)

$(OBJS_DIR)/%.o: %.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

clean:
	$(RM) -f $(TESTS_OBJS)
	$(RM) -f $(TESTS_BIN)
	$(RM) -rf $(OBJS_DIR)

.PHONY: all install $(TESTS_BIN) clean
