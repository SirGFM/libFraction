
#==============================================================================
# Define every object required by compilation
#==============================================================================
  OBJS =                       \
          $(OBJDIR)/fraction.o \
          $(OBJDIR)/prime.o 
#==============================================================================

#==============================================================================
# Set flags from the selected rule
#==============================================================================
# Set which mode to compile (either release or debug, and defaults to debug)
  ifneq (,$(findstring release, $(MAKECMDGOALS)))
    RELEASE := yes
    DEBUG := no
  endif
  ifneq (,$(findstring debug, $(MAKECMDGOALS)))
    RELEASE := no
    DEBUG := yes
  endif
  CC := gcc
#==============================================================================

#==============================================================================
# Clear the suffixes' default rule, since there's an explicit one
#==============================================================================
.SUFFIXES:
#==============================================================================

#==============================================================================
# Define all targets that doesn't match its generated file
#==============================================================================
.PHONY: clean debug distclean install release
#==============================================================================

#==============================================================================
# Define compilation target
#==============================================================================
  TARGET := libFraction
  MAJOR_VERSION := 1
  MINOR_VERSION := 0
  REV_VERSION := 0
# If the DEBUG flag was set, generate another binary (so it doesn't collide
# with the release one)
  ifeq ($(DEBUG), yes)
    TARGET := $(TARGET)_dbg
  endif
#==============================================================================

#==============================================================================
# Set OS flag
#==============================================================================
  OS := $(shell uname)
  ifeq ($(OS), MINGW32_NT-6.1)
    OS := Win
  endif
  ifeq ($(CC), emcc)
    OS := emscript
  endif
#==============================================================================

#==============================================================================
# Set the binary extensions (if on Windows, since some rules requires it)
#==============================================================================
  ifeq ($(OS), Win)
    BIN_EXT := .exe
  else
    BIN_EXT :=
  endif
#==============================================================================

#==============================================================================
# Define CFLAGS (compiler flags)
#==============================================================================
# Add all warnings and default include path
  CFLAGS := -Wall -I"./include/" -I"./src/include"
# Add architecture flag
  ARCH := $(shell uname -m)
  ifeq ($(OS), emscript)
    CFLAGS := $(CFLAGS) -I"$(EMSCRIPTEN)/system/include/" -m32
  else
    ifeq ($(ARCH), x86_64)
      CFLAGS := $(CFLAGS) -m64
    else
      CFLAGS := $(CFLAGS) -m32
    endif
  endif
# Add debug flags
  ifeq ($(OS), emscript)
    CFLAGS := $(CFLAGS) -O2
  else
    ifneq ($(RELEASE), yes)
      CFLAGS := $(CFLAGS) -g -O0 -DDEBUG
    else
      CFLAGS := $(CFLAGS) -O3
    endif
  endif
# Set flags required by OS
  ifeq ($(OS), Win)
    CFLAGS := $(CFLAGS) -I"/d/windows/mingw/include"
  else
    CFLAGS := $(CFLAGS) -fPIC
  endif
#==============================================================================

#==============================================================================
# Define LFLAGS (linker flags)
#==============================================================================
  LFLAGS :=
# Add libs and paths required by an especific OS
  ifeq ($(OS), Win)
    ifeq ($(ARCH), x64)
      LFLAGS := $(LFLAGS) -L"/d/windows/mingw/lib"
    else
      LFLAGS := $(LFLAGS) -L"/d/windows/mingw/mingw32/lib"
    endif
    LFLAGS := $(LFLAGS) -lmingw32
  endif
#==============================================================================

#==============================================================================
# Define where source files can be found and where objects & binary are output
#==============================================================================
 VPATH := src:tst
 TESTDIR := tst
 OBJDIR := obj/$(OS)
 BINDIR := bin/$(OS)
 ifeq ($(OS), Win)
    ifeq ($(ARCH), x64)
      LIBPATH := /d/windows/mingw/lib
    else
      LIBPATH := /d/windows/mingw/mingw32/lib
    endif
    HEADERPATH := /d/windows/mingw/include
 else
   LIBPATH := /usr/lib
   HEADERPATH := /usr/include
 endif
#==============================================================================

#==============================================================================
# (Automatically) create a list with every .c file used
#==============================================================================
 ALL_SRC := $(OBJS:$(OBJDIR)%.o=src%.c)
