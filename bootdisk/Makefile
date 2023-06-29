.PHONY: clean ../bootdisk.img

../bootdisk.img: freedos-minimal.img
	cp $< $@
	SDL_VIDEODRIVER=dummy SDL_AUTIODRIVER=dummy \
	    dosbox -conf mkdisk.cfg

clean:
	rm -f ../bootdisk.img

