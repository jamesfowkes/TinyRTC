GPP = g++ -g

INC_DIRS = -I$(UNITY_PATH)

NEW_SRC_FILES = syntax_parser.c
NEW_SRC_FILES += ast_node.c
NEW_SRC_FILES += syntax_parser.test.cpp
NEW_SRC_FILES += $(UNITY_PATH)/unity.c

test_syntax_parser:
	$(GPP) $(INC_DIRS) $(NEW_SRC_FILES) -o syntax_parser.test.exe
	./syntax_parser.test.exe
