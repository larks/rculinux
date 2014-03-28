// $Id: cmdInterpreter.h,v 1.5 2005/11/01 14:27:49 richter Exp $

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

#ifndef _CMDINTERPRETER_H
#define _CMDINTERPRETER_H

#include <stdint.h>
#include <stdlib.h>

/* RCU2 register access driver name */
//const char * dev_name = "/dev/sample2";

int printHelp();
uint32_t registerAccess(uint32_t, uint32_t, char *);
void executeCommands(uint32_t arg_count, char **arguments, FILE * fp);
unsigned int parseBinary(char *str);
unsigned int parseNumber(char *str);
//int printInfo();

#endif // _CMDINTERPRETER_H
