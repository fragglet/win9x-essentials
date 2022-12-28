
ISO = win9x_essentials.iso

.PHONY: $(ISO) clean
$(ISO): 
	genisoimage -J -m $@ -m .git -m .gitignore -o $@ .

clean:
	rm -f $(ISO)

