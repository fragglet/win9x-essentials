../bootdisk.img: freedos-minimal.img
	cp $< $@
	SDL_VIDEODRIVERx=dummy SDL_AUTIODRIVERx=dummy \
	    dosbox -conf mkdisk.cfg

