ifeq ($(OS),WIN32)
	CCFLAGS += -D WIN32
	ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
		CCFLAGS += -D AMD64
	endif
	ifeq ($(PROCESSOR_ARCHITECTURE),x86)
		CCFLAGS += -D IA32
	endif
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		OS = LINUX
		CCFLAGS += -D LINUX
	endif
	ifeq ($(UNAME_S),Darwin)
		OS = OSX
		CCFLAGS += -D OSX
	endif
	UNAME_P := $(shell uname -p)
	ifeq ($(UNAME_P),x86_64)
		CCFLAGS += -D AMD64
	endif
	ifneq ($(filter %86,$(UNAME_P)),)
		CCFLAGS += -D IA32
	endif
	ifneq ($(filter arm%,$(UNAME_P)),)
		CCFLAGS += -D ARM
	endif
endif


ifeq ($(OS), LINUX)
	CC=gcc
	CFLAGS=-O2 -W -Wall --std=gnu99 -fgnu89-inline
	CPPFLAGS=-DGLUS_ES2 -I ./GLUS -I /opt/vc/include -I /opt/vc/include/interface/vcos/pthreads -I /opt/vc/include/interface/vmcs_host/linux
	LDFLAGS=-lSDL -lEGL -lGLESv2 -lbcm_host -lvcos -lvchiq_arm -lpthread -lrt -lm -L /opt/vc/lib
endif
ifeq ($(OS), OSX)
	CC=gcc
	CFLAGS=-O0 -W -Wall --std=gnu99 -fgnu89-inline -g
	CPPFLAGS=-DGLUS_GL -I ./GLUS
	LDFLAGS=-framework OpenGL -framework Cocoa -framework QuartzCore -framework IOKit -lm -lglfw3
endif
ifeq ($(OS), WIN32)
	CC=!
	CFLAGS=
	CPPFLAGS=
	LDFLAGS=
endif


MAKEDEPEND=@set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

%.d: %.c Makefile
	@$(MAKEDEPEND)


DOMEDECAHEDRON_SRC= \
	main.c \
	gles2_harness.c \
	glus.c \
	dais_input.c \
	domedecahedron.c \
	domedecahedron_data.c \
	effect.c \
	effect_ca_0.c \
	effect_ca_1.c \
	effect_controller.c \
	effect_dusk.c \
	effect_plasma_0.c \
	effect_rings_0.c \
	effect_rings_1.c \
	effect_rings_2.c \
	effect_util.c \
	fix16.c \
	fix16_exp.c \
	fix16_sqrt.c \
	fix16_str.c \
	fix16_trig.c \
	fract32.c \
	uint32.c


-include $(DOMEDECAHEDRON_SRC:.c=.d)


.PHONY: all cleanall clean


%.o: %.c Makefile
	@$(MAKEDEPEND)
	gcc -c -g $(CFLAGS) -o $@ $(CPPFLAGS) $<

domedecahedron: $(DOMEDECAHEDRON_SRC:.c=.o)
	gcc -g -o $@ $(LDFLAGS) $^

all: dodecahall

cleanall:
	for X in dodecahall *.o *.o.* *.d *.d.*; do \
		if [ -f $$X ]; then rm $$X; fi \
		done

clean:
	for X in artifactuary *.o *.o.* *.d.*; do \
		if [ -f $$X ]; then rm $$X; fi \
		done

