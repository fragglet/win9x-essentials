
ISO = win9x_essentials.iso

.PHONY: $(ISO) clean
$(ISO): bootdisk.img
	genisoimage -J -m $@ -m .git -m .gitignore -m bootdisk \
	            -b bootdisk.img -hard-disk-boot -o $@ .

bootdisk.img:
	make -C bootdisk

clean:
	rm -f $(ISO) bootdisk.img

