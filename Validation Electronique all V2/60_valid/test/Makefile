CXX      = g++
CXXFLAGS = -Wall -Wextra -Wpedantic -std=c++17 \
           -I ../../30_src/components/config/include \
           -I ../../30_src/components/low/hal/sensors/include \
           -I ../../30_src/components/app/include

SRCS     = test_main.cpp \
           ../../30_src/components/low/hal/sensors/drv_ina237_convert.cpp \
           ../../30_src/components/app/app_power_monitor_logic.cpp

BIN      = test_runner

.PHONY: all test clean

all: test

$(BIN): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(BIN)

test: $(BIN)
	./$(BIN)

clean:
	rm -f $(BIN)
