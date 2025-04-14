

OPTIMIZER_FLAGS := -Os -ffast-math

ifeq ($(OS),Windows_NT)
	OUT_NAME := main.exe
else
	UNAME_S := $(shell uname-s)
	ifeq ($(UNAME_S),Linux)
		OUT_NAME:= main.out
	endif
endif

main: main.cpp
	g++ main.cpp -I"dependencies/Tiger/include" $(OPTIMIZER_FLAGS) -o $(OUT_NAME)

