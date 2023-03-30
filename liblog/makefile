name := liblog.a

bin := $(patsubst src/%,bin/%.o,$(wildcard src/*))

all: clean $(name)

clean:
	@echo Cleaning $(name)
	@rm -f bin/*
	@rm -f $(name)

$(name): $(bin)
	@echo Creating $(name)
	@ar crv $@ $^

$(bin):bin/%.o:src/%
	@echo Compiling $^
	@g++ -c -o $@ $^ -std=c++2b -I headers -I ../includes