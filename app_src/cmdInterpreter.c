// $Id: cmdInterpreter.c,v 1.26 2007/03/17 00:01:16 richter Exp $

/************************************************************************
**
**
** This file is property of and copyright by the Experimental Nuclear 
** Physics Group, Dep. of Physics and Technology
** University of Bergen, Norway, 2004
** This file has been written by Matthias Richter,
** Matthias.Richter@ift.uib.no
**
** Permission to use, copy, modify and distribute this software and its  
** documentation strictly for non-commercial purposes is hereby granted  
** without fee, provided that the above copyright notice appears in all  
** copies and that both the copyright notice and this permission notice  
** appear in the supporting documentation. The authors make no claims    
** about the suitability of this software for any purpose. It is         
** provided "as is" without express or implied warranty.                 
**
*************************************************************************/

#include <stdio.h>
#include <string.h>
//#include "memoryguard.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include "cmdInterpreter.h"

int printHelp()
{
  printf("  all parameters are hex values preceeded by a '0x' or decimal numbers\n");
  printf("  some of the commands have a short version, in most cases the first letter\n");
//  printf("  quit                   : q\n");
//  printf("  info                   : i\n");
  printf("  driver info            : d\n");
//  printf("  repeat previous command: p\n");
  printf("  single read rcu bus memory   : r[ead] 0x<address> (e.g.: r 0x7000)\n");
  printf("  multiple read rcu bus memory : r[ead] 0x<address> <dec no> (e.g.: r 0x7000 16)\n");
//  printf("                                 see 'hr\' for details and further options\n");
  printf("  single write rcu bus memory  : w[rite] 0x<address> 0x<data>\n");
  printf("                                 (e.g.: w 0x6800 0x34)\n");
//  printf("  multiple write with const    : w[rite] [-s,--swap] 0x<address> <dec no> 0x<data>\n");
//  printf("                                 (e.g.: w 0x6800 12 0x0a)\n");
/*  printf("  write file to rcu bus memory : w[rite] [-s,--swap] 0x<address> <fspec> 'filepath' <count> \n");
  printf("     optional format spec: -b4(default) -b2 -b1 binary 32,16,8 bit\n");
  printf("                           -b10 binary compressed 10 bit, -c ascii\n");
  printf("     optional number after the filepath specifies count for partially write\n");
  printf("     e.g.: w 0x6800 -c 'pedestal.dat' 512,  w 0x7000 'pgm.dat'\n");
*/
  printf("  sending a single command     : c 0x<address> (translated to w <address> 0x0\n");
/*
  printf("  check status of mem location : ? 0x<address> [[c,f] 0x<bitmask> 0x<pattern>]\n");
  printf("                                               [t n s(ec)/u(sec)]\n");
*/
  printf("  batch processing             : b[atch] 'filepath' [[-l] <count>,-i]\n");
/*
  printf("                                 see \'hb\' for details and further options\n");
  printf("  wait command                 : wait <n> s(ec)/u(sec)\n");
  printf("  log message                  : e[cho] [-o,-a <filename>] <message>\n");
  printf("     the message can contain a \'-t\' specifier to print a timestamp\n\n");
*/
//  printf("  flash memory commands        : flash <cmd> (try \'flash help\')\n");
//  printf("  selectmap commands           : sm[selectmap] <cmd> (try \'sm help\')\n");
//  printf("  firmware commands            : fw[firmware] <cmd> (try \'fw help\')\n");
//  printf("  read bus control register    :rcr[read-ctrlreg]\n");
  //printf("  check rcu bus memory: m 0x<address> 0x<size in byte>\n");
  //printf("  read from file\n");
//  printf("  -/+ profile: switch on/off profiling\n");
//  printf("  - turn off all messages\n");
//  printf("  + turn on default messages\n");
//  printf("  hd debug message info\n");
//  printf("  hr detailed help for the read-command\n");
  return 0;
}

