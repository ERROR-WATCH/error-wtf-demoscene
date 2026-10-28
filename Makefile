CC ?= gcc
CFLAGS ?= -O2 -Wall -Wextra

.PHONY: all smoke 1m errorwtf-1m verify-1m-size sizes clean

all: smoke

build:
	mkdir -p build

build/1m:
	mkdir -p build/1m

smoke: build build/smoke_av

build/smoke_av: tools/smoke_av.c | build
	$(CC) $(CFLAGS) tools/smoke_av.c -o $@ -lEGL -lGL -lasound -lm

# ---- ERROR.wtf // 1MiB (BEYOND THE ERROR: ONE MEGABYTE) ----
build/1m/errorwtf_1m_prod: src/1m/production.c | build/1m
	$(CC) $(CFLAGS) -DEW_SECONDS=120 src/1m/production.c -o $@ -lglfw -lGL -lasound -lEGL -lm

errorwtf-1m: build/1m/errorwtf_1m_prod
	cp build/1m/errorwtf_1m_prod errorwtf-1m
	bash tools/1m/pad_exact.sh errorwtf-1m 1048576

1m: errorwtf-1m verify-1m-size

verify-1m-size:
	@stat -c '%n %s' errorwtf-1m
	@file errorwtf-1m | grep -q "ELF 64-bit" && echo "ELF OK (x86-64)"

sizes: errorwtf-4k errorwtf-16k errorwtf-64k
	@stat -c '%n %s' errorwtf-4k errorwtf-16k errorwtf-64k

clean:
	rm -rf build
