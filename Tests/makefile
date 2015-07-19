GPP = g++ -g -Wall -Wextra

INC_DIRS = -I$(UNITY_PATH) -I../

TARGET = syntax_parser.test

SRC_FILES = ../syntax_parser.c ../ast_node.c
SRC_FILES += syntax_parser.test.cpp
SRC_FILES += $(UNITY_PATH)/unity.c

all:
	$(GPP) $(INC_DIRS) $(SRC_FILES) -o $(TARGET)
	./$(TARGET)
