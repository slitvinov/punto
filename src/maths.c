/*****************************************************************************
**  This is part of the punto program
**  Copyright (C) 1998-2009  Mariano Revenga
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
#include <math.h>
#include "punto.h"
#include "maths.h"


void TCR(float a,float b, float c, long n1, long n2,
	 struct Point r,float comp,short field,int *reset_box,struct Punto *p){
  /* 
     rotate, traslate and scale all puntos in *p writing the result in *p1.
     a,b,c are the angles of rotation,
     n1,n2 the range number of puntos.
     p the puntos to rotate
     r the translation vector.
     comp is the scale factor.
     reset_box to initialize variables
     field if we are rotating a vectorial field 
  */

  long i,j,k;
  static float A0[3][3];
  float A1[3][3];
  float temp[3];
  float cosa,sina,cosb,sinb,cosc,sinc;

  /* initialitation */

  if(*reset_box==TRUE){
    *reset_box=FALSE;
    
    A0[0][0]=1;
    A0[0][1]=0;
    A0[0][2]=0;
    
    A0[1][0]=0;
    A0[1][1]=1;
    A0[1][2]=0;
    
    A0[2][0]=0;
    A0[2][1]=0;
    A0[2][2]=1;    
  }

    cosa=cosb=cosc=1.;
    sina=sinb=sinc=0.;
    
    if(a!=0){cosa=(float)cos(a);sina=(float)sin(a);} 
    if(b!=0){cosb=(float)cos(b);sinb=(float)sin(b);} 
    if(c!=0){cosc=(float)cos(c);sinc=(float)sin(c);} 
    
    A1[0][0]=cosb*cosc; 
    A1[0][1]=cosb*sinc; 
    A1[0][2]=sinb; 
    
    A1[1][0]=-sina*sinb*cosc-cosa*sinc; 
    A1[1][1]=-sina*sinb*sinc+cosa*cosc; 
    A1[1][2]=sina*cosb; 
    
    A1[2][0]=-cosa*sinb*cosc+sina*sinc; 
    A1[2][1]=-cosa*sinb*sinc-sina*cosc; 
    A1[2][2]=cosa*cosb; 
    
    /* matrix mult  A0=A1*A0*/
    for(i=0;i<3;i++){ 
      for(j=0;j<3;j++){ 
	temp[j]=0.0;
	for(k=0;k<3;k++)temp[j]+=A1[j][k]*A0[k][i];
      }
      for(k=0;k<3;k++)A0[k][i]=temp[k];
    }
    
    /* reescaling A1=A0*comp */
    for(i=0;i<3;i++){ 
      for(j=0;j<3;j++){ 
	A1[i][j]=A0[i][j]*comp;
      }
    }
    
    /* rotation  + traslation */
    
    for (i=n1;i<n2;i++){
      p[i].x1=(A1[0][0]*(p[i].x-r.x)+A1[0][1]*(p[i].y-r.y)+A1[0][2]*(p[i].z-r.z)); 
      p[i].y1=(A1[1][0]*(p[i].x-r.x)+A1[1][1]*(p[i].y-r.y)+A1[1][2]*(p[i].z-r.z)); 
      p[i].z1=(A1[2][0]*(p[i].x-r.x)+A1[2][1]*(p[i].y-r.y)+A1[2][2]*(p[i].z-r.z)); 
    }
    
    if(field==TRUE){
      
      /* rotation */
      for (i=n1;i<n2;i++){
	p[i].vx1=((A1[0][0]*(p[i].vx)+A1[0][1]*(p[i].vy)+A1[0][2]*(p[i].vz)))/comp; 
	p[i].vy1=((A1[1][0]*(p[i].vx)+A1[1][1]*(p[i].vy)+A1[1][2]*(p[i].vz)))/comp; 
	p[i].vz1=((A1[2][0]*(p[i].vx)+A1[2][1]*(p[i].vy)+A1[2][2]*(p[i].vz)))/comp; 
      }
    }
}  /* -- funcion TCR3 */

void Rapida( long *a, int izq,int drch,struct Punto *p)
     /* function Rapida:
	sort puntos from low to high in z axis value,
	the ordered indexes are returned in the vector a
     */ 
{
  long i,j,k;
  double z;
  if(drch<=izq)return;
  i=izq;
  j=drch;
  
  z=p[a[(izq+drch)/2]].z1;
  do{
    while(p[a[i]].z1 < z && i < drch)i++;
    while(z < p[a[j]].z1 && j > izq)j--;
    if(i <= j){
      k=a[i];
      a[i]=a[j];
      a[j]=k;
      i++;j--;
    }
  }while (i <= j);
  if(izq < j) Rapida(a,izq,j,p);
  if(i < drch) Rapida(a,i,drch,p);
}  /* --funcion Rapida */


void Dec2Hex(long n,char *a){
  /*
    return in the char string a the HEX value of the integer number n.
   */
  char *num[16]={"0","1","2","3","4","5","6","7","8","9","a","b","c","d","e","f"};

  long i,j;

  while(n>255)n/=256;

  strcpy(a,"");

  i=n/16;
  j=n%16;
  strncat(a,num[i],1);
  strncat(a,num[j],1);

} 

double aleatorio(double a){
  /*  
      return a random value between 0 and a.
  */
  
  double b;
  b=((double)rand()/RAND_MAX*a);
  return b;
}


