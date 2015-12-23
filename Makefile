# Makefile for Appy

# Linker flags
# Links the ncurses panel library
LDFLAGS = -lpanel
# Links the ncurses menu library
LDFLAGS += -lmenu
# Links the ncurses forms library
LDFLAGS += -lform
# Primary ncurses library needs to be loaded lasts
LDFLAGS += -lncurses 

# Targets
# Dummy target for complete build
all: appy
	