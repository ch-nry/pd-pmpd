current:
	echo make pd_linux, pd_nt, pd_darwin

clean: ; rm -f *.pd_linux *.o

# ----------------------- NT -----------------------

pd_nt: pmpd.dll mass.dll link.dll mass2D.dll link2D.dll mass3D.dll link3D.dll iAmbient2D.dll iLine2D.dll iSeg2D.dll iCircle2D.dll tSquare2D.dll tLine2D.dll tSeg2D.dll tCircle2D.dll tLink2D.dll iAmbient3D.dll iSphere3D.dll iPlane3D.dll iCircle3D.dll iCylinder3D.dll tCube3D.dll tSphere3D.dll tPlane3D.dll tCircle3D.dll tCylinder3D.dll tLink3D.dll pmpd~.dll

.SUFFIXES: .dll

PDNTCFLAGS = /W3 /WX /DNT /DPD /nologo
VC="C:\Program Files\Microsoft Visual Studio\Vc98"

PDNTINCLUDE = /I. /I\tcl\include /I..\..\..\pd\src /I$(VC)\include

PDNTLDIR = $(VC)\lib
PDNTLIB = $(PDNTLDIR)\libc.lib \
	$(PDNTLDIR)\oldnames.lib \
	$(PDNTLDIR)\kernel32.lib \
	..\..\..\pd\bin\pd.lib 

.c.dll:
	cl $(PDNTCFLAGS) $(PDNTINCLUDE) /c $*.c
	link /dll /export:$*_setup $*.obj $(PDNTLIB)

# ----------------------- IRIX 5.x -----------------------

pd_irix5: pmpd.pd_irix5

.SUFFIXES: .pd_irix5

SGICFLAGS5 = -o32 -DPD -DUNIX -DIRIX -O2


SGIINCLUDE =  -I../../src/

.c.pd_irix5:
	cc $(SGICFLAGS5) $(SGIINCLUDE) -o $*.o -c $*.c
	ld -elf -shared -rdata_shared -o $*.pd_irix5 $*.o
	rm $*.o

# ----------------------- IRIX 6.x -----------------------

pd_irix6: pmpd.pd_irix6

.SUFFIXES: .pd_irix6

SGICFLAGS6 = -n32 -DPD -DUNIX -DIRIX -DN32 -woff 1080,1064,1185 \
	-OPT:roundoff=3 -OPT:IEEE_arithmetic=3 -OPT:cray_ivdep=true \
	-Ofast=ip32

.c.pd_irix6:
	cc $(SGICFLAGS6) $(SGIINCLUDE) -o $*.o -c $*.c
	ld -IPA -n32 -shared -rdata_shared -o $*.pd_irix6 $*.o
	rm $*.o

# ----------------------- LINUX i386 -----------------------

pd_linux: pmpd.pd_linux mass.pd_linux link.pd_linux mass2D.pd_linux link2D.pd_linux mass3D.pd_linux link3D.pd_linux iAmbient2D.pd_linux iLine2D.pd_linux iSeg2D.pd_linux iCircle2D.pd_linux tSquare2D.pd_linux tLine2D.pd_linux tSeg2D.pd_linux tCircle2D.pd_linux tLink2D.pd_linux iAmbient3D.pd_linux iSphere3D.pd_linux iPlane3D.pd_linux iCircle3D.pd_linux iCylinder3D.pd_linux tCube3D.pd_linux tSphere3D.pd_linux tPlane3D.pd_linux tCircle3D.pd_linux tCylinder3D.pd_linux tLink3D.pd_linux pmpd~.pd_linux

.SUFFIXES: .pd_linux

LINUXCFLAGS = -DPD -O2 -funroll-loops -fomit-frame-pointer \
    -Wall -W -Wshadow -Wstrict-prototypes -Werror \
    -Wno-unused -Wno-parentheses -Wno-switch

LINUXINCLUDE =  -I../../src

.c.pd_linux:
	cc $(LINUXCFLAGS) $(LINUXINCLUDE) -o $*.o -c $*.c
	ld -export_dynamic  -shared -o $*.pd_linux $*.o -lc -lm
	strip --strip-unneeded $*.pd_linux
	rm $*.o

# ----------------------- Mac OS X (Darwin) -----------------------

pd_darwin:  pmpd.pd_darwin mass.pd_darwin link.pd_darwin mass2D.pd_darwin link2D.pd_darwin mass3D.pd_darwin link3D.pd_darwin iAmbient2D.pd_darwin iLine2D.pd_darwin iSeg2D.pd_darwin iCircle2D.pd_darwin tSquare2D.pd_darwin tLine2D.pd_darwin tSeg2D.pd_darwin tCircle2D.pd_darwin tLink2D.pd_darwin iAmbient3D.pd_darwin iSphere3D.pd_darwin iPlane3D.pd_darwin iCircle3D.pd_darwin iCylinder3D.pd_darwin tCube3D.pd_darwin tSphere3D.pd_darwin tPlane3D.pd_darwin tCircle3D.pd_darwin tCylinder3D.pd_darwin tLink3D.pd_darwin pmpd~.pd_darwin

.SUFFIXES: .pd_darwin

DARWINCFLAGS = -DPD -DUNIX -DMACOSX -O2 \
    -Wall -W -Wshadow -Wstrict-prototypes \
    -Wno-unused -Wno-parentheses -Wno-switch

# where is your m_pd.h ???
DARWININCLUDE =  -I/. -I../pd/src -I../pd/obj

.c.pd_darwin:
	cc $(DARWINCFLAGS) $(DARWININCLUDE) -o $*.o -c $*.c
	cc -bundle -undefined suppress -flat_namespace -o $*.pd_darwin $*.o
	rm -f $*.o ../$*.pd_darwin
	ln -s $*/$*.pd_darwin ..


