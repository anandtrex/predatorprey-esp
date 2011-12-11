######################################################################
#
#  Changes by chernhan to accomodate GLUT (for linux) :
#    Changed CFLAGS = line
#    Added LDLIBS = line
#    Changed INCLUDES = line
#    Changed link line from LDFLAGS to LDLIBS
#
#  $Header: Makefile,v 1.1 93/08/03 16:10:14 polani Exp $
#
######################################################################

CC = clang++ #g++
LD = clang++ #g++

DIR = $(shell echo `pwd`)
CFLAGS = -g -O3 -Wall
LDLIBS = -L/usr/include/X11/ -L$(DIR)/glog/lib/ -L$(DIR)/libconfig/lib/ -L/usr/lib/X11 -lglut -lGL -lGLU -lXi -lXmu -lX11 -lm -lglog -lconfig++
LDFLAGS = -L.
ARCHIVES = -lm -lstdc++
INCLUDES = -I/p/include/Mesa -I/usr/include/X11 -I/usr/lib/X11 -I. -Iglog/include -Ilibconfig/include -I/usr/include/x86_64-linux-gnu #last include if for clang's benefit

DEPENDFLAGS = $(INCLUDES)

TARGET = esp-predprey
LIBNAME = libWal.a

OBJECTS = ESP.o Network.o signal-handler.o PredPrey.o PredPreyST1.o PredPreyST2.o


######################################################################
######################################################################

# Regeln

.SUFFIXES: .C

.C.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $*.C 


######################################################################


$(TARGET): Makefile $(OBJECTS) $(MYLIBS)
	$(LD) $(OBJECTS) $(LDLIBS) $(ARCHIVES) -o $(TARGET)



lib: $(OBJECTS) 
	ar ruv $(LIBNAME) $(OBJECTS)

depend :
	makedepend $(DEPENDFLAGS) *.C

clean::	
	rm -f $(TARGET)
	rm -f *.o
	rm -f *~
	rm -f core
	rm -f a.out	

cleandata::
	rm *.pop
	rm *.pca

pure::  clean
	rm -f $(EXTRAPURE)
	rm -f $(LIBNAME)

tar:
	tar cvf ESP.tar *.C *.h $(EXTRATAR) Makefile \
	esp_predprey.cfg config_file.txt install_dependencies.sh doxy_config condor_run README #\
	#ChangeLog Makefile RCS README USERDOC
	
doc:
	doxygen -g doxy_config


# DO NOT DELETE THIS LINE -- make depend depends on it.
