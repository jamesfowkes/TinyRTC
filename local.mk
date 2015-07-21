TARGET = TinyRTC

C_OBJDEPS = application.o ast_node.o messaging.o syntax_parser.o msggetter.local.o
INCLUDES = -I$(CODE_LIBRARY_PATH)/Utility

GPP = g++
CFLAGS = -Wall -Wextra

%.o:%.c
	$(GPP) $(INCLUDES) $(CFLAGS) -c $<

all: $(C_OBJDEPS)
	$(GPP) $(CFLAGS) $^ -o $(TARGET)