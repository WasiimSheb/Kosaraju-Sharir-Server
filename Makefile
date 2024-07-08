## Credits to Shifaa and Wasim
## Wasimshebalny@gmail.com
## Shifaakhatib28@gmail.com

# List of subdirectories
SUBDIRS = Q1 Q2 Q3 Q4 Q6 Q7 Q9 Q10

# Default target
all: $(SUBDIRS)

# Rule to build each subdirectory
$(SUBDIRS):
	$(MAKE) -C $@

# Recursive call to build all subdirectories
exe1:
	$(MAKE) -C Q1

exe2:
	$(MAKE) -C Q2

exe3:
	$(MAKE) -C Q3

exe4:
	$(MAKE) -C Q4


exe6:
	$(MAKE) -C Q6

exe7:
	$(MAKE) -C Q7


exe9:
	$(MAKE) -C Q9

exe10:
	$(MAKE) -C Q10

# Clean target for each subdirectory
.PHONY: clean $(SUBDIRS)
clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done
