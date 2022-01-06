CC = mingw32-gcc

default: RipMort

HEADERS = main.h \
pack_file.h

OBJECTS = main.o pack_file.o

RELEASE ?= 0

%.o: %.c $(HEADERS)
ifeq ($(RELEASE), 1)
	CC -c $< -o $@ -O3 -ffunction-sections -fdata-sections
else
	CC -c $< -o $@ -D_PF_DEBUG
endif

RipMort: $(OBJECTS)
ifeq ($(RELEASE), 1)
	CC $(OBJECTS) -o $@ -s -O2 -Wl,--gc-sections --static
	strip -s $@.exe
else
	CC $(OBJECTS) -o $@
endif

clean:
	-rm -f $(OBJECTS)
	-rm -f RipMort