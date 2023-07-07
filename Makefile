
ISO = win9x_essentials.iso

.PHONY: $(ISO) clean bootdisk.img
$(ISO): bootdisk.img
	genisoimage -J -m $@ -m .git -m .gitignore -m bootdisk \
	            -b bootdisk.img -o $@ .

bootdisk.img:
	make -C bootdisk

clean:
	rm -f $(ISO) bootdisk.img

