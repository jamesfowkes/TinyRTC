GPP = g++ -g

INC_DIRS = -I$(UNITY_PATH)

NEW_SRC_FILES = syntax_parser.c
NEW_SRC_FILES += syntax_parser.test.new.cpp
NEW_SRC_FILES += $(UNITY_PATH)/unity.c

OLD_SRC_FILES = syntax_parser_old.c
OLD_SRC_FILES += syntax_parser.test.old.cpp
OLD_SRC_FILES += $(UNITY_PATH)/unity.c

new:
	$(GPP) $(INC_DIRS) $(NEW_SRC_FILES) -o new.test
	./new.test

old:
	$(GPP) $(INC_DIRS) $(OLD_SRC_FILES) -o old.test
	./old.test