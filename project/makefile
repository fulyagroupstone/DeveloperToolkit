name := project

libs := -llog

bin := $(patsubst src/%,bin/%.o,$(wildcard src/*))

all: clean $(name)

clean:
	@echo Cleaning $(name)
	@rm -f bin/*
	@rm -f $(name)

$(name): $(bin)
	@echo Linking $(name)
	@g++ -o $@ $^ -L ../lib $(libs) -std=c++2b

$(bin):bin/%.o:src/%
	@echo Compiling $^
	@g++ -c -o $@ $^ -std=c++2b -I headers -I ../includes