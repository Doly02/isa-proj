#   Project:        ISA Project - IMAP Client With TLS Support
#   File Name:      Makefile
#   Author:         Tomas Dolak
#   Date:           24.09.2024
#   Description:    Makefile for IMAPv4 Client Application.

# Program Name
TARGET = imapcl
# Program Name For Debug Configuration
DEBUG_TARGET = imapcl_debug
# Test Program Name
TEST_TARGET = imapcl_test

# Compiler
CC = g++
# Compiler Flags
CFLAGS = -std=c++17 -Wall -Wextra -Werror -Wshadow -Wnon-virtual-dtor -pedantic -Iinclude
DEBUG_CFLAGS = -fsanitize=address -g -std=c++17 -Wall -Wextra -Werror -Wshadow -Wnon-virtual-dtor -pedantic

# Header Files
HEADERS = include/definitions.hpp
# include/definitions.hpp include/utilities.hpp include/ClientConfig.hpp include/BaseImapClient.hpp
# Libraries
LIBS = #-lpcap

# Source Files
SOURCES = src/main.cpp
# src/utilities.cpp src/ClientConfig.cpp  src/BaseImapClient.cpp
# Object Files 
OBJECTS = $(SOURCES:.cpp=.o)

# Test Source Files
TEST_SOURCES = tests/SnifferConfigTest.cpp
# Test Object Files (Derived from TEST_SOURCES)
TEST_OBJECTS = $(TEST_SOURCES:.cpp=.o)
# Google Test Flags
GTEST_FLAGS = -lgtest -lgtest_main -pthread
# Default build target
all: $(TARGET)

# Main target
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	rm -f $(OBJECTS)

# Object compilation
%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJECTS) $(TARGET) $(TEST_OBJECTS) $(TEST_TARGET)
