.PHONY: all
all:
	make -C build

.PHONY: regen
regen:
	mkdir -p build
	cd build ; cmake ..

.PHONY: clean
clean:
	make -C build clean

.PHONY: distclean
distclean:
	rm -rf build/
