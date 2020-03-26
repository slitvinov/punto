/*****************************************************************************
 **  This is part of the punto program
 **  Copyleft (C) 1998-2009  Mariano Revenga
 **
 **  This program is free software; you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License (version 2) as
 **  published by the Free Software Foundation.
 **
 **  This program is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with this program; if not, write to the Free Software
 **  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ******************************************************************************/


/* $Revision: 1.3 $
 */
#include <string.h>
#include <linux/kernel.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>

#include "punto.h"
#include "system.h"

extern struct Options option;
extern int actualizar;

void sigalrm_handler(){}

void duerme(unsigned long tiempo){
  /* funtion duerme
     delay between frames in centiseconds
  */
  
  usleep(10000*tiempo);
}/* --funcion duerme */

void duerme_old(unsigned long tiempo)
     /* funcion duerme
	detiene la ejecucion en tiempo centisegundos
     */
{
  struct itimerval temporizador,temporizador_ant;
  signal(SIGALRM,sigalrm_handler);
  temporizador.it_value.tv_sec=0;
  temporizador.it_value.tv_usec=tiempo*10000;
  temporizador.it_interval.tv_sec=0;
  temporizador.it_interval.tv_usec=0;

  setitimer(ITIMER_REAL,&temporizador,&temporizador_ant);
  pause();  
}/* --funcion duerme */


void print_sysinfo(void){ 
  /*
    Show memory system information
    only for improve and debug reasons
   */
  struct sysinfo info; 

  sysinfo(&info); 

  printf(" Seconds since boot %ld\n1, 5, and 15 minute load averages %lu %lu %lu\n           Total usable main memory size %lu\n                    Available memory size %lu\n                            Amount of shared memory %lu\n                 	 Memory used by buffers %lu\n                 	 Total swap space size %lu\n                   	 swap space still available %lu\n              	 Number of current processes %u\n             	 Pads structure to 64 bytes %s\n   ",          
	 info.uptime,              
	 info.loads[0],info.loads[1],info.loads[2],   
	 info.totalram/1024,   
	 info.freeram/1024,    
	 info.sharedram/1024,  
	 info.bufferram/1024,  
	 info.totalswap/1024,  
	 info.freeswap/1024,   
	 info.procs,     
	 info._f);            



//  long uptime;              /* Seconds since boot */
//  unsigned long loads[3];   /* 1, 5, and 15 minute load average s */
//  unsigned long totalram;   /* Total usable main memory size */
//  unsigned long freeram;    /* Available memory size */
//  unsigned long sharedram;  /* Amount of shared memory */
//  unsigned long bufferram;  /*  Memory used by buffers */
//  unsigned long totalswap;  /* Total swap space size */
//  unsigned long freeswap;   /* swap space still available */
//  unsigned short procs;     /* Number of current processes */
//  char _f[22];              /* Pads structure to 64 bytes */            
} 

/***
    funcion: sigfpe_handler
    descripcion: rutina de tratamiento de la senal SIGFPE.
****/

void sigfpe_handler(int sig){
  printf("\nSignal recived: %d. Floating point error.\n",sig);
  exit(-1);
}

void sigint_handler(int sig){
 
  option.signal=TRUE;
  actualizar=TRUE;
  //  EXIT=TRUE;
}

void sigterm_handler(int sig){
  
  option.signal=TRUE;
  actualizar=TRUE;
  // EXIT=TRUE;
}
void sigquit_handler(int sig){
  
  option.signal=TRUE;
  actualizar=TRUE;
  // EXIT=TRUE;
}

void TakeEnv(char **envp,char *string,char *var){
  /* 
     copy in string the enviroment variable var
   */

  int sw=0;
  size_t len;
  char **env;
  env=envp;
  len=strlen(var);
  while (*env){
    if(strncmp(*env,var,len)==0){
      strncpy(string,&(*env)[len+1],(size_t)128);
      sw=1;
    }
    env++;
  }
  if(sw==0)strcpy(string,"\0");
}

