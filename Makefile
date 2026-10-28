CC ?= gcc
CFLAGS ?= -O2 -Wall -Wextra

.PHONY: all smoke clean sizes

all: smoke

build:
	mkdir -p build

smoke: build build/smoke_av

build/smoke_av: tools/smoke_av.c | build
	$(CC) $(CFLAGS) tools/smoke_av.c -o $@ -lEGL -lGL -lasound -lm

sizes: errorwtf-4k errorwtf-16k errorwtf-64k
	@stat -c '%n %s' errorwtf-4k errorwtf-16k errorwtf-64k

clean:
	rm -rf build
