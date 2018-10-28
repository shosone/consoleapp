#for build libraries
CC               := gcc
CFLAGS_RELEASE   := -Wall -O3 
CFLAGS_DEBUG     := -Wall -g3 -O0 
ARCHIVER         := ar
OBJFLAGS         := rcs
INC_PATH         := ./src
SRC_PATH         := ./src
LIB_PATH_RELEASE := ./lib/release
LIB_PATH_DEBUG   := ./lib/debug
OBJ_PATH_RELEASE := ./obj/release
OBJ_PATH_DEBUG   := ./obj/debug
TARGET_RELEASE   := libprompt.a liboption.a
TARGET_DEBUG     := libprompt_debug.a liboption_debug.a

#for build c sample
SRC_PATH_CSAMPLE := ./sample/c
TARGET_CSAMPLE   := c_prompt c_option
FLAGS_CSAMPLE    := -g3 -Wall -lprompt_debug -loption_debug -O0 

#for build cpp sample
CPPC               := g++
SRC_PATH_CPPSAMPLE := ./sample/cpp
TARGET_CPPSAMPLE   := cpp_prompt cpp_option
FLAGS_CPPSAMPLE    := -g3 -Wall -lprompt_debug -loption_debug  -O0

#for install
LIB_DIR  := /usr/lib
INC_DIR  := /usr/include/consoleapp

vpath %.h   $(INC_PATH)
vpath %.c   $(SRC_PATH) $(SRC_PATH_CSAMPLE)
vpath %.o   $(OBJ_PATH_RELEASE) $(OBJ_PATH_DEBUG)
vpath %.a   $(LIB_PATH_RELEASE) $(LIB_PATH_DEBUG) 
vpath %.cpp $(SRC_PATH_CPPSAMPLE)

.PHONY: clean tag install uninstall 

all: 
	@for r in $(TARGET_RELEASE); \
	do                           \
		make $$r;                \
	done
	@for d in $(TARGET_DEBUG);   \
	do                           \
		make $$d;                \
	done
	@for s in $(TARGET_CSAMPLE);  \
	do                            \
		make $$s;                 \
	done
	@for s in $(TARGET_CPPSAMPLE);  \
	do                              \
		make $$s;                   \
	done
	ctags -R --language-force=C

c_%: sample_%.c $(TARGET_DEBUG)
	$(CC) -I$(INC_PATH) -L$(LIB_PATH_DEBUG) -o$(SRC_PATH_CSAMPLE)/$@ $< $(FLAGS_CSAMPLE)

cpp_%: sample_%.cpp $(TARGET_DEBUG)
	$(CPPC) -I$(INC_PATH) -L$(LIB_PATH_DEBUG) -o$(SRC_PATH_CPPSAMPLE)/$@ $< $(FLAGS_CPPSAMPLE) 

lib%_debug.a: %_debug.o %_errmsg_debug.o common_debug.o
	@mkdir -p $(LIB_PATH_DEBUG)
	$(ARCHIVER) $(OBJFLAGS) $@ $(patsubst %,$(OBJ_PATH_DEBUG)/%,$(filter-out $(OBJ_PATH_DEBUG)%,$^)) $(filter $(OBJ_PATH_DEBUG)%,$^)
	@mv $@ $(LIB_PATH_DEBUG)

lib%.a: %.o %_errmsg.o common.o
	@mkdir -p $(LIB_PATH_RELEASE)
	$(ARCHIVER) $(OBJFLAGS) $@ $(patsubst %,$(OBJ_PATH_RELEASE)/%,$(filter-out $(OBJ_PATH_RELEASE)%,$^)) $(filter $(OBJ_PATH_RELEASE)%,$^)
	@mv $@ $(LIB_PATH_RELEASE)

%_debug.o: %.c %.h 
	@mkdir -p $(OBJ_PATH_DEBUG)
	$(CC) $(CFLAGS_DEBUG) -I$(INC_PATH) -o$@ -c $(SRC_PATH)/$*.c
	@mv $@ $(OBJ_PATH_DEBUG)

%.o: %.h %.c confing.h
	@mkdir -p $(OBJ_PATH_RELEASE)
	$(CC) $(CFLAGS_RELEASE) -I$(INC_PATH) -c $(SRC_PATH)/$*.c
	@mv $@ $(OBJ_PATH_RELEASE)

confing.h:
	touch src/config.h

install:
	sudo mkdir $(INC_DIR)
	@for r in $(TARGET_RELEASE);                    \
	do                                              \
		make $$r;                                   \
		sudo cp $(LIB_PATH_RELEASE)/$$r $(LIB_DIR); \
		sudo cp $(INC_PATH)/*.h $(INC_DIR);         \
	done

uninstall:
	sudo rm -rf $(INC_DIR)
	@for r in $(TARGET_RELEASE);   \
	do                             \
		sudo rm -f $(LIB_DIR)/$$r; \
	done

tag:
	ctags -R --language-force=C

clean:
	find . -name "*.[ao]" | xargs rm -f
	rm -rf obj
	rm -rf lib
	rm -f tags
	@for s in $(TARGET_CSAMPLE);       \
	do                                 \
		rm -f $(SRC_PATH_CSAMPLE)/$$s; \
	done
	@for s in $(TARGET_CPPSAMPLE);       \
	do                                   \
		rm -f $(SRC_PATH_CPPSAMPLE)/$$s; \
	done
