
ISO = win9x_essentials.iso
EXCLUDES = bootdisk .git .gitignore .github Makefile $(ISO)

.PHONY: $(ISO) clean bootdisk.img
$(ISO): bootdisk.img
	genisoimage -J $(patsubst %, -m %, $(EXCLUDES)) \
	            -V "9x Essentials" \
	            -b bootdisk.img -o $@ .

bootdisk.img:
	make -C bootdisk

clean:
	rm -f $(ISO) bootdisk.img

