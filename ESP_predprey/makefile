######################################################################
#
#  Changes by chernhan to accomodate GLUT (for linux) :
#    Changed CFLAGS = line
#    Added LDLIBS = line
#    Changed INCLUDES = line
#    Changed link line from LDFLAGS to LDLIBS
#
#  $Header: makefile,v 1.1 93/08/03 16:10:14 polani Exp $
#
######################################################################

CC = g++
LD = g++

DIR = .
CFLAGS = -g -O  -Wall 
LDLIBS = -L/lusr/X11/include/ -lglut  -lGL -lGLU -L/lusr/X11/lib -lXi -lXmu -lX11 -lm
LDFLAGS = -L.
ARCHIVES = -lm -lstdc++
## INCLUDES = -I/usr/include/g++
INCLUDES = -I/p/include/Mesa -I/lusr/X11/include/   -I/lusr/X11/lib

DEPENDFLAGS = $(INCLUDES)

TARGET = esp-predprey
LIBNAME = libWal.a

OBJECTS = ESP.o Network.o signal-handler.o 


######################################################################
######################################################################

# Regeln

.SUFFIXES: .C

.C.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $*.C 


######################################################################


$(TARGET): makefile $(OBJECTS) PredPrey.o $(MYLIBS)
	$(LD) PredPrey.o $(OBJECTS) $(LDLIBS) $(ARCHIVES) -o $(TARGET)



lib: $(OBJECTS) 
	ar ruv $(LIBNAME) $(OBJECTS)

depend :
	makedepend $(DEPENDFLAGS) *.C

clean::	
#	rm -f $(TARGET)
#	rm -f *.o
#	rm -f *~
#	rm -f core
#	rm -f a.out	

cleandata::
	rm *.pop
	rm *.pca

pure::  clean
	rm -f $(EXTRAPURE)
	rm -f $(LIBNAME)

tar:
	tar cvf ESP.tar *.C *.h $(EXTRATAR) \
	ChangeLog makefile RCS README USERDOC


# DO NOT DELETE THIS LINE -- make depend depends on it.
