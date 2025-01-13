.PHONY: clean ../bootdisk.img

../bootdisk.img: freedos-minimal.img prompter-boot.bin prompter/promptify.py
	cp $< $@
	SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy \
	    dosbox -conf mkdisk.cfg
	./prompter/promptify.py $@

prompter-boot.bin: prompter/boot.asm
	nasm $< -o $@

clean:
	rm -f ../bootdisk.img prompter-boot.bin
