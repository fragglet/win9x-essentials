
ISO = win9x_essentials.iso

.PHONY: $(ISO) clean
$(ISO): 
	genisoimage -J -m $@ -m .git -m .gitignore -m bootdisk \
	            -b bootdisk.img -hard-disk-boot -o $@ .

clean:
	rm -f $(ISO)