#==============================================================================

#==============================================================================
# Automatically look up for tests and compile them
#==============================================================================
 TEST_SRC := $(wildcard $(TESTDIR)/*.c)
 TEST_BIN := $(TEST_SRC:%.c=%$(BIN_EXT))
#==============================================================================

#==============================================================================
# Make the objects list constant (and the icon, if any)
#==============================================================================
 OBJS := $(OBJS)
#==============================================================================

#==============================================================================
# Set shared library's extension
#==============================================================================
 ifeq ($(OS), Win)
   SO := dll
   MJV := $(SO)
   MNV := $(SO)
 else
   SO := so
   MJV := $(SO).$(MAJOR_VERSION)
   MNV := $(SO).$(MAJOR_VERSION).$(MINOR_VERSION).$(REV_VERSION)
 endif
#==============================================================================

#==============================================================================
# Get the number of cores for fun stuff
#==============================================================================
  ifeq ($(OS), Win)
   CORES := 1
  else
   CORES := $$(($(shell nproc) * 2))
  endif
#==============================================================================

#==============================================================================
# Define default compilation rule
#==============================================================================
all: static shared
	date
#==============================================================================

#==============================================================================
# Define 'release' and 'debug' as alias for 'all'
#==============================================================================
release: static shared
debug: static shared
#==============================================================================

#==============================================================================
# Rule for building the static lib
#==============================================================================
static: MAKEDIRS $(BINDIR)/$(TARGET).a
#==============================================================================

#==============================================================================
# Rule for building the shared libs
#==============================================================================
shared: MAKEDIRS $(BINDIR)/$(TARGET).$(MNV)
#==============================================================================

#==============================================================================
# Rule for building tests
#==============================================================================
tests: MAKEDIRS static $(TEST_BIN)
#==============================================================================

#==============================================================================
# Rule for installing the library
#==============================================================================
ifeq ($(OS), Win)
  install:
	# Compile release and debug binaries
	make release
	rm -f $(OBJS)
    make debug
	# Remove all debug info from the binaries
	strip $(BINDIR)/$(TARGET).a
	strip $(BINDIR)/$(TARGET).$(MNV)
	# Create destiny directories
	mkdir -p /c/fraction/lib/
	mkdir -p /c/fraction/include/GFrame
	# Copy every shared lib (normal and debug)
	cp -f $(BINDIR)/$(TARGET)*.$(MNV) /c/fraction/lib
	# -P = don't follow sym-link
	cp -fP $(BINDIR)/$(TARGET)*.$(MJV) /c/fraction/lib
	cp -fP $(BINDIR)/$(TARGET)*.$(SO) /c/fraction/lib
	# Copy the headers
	cp -rf ./include/fraction/* /c/GFraMe/include/GFrame
else
  install:
	# Compile release and debug binaries
	make release
	rm -f $(OBJS)
	make debug
	# Remove all debug info from the binaries
	strip $(BINDIR)/$(TARGET).a
	strip $(BINDIR)/$(TARGET).$(MNV)
	# Create destiny directories
	mkdir -p $(LIBPATH)/fraction
	mkdir -p $(HEADERPATH)/fraction
	# Copy every shared lib (normal and debug)
	cp -f $(BINDIR)/$(TARGET)*.$(MNV) $(LIBPATH)/fraction
	# -P = don't follow sym-link
	cp -fP $(BINDIR)/$(TARGET)*.$(MJV) $(LIBPATH)/fraction
	cp -fP $(BINDIR)/$(TARGET)*.$(SO) $(LIBPATH)/fraction
	# Copy the static lib
	cp -f $(BINDIR)/$(TARGET).a $(LIBPATH)/fraction
	# Copy the headers
	cp -rf ./include/fraction/* $(HEADERPATH)/fraction
	# Make the lib be automatically found
	echo "$(LIBPATH)/fraction" > /etc/ld.so.conf.d/fraction.conf
	# Update the paths
	ldconfig
endif
#==============================================================================

#==============================================================================
# Rule for uninstalling the library
#==============================================================================
ifeq ($(OS), Win)
  uninstall:
	# Remove the libraries
	rm -f /c/fraction/lib/$(TARGET)_dbg.$(MNV)
	rm -f /c/fraction/lib/$(TARGET)_dbg.$(MJV)
	rm -f /c/fraction/lib/$(TARGET)_dbg.$(SO)
	rm -f /c/fraction/lib/$(TARGET).$(MNV)
	rm -f /c/fraction/lib/$(TARGET).$(MJV)
	rm -f /c/fraction/lib/$(TARGET).$(SO)
	# Remove the headers
	rm -rf /c/fraction/include/*
	# Remove its directories
	rmdir /c/fraction/lib/
	rmdir /c/fraction/include/
	rmdir /c/fraction/
else
  uninstall:
	# Remove the libraries
	rm -f $(LIBPATH)/fraction/$(TARGET)_dbg.$(MNV)
	rm -f $(LIBPATH)/fraction/$(TARGET)_dbg.$(MJV)
	rm -f $(LIBPATH)/fraction/$(TARGET)_dbg.$(SO)
	rm -f $(LIBPATH)/fraction/$(TARGET).$(MNV)
	rm -f $(LIBPATH)/fraction/$(TARGET).$(MJV)
	rm -f $(LIBPATH)/fraction/$(TARGET).$(SO)
	rm -f $(LIBPATH)/fraction/$(TARGET).a
	# Remove the headers
	rm -rf $(HEADERPATH)/fraction/*
	# Remove its directories
	rmdir $(LIBPATH)/fraction
	rmdir $(HEADERPATH)/fraction
	# Remove the lib from the default path
	rm /etc/ld.so.conf.d/fraction.conf
	# Update the paths
	ldconfig
endif
#==============================================================================

#==============================================================================
# Rule for actually building the static library
#==============================================================================
$(BINDIR)/$(TARGET).a: $(OBJS)
	rm -f $(BINDIR)/$(TARGET).a
	ar -cvq $(BINDIR)/$(TARGET).a $(OBJS)
#==============================================================================

#==============================================================================
# Rule for actually building the shared library
#==============================================================================
ifeq ($(OS), Win)
  $(BINDIR)/$(TARGET).$(MNV): $(OBJS)
	rm -f $(BINDIR)/$(TARGET).$(MNV)
	$(CC) -shared -Wl,-soname,$(TARGET).$(MJV) -Wl,-export-all-symbols \
	    $(CFLAGS) -o $(BINDIR)/$(TARGET).$(MNV) $(OBJS) $(LFLAGS)
else
  $(BINDIR)/$(TARGET).$(MNV): $(OBJS)
	rm -f $(BINDIR)/$(TARGET).$(MNV) $(BINDIR)/$(TARGET).$(SO)
	$(CC) -shared -Wl,-soname,$(TARGET).$(MJV) -Wl,-export-dynamic \
	    $(CFLAGS) -o $(BINDIR)/$(TARGET).$(MNV) $(OBJS) $(LFLAGS)
	cd $(BINDIR); ln -f -s $(TARGET).$(MNV) $(TARGET).$(MJV)
	cd $(BINDIR); ln -f -s $(TARGET).$(MJV) $(TARGET).$(SO)
endif
#==============================================================================

#==============================================================================
# Rule for compiling any .c in its object
#==============================================================================
$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<
#==============================================================================

#==============================================================================
# Build a emscript (LLVM) binary, to be used when compiling for HTML5
#==============================================================================
$(BINDIR)/$(TARGET).bc: MAKEDIRS $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(OBJS)
#==============================================================================

#==============================================================================
# Rule for creating every directory
#==============================================================================
MAKEDIRS: | $(OBJDIR)
#==============================================================================

#==============================================================================
# Rule for actually creating every directory
#==============================================================================
$(OBJDIR):
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)
#==============================================================================

#==============================================================================
# Removes all built objects (use emscript_clean to clear the emscript stuff)
#==============================================================================
clean:
	rm -f $(OBJS)
	rm -f $(BINDIR)/$(TARGET)*.$(MJV)
	rm -f $(BINDIR)/$(TARGET)*.$(MNV)
	rm -f $(BINDIR)/$(TARGET)*.$(SO)
	rm -f $(BINDIR)/$(TARGET)*
#==============================================================================

#==============================================================================
# Remove all built objects and target directories
#==============================================================================
distclean: clean
	rm -f $(OBJDIR)/*
	rm -f $(BINDIR)/*
	rmdir $(OBJDIR)
	rmdir $(BINDIR)
	rmdir obj/
	rmdir bin/
#==============================================================================

