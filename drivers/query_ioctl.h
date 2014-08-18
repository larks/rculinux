#ifndef QUERY_IOCTL_H
#define QUERY_IOCTL_H
#include <linux/ioctl.h>

/* This is probably not good */
#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif
/* Define ISP modes */
#define AUTHENTICATE 0x1
#define VERIFY       0x2
#define PROGRAM      0x3

/* Not used for the moment */
typedef struct
{
	uint8_t serial_number[16];
	uint8_t user_code[4];
	uint8_t design_version[2];
} query_arg_t;

/* 'q' is MAJOR_NUM, where from? */ 
#define READ_IDCODE _IOR('q', 1, query_arg_t *)
#define READ_USERCODE _IOR('q', 2, query_arg_t *)
#define PROG_AUTHENTICATE _IOWR('q', 3, query_arg_t *)
#define PROG_VERIFY _IOWR('q', 4, query_arg_t *)
#define PROG_PROGRAM _IOWR('q', 5, query_arg_t *)
//#define QUERY_CLR_VARIABLES _IO('q', 2)
//#define QUERY_SET_VARIABLES _IOW('q', 3, query_arg_t *)

#endif
