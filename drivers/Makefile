# Makefile for a custom application comprised of 
# a loable kernel module and a user space program
# (or a set of related modules and user space programs).
# If your application doesn't need to have a kernel module or
# a user space program, edit the two goals below to
# exclude one or the other. 
all		: modules
clean		: clean_modules

# Edit the line below to modify a set of user-space programs
# you need to build 
#APPS		= app
#apps		: $(APPS)	

# These are flags/tools used to build user-space programs
CFLAGS		:= -Os -mcpu=cortex-m3 -mthumb -Wall
LDFLAGS		:= -mcpu=cortex-m3 -mthumb -Wall
CC		= $(CROSS_COMPILE_APPS)gcc

# Clean-up after user-space programs
#clean_apps	:
#	-rm -f $(APPS) *.gdb *.o

# Edit the line below to modify a set of loadable modules
# you need to build 
obj-m		+= minim.o sample2.o radmonc.o ad7417.o sysctrl.o

# Define dependencies for a particular module
# sample-y	:= some.o
sysctrl-y	:= mss_sys_services/mss_sys_services.o mss_sys_services/mss_comblk.o

#mss_comblk.o	: mss_sys_services/mss_comblk.c mss_sys_services/mss_comblk.h mss_sys_services/m2sxxx.h
#		  $(CC) -c mss_sys_services/mss_comblk.c $(CFLAGS)
#mss_sys_services.o : mss_sys_services/mss_sys_services.c mss_sys_services/mss_sys_services.h mss_sys_services/mss_comblk.h mss_sys_services/mss_assert.h
#		  $(CC) -c mss_sys_services/mss_sys_services.c $(CFLAGS)
#sysctrl.o	:=	mss_comblk.o mss_sys_services.o
#		  $(CC) 

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

