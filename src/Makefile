HOME = ..

TARGETS = $(POLI) $(CARA)

OBJECTS = $(OBJPOLI) $(OBJCARA)

#######
VtkIncDir = /usr/include/vtk
VtkLibDir = -L/usr/lib/vtk-5.4
VtkLibs = -lvtkImaging -lvtkGraphics -lvtkCommon -lvtkRendering -lvtkFiltering -lvtkIO -lvtkCommon


########
# Mesa #
########
MesaIncDir = /usr/include/GL /usr/X11R6/include/GL
MesaLibs = -L/usr/X11R6/lib -lXext -lSM -lICE -lX11 -lm



###############################
# esto es general

CPP = g++ 

incdir1 = $(HOME)/inc
incdir2 = /usr/include/magick


SRC= $(HOME)/src
OBJ = $(HOME)/obj
bindir=$(HOME)/bin

INCLUDES = -I$(subst :, -I,$(VtkIncDir)) -I$(subst :, -I,$(Mesaincdir)) -I$(subst :, -I,$(incdir2)) -I$(subst :, -I,$(incdir1))


CPPFLAGS = -g -O2 -Wno-deprecated




##########################################
# POLI

POLI = poli
OBJPOLI = Politext.o


installpoli: $(bindir)/$(POLI)
$(bindir)/poli: $(OBJ)/poli
	-rm $(bindir)/poli
	cp -p $(OBJ)/poli $(bindir)/poli

poli: $(OBJ)/poli

$(OBJ)/poli: $(addprefix $(OBJ)/, $(OBJPOLI))
	$(CPP) $(INCLUDES) -rdynamic $(CPPFLAGS) $(VTKFLAGS) -o $(addprefix $(OBJ)/,$(POLI)) $(addprefix $(OBJ)/, $(OBJPOLI)) -lpthread  $(VtkLibDir) $(VtkLibs) $(MesaLibs)


##########################################
# CARA

CARA = cara
OBJCARA = cara.o 


installcara: $(bindir)/$(CARA)
$(bindir)/cara: $(OBJ)/cara
	-rm $(bindir)/cara
	cp -p $(OBJ)/cara $(bindir)/cara

cara: $(OBJ)/cara

$(OBJ)/cara: $(addprefix $(OBJ)/, $(OBJCARA))
	$(CPP) $(INCLUDES) -rdynamic $(CPPFLAGS) $(VTKFLAGS) -o $(addprefix $(OBJ)/,$(CARA)) $(addprefix $(OBJ)/, $(OBJCARA)) -lpthread  $(VtkLibDir) $(VtkLibs) $(MesaLibs)


#########################################
# reglas generales


$(OBJ)/%.o : %.cpp
	$(CPP) -c $(INCLUDES) $(CPPFLAGS)  $< -o $@


.PHONY: poli cara

all: directorios $(TARGETS)

clean:
	-rm $(TARGETS)
	-rm $(OBJ)/*.o

cleanall: clean

build: clean all

install: $(subst $(OBJ),$(bindir),$(TARGETS))

directorios:
	@if [ ! -d $(bindir) ]; then mkdir $(bindir); fi
	@if [ ! -d $(OBJ) ]; then mkdir $(OBJ); fi



