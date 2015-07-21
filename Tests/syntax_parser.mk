GPP = g++

INC_DIRS = -I$(UNITY_PATH) -I../

TARGET = syntax_parser.test

SRC_FILES = ../syntax_parser.c ../ast_node.c
SRC_FILES += syntax_parser.test.cpp
SRC_FILES += $(UNITY_PATH)/unity.c

CFLAGS = -Wall -Wextra -ggdb -fstack-protector-all
VALGRIND_FLAGS = -O0

test:
	$(GPP) $(CFLAGS) $(INC_DIRS) $(SRC_FILES) -o $(TARGET)
	./$(TARGET)

test-debug:
	$(GPP) $(CFLAGS) $(INC_DIRS) -D_DEBUG $(SRC_FILES) -o $(TARGET)
	./$(TARGET)

valgrind:
	$(GPP) $(VALGRIND_FLAGS) $(CFLAGS) $(INC_DIRS) $(SRC_FILES) -o $(TARGET)
	valgrind --leak-check=yes ./$(TARGET)