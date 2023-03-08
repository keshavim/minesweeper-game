#
# 'make'        build executable file 'main'
# 'make clean'  removes all .o and executable files
#

# define the C compiler to use
CC = clang

# define any compile-time flags
CFLAGS	:= -Wall -Wextra -Wpedantic -g -O0 -MMD -MP

PCH 	:= include/stdafx.h

ifeq ($(CC),clang)
	CFLAGS	+= -Wdangling 
	IncPCH		:= -include-pch $(PCH).pch
else ifeq ($(CC),gcc)
	IncPCH 	:= 
endif


# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
LFLAGS = -ldl -lpthread -lm -lglad -lglfw3 -lcglm

# define output directory
OUTPUT	:= output

# define source directory
SRCDIR		:= src

#define object and dependency directory
OBJDIR		:= $(OUTPUT)/.obj

# define include directory
INCLUDEDIR	:= include

# define lib directory
LIB		:= lib

ifeq ($(OS),Windows_NT)
	MAIN	:= main.exe
	SOURCEDIRS	:= $(SRCDIR)
	LIBDIRS		:= $(LIB)
	FIXPATH = $(subst /,\,$1)
	RM			:= del /q /f
	MD	:= mkdir
else
	MAIN	:= main
	SOURCEDIRS	:= $(shell find $(SRCDIR) -type d)
	LIBDIRS		:= $(shell find $(LIB) -type d)
	FIXPATH = $1
	RM = rm -rdf
	MD	:= mkdir -p
endif

# define any directories containing header files other than /usr/include
INCLUDES	:= $(patsubst %,-I%, $(INCLUDEDIR:%/=%))

# define the C libs
LIBS		:= $(patsubst %,-L%, $(LIBDIRS:%/=%))

# define the C source files
SOURCES		:= $(wildcard $(patsubst %,%/*.c, $(SOURCEDIRS)))

# define the C object files 
OBJECTS		:= $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

DEPS		:= $(OBJECTS:.o=.d)



#
# The following part of the makefile is generic; it can be used to 
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

OUTPUTMAIN	:= $(call FIXPATH,$(OUTPUT)/$(MAIN))

-include $(DEPS)



all: $(OUTPUT) $(MAIN)
	@echo Executing 'all' complete!

$(OUTPUT):
	$(MD) $(OUTPUT)

$(MAIN): $(OBJECTS) 
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJECTS) -o $(OUTPUTMAIN) $(LFLAGS) $(LIBS)

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
$(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(MD) $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES)  -c $< -o $@


pch:
	$(CC) -O2 -x c-header $(PCH) -o $(PCH).pch

.PHONY: clean
clean:
	$(RM) $(OUTPUT)
	@echo Cleanup complete!

run: all
	./$(OUTPUTMAIN)
	@echo Executing 'run: all' complete!
