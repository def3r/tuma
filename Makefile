SRCFILES := $(shell ls ./src/*)

.PHONY: all

all:
	gcc -I./include/ -I./raylib/include/ ${SRCFILES} -L./raylib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o tuma
