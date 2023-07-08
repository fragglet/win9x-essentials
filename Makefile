
ISO = win9x_essentials.iso
EXCLUDES = bootdisk .git .gitignore Makefile $(ISO)

.PHONY: $(ISO) clean bootdisk.img
$(ISO): bootdisk.img
	genisoimage -J $(patsubst %, -m %, $(EXCLUDES)) \
	            -b bootdisk.img -o $@ .

bootdisk.img:
	make -C bootdisk

clean:
	rm -f $(ISO) bootdisk.img

