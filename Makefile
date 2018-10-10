CC               = gcc
INC_PATH         = ./src
SRC_PATH         = ./src
SAMPLE_SRC_PATH  = ./sample
LIB_PATH_RELEASE = ./lib/release
LIB_PATH_DEBUG   = ./lib/debug
OBJ_PATH_RELEASE = ./obj/release
OBJ_PATH_DEBUG   = ./obj/debug
CFLAGS_RELEASE   = -Wall -O3 -D_GNU_SOURCE
CFLAGS_DEBUG     = -Wall -g3 -O0 -D_GNU_SOURCE
CFLAGS_LINK_LIB  = -lreadline

vpath %.h $(INC_PATH)
vpath %.c $(SRC_PATH) $(SAMPLE_SRC_PATH)
vpath %.o $(OBJ_PATH_RELEASE) $(OBJ_PATH_DEBUG)
vpath %.a $(LIB_PATH_RELEASE) $(LIB_PATH_DEBUG) 

.PHONY: clean tag

all: 
	make release
	make debug
	make sample
	ctags -R

sample: sample.c libconsoleapp_debug.a
	$(CC) $(CFLAGS_DEBUG) -I$(INC_PATH) -L$(LIB_PATH_DEBUG) -o$(SAMPLE_SRC_PATH)/$@ $(SAMPLE_SRC_PATH)/sample.c -lconsoleapp_debug -lreadline

release: option.o prompt.o
	mkdir -p $(LIB_PATH_RELEASE)
	ar rcs libconsoleapp.a $(OBJ_PATH_RELEASE)/*
	mv libconsoleapp.a $(LIB_PATH_RELEASE)

debug: option_debug.o prompt_debug.o
	mkdir -p $(LIB_PATH_DEBUG)
	ar rcs libconsoleapp_debug.a $(OBJ_PATH_DEBUG)/*
	mv libconsoleapp_debug.a $(LIB_PATH_DEBUG)

%_debug.o: %.c %.h
	mkdir -p $(OBJ_PATH_DEBUG)
	$(CC) $(CFLAGS_DEBUG) -I$(INC_PATH) -o$@ -c $(SRC_PATH)/$*.c
	mv $@ $(OBJ_PATH_DEBUG)

%.o: %.h %.c
	mkdir -p $(OBJ_PATH_RELEASE)
	$(CC) $(CFLAGS_RELEASE) -I$(INC_PATH) -c $(SRC_PATH)/$*.c
	mv $@ $(OBJ_PATH_RELEASE)

tag:
	ctags -R --language-force=C

clean:
	find . -name "*.[ao]" | xargs rm -f
	rm -rf obj
	rm -rf lib
	rm -f tags
	rm -f $(SAMPLE_SRC_PATH)/sample
