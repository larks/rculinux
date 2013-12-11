# Makefile

all		: rcu-sh
clean		: clean_apps

# Edit the line below to modify a set of user-space programs
# you need to build 

DEPS = cmdInterpreter.h
#rcu_sh_SRC	= RCU_shell.c cmdInterpreter.c
rcu_sh_OBJ	= RCU_shell.o cmdInterpreter.o
RCU_shell.o	: RCU_shell.c $(DEPS)
		  $(CC) -c RCU_shell.c $(CFLAGS)
cmdInterpreter.o : cmdInterpreter.c $(DEPS)
		  $(CC) -c cmdInterpreter.c $(CFLAGS)
rcu-sh		: $(rcu_sh_OBJ)
		  $(CC) $(LDFLAGS) -o rcu-sh $(rcu_sh_OBJ)


# These are flags/tools used to build user-space programs
CFLAGS		:= -Os -mcpu=cortex-m3 -mthumb -Wall
LDFLAGS		:= -mcpu=cortex-m3 -mthumb -Wall
CC		= $(CROSS_COMPILE_APPS)gcc

# Clean-up after user-space programs
clean_apps	:
	-rm -f $(APPS) *.gdb *.o

# Edit the line below to modify a set of loadable modules
# you need to build 
#obj-m		+= sample.o

# Define dependencies for a particular module
# sample-y	:= some.o

# Path to the kernel modules directory in context of which
# these loadable modules are built
KERNELDIR	:=  $(INSTALL_ROOT)/linux

# Build the modules in this directory
modules		:
	make -C $(KERNELDIR) M=`pwd` modules CFLAGS="" LDFLAGS=""

# Clean-up after modules
clean_modules:
	make -C $(KERNELDIR) M=`pwd` clean
	rm -f modules.order

