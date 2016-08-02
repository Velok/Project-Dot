BINARY_FILE = bin/dot
BUILD_FOLDERS = build/

FILE001 = dot


BUILD_LIST = build/$(FILE001).o
ARGS_LIST = -lallegro -lallegro_main -lallegro_dialog -lallegro_image -lallegro_audio -lallegro_acodec -lallegro_font -lallegro_ttf

COMP_CMD = gcc -c -o

bin: $(BUILD_LIST)
	gcc -o $(BINARY_FILE) $(BUILD_LIST) $(ARGS_LIST)

build/$(FILE001).o: src/$(FILE001).c
	$(COMP_CMD) build/$(FILE001).o src/$(FILE001).c -Iinclude/


clean:
	rm -rf build/
	rm -rf bin/*
	for dir in $(BUILD_FOLDERS); do \
		mkdir $$dir; \
	done

exec:
	$(BINARY_FILE)