# 总控 Makefile

.PHONY: all build run clean test gui

all: build

build:
	$(MAKE) -C backend

run: build
	python frontend/main.py

gui: run

test:
	$(MAKE) -C backend test

clean:
	$(MAKE) -C backend clean
