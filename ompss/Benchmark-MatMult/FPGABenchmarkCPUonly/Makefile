ALPHADATADIR=alphadata

#CPPFLAGS=-g -I$(ALPHADATADIR)/include -I$(ALPHADATADIR)/app -I. -I/home/dev/FirmataC/includes  -Wno-write-strings -O0
CPPFLAGS=-I$(ALPHADATADIR) -I. -Iutils -I/home/dev/FirmataC/includes  -Wno-write-strings -O0 -mtune=native -Wvolatile-register-var
#CFLAGS=-std=c99
FLAGTB= -I$(ALPHADATADIR) -I. -Iutils -I/home/dev/FirmataC/includes  -Wno-write-strings -O0 -mtune=native -Wvolatile-register-var
LDFLAGS=-L/home/dev/FirmataC/ -ladb3 -ladmxrc3
VPATH=$(ALPHADATADIR):utils

test: timer.o alphadata.o c_wrapper.o tb_MM100k.o functionC/*.o
	mcxx $(LDFLAGS) --instrument -o $@ --ompss $^ -lfirmatac

tb_MM100k.o:
	mcxx $(LDFLAGS) $(FLAGTB) --instrument -c tb_MM100k.cpp --ompss -lfirmatac 

clean:
	rm -f *.o test
edit:
	gedit *.cpp functionC/*.c functionC/*.h &
