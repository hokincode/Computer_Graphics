DEPENDENDENT_DIRS = Image Util Ray
DEPENDENDENT_MAKEFILES = Makefile1 Makefile2 Makefile3 Makefile4

all:
	for dir in $(DEPENDENDENT_DIRS); do make -C $$dir; done
	for makefile in $(DEPENDENDENT_MAKEFILES); do make -f $$makefile; done

debug:
	for dir in $(DEPENDENDENT_DIRS); do make debug -C $$dir; done
	for makefile in $(DEPENDENDENT_MAKEFILES); do make -f $$makefile; done

clean:
	for dir in $(DEPENDENDENT_DIRS); do make clean -C $$dir; done
	for makefile in $(DEPENDENDENT_MAKEFILES); do make clean -f $$makefile; done
