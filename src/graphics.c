#include "graphics.h"

int LookUpColor(char *name,struct RGBColor *table,SDL_Color *color){
  /* look for a color name name into the table table returning 
     the rgb values in color 
*/
  int i;
  int status=-1;

  for(i=0;i<MAX_NCOLORS;i++){
    if(strncmp(name,table[i].name,MAX_WORD_LEN)==0){
      color->r=table[i].r;
      color->g=table[i].g;
      color->b=table[i].b;
      status=0;
      break;
    }
  }
  return(status);
}


int Led1(SDL_Surface *screen,Uint32 color,int x,int y,int n){
  /*    
	Draw in the x,y position the number n as a led  with color color 

        4
	--
     8 | 1| 2
	-- 
    16 |  | 64
	--
	32
  */

  int x0,x1,y0,y1;
  int i,j,k,n0;
  unsigned int number;
  unsigned int mask[10]={126,66,55,103,75,109,125,70,127,79};
  int num[10];
  int temp;
  int size=FONTSIZE;
  
  for(k=0;k<10;k++){
    n0=n%10;
    num[k]=n0;
    n=(int)n/10;
    if(n<=0)break;
  } /* for */
  /* flip */
  k++;
  for(i=0;i<k/2;i++){
    temp=num[i];
    num[i]=num[k-1-i];
    num[k-1-i]=temp;
  }
  
  x0=x1=y0=y1=0;

  for(j=0;j<k;j++){
    number=mask[num[j]];
    for(i=0;i<=6;i++){
      if(number & 1){
	switch(i){
	case 0:
	  x0=1;
	  y0=size+2;
	  x1=x0+size;
	  y1=size+2;
	  break;
	case 1:
	  x0=size+2;
	  y0=1;
	  x1=size+2;
	  y1=y0+size;
	  break;
	case 2:
	  x0=1;
	  y0=0;
	  x1=x0+size;
	  y1=0;
	  break;
	case 3:
	  x0=0;
	  y0=1;
	  x1=0;
	  y1=y0+size;
	  break;
	case 4:
	  x0=0;
	  y0=size+3;
	  x1=0;
	  y1=y0+size;
	  break;
	case 5:
	  x0=1;
	  y0=2*(size+1)+2;
	  x1=x0+size;
	  y1=2*(size+1)+2;
	  break;
	case 6:
	  x0=size+2;
	  y0=size+3;
	  x1=size+2;
	  y1=y0+size;
	  break;
	default:
	  printf("error en led\n");
	  exit(EXIT_FAILURE);
	}
	DrawLine(screen,  
		  x+x0+j*(6+size),y+y0,x+x1+j*(6+size),y+y1,color);
	
      }
      number>>=1;
    }
  } /* for j */
  return(k);
} /* --led */


void ReadRGBColors(char *fname,struct RGBColor *color,int n){
  /*
    Reads n rgb names and rgb values from the file fname
    writes the result in the struct color
   */
  FILE *fp;
  int i;
  int r,g,b; 
  char name[MAX_WORD_LEN]="";
  int status;
  size_t len;

  if((fp=fopen(fname,"rt"))==NULL){
    perror(fname);
    exit(EXIT_FAILURE);
  }
  for(i=0;i<n;i++){
    status= fscanf(fp,"%d%d%d%s",&r,&g,&b,name);

    color[i].r=(Uint8)r;
    color[i].g=(Uint8)g;
    color[i].b=(Uint8)b;
    strncpy(color[i].name,name,MAX_WORD_LEN);
    if(status==0){
      i--;
      fscanf(fp,"%s",name);
      strcat(color[i].name," ");
      len=strlen(name);
      if(len>MAX_WORD_LEN-1)len=MAX_WORD_LEN-1;
      strncat(color[i].name,name,len);
    }
  }
  fclose(fp);
}

void DrawArrow(SDL_Surface *screen,SDL_Rect pos,struct Point f,float factor,Uint32 color,int type){
  /*
    draw an arrow in the position pos
    with size f and reescale by the factor factor
    with the color color and
    the type type.
    types can be:
      0: a point
      1: a line
      2: an arrow
   */

  float sin_O1,cos_O1;
  float vx,vy;
  float r,x,y,x1,y1,x2,y2,x3,y3;
  float ang1;
  static float sin_O2=.8660254038;   //  sin_O2=sin(PI/3.);
  static float cos_O2=.5;  //     cos_O2=cos(PI/3.);
  static float sin_O3=-.8660254038;   //  sin_O3=sin(4.*PI/3.);
  static float  cos_O3=-.5; //cos(4.*PI/3.);
  
  vx=f.x;
  vy=f.y;
  
  x=(float)(pos.x) - vx*factor; 
  y=(float)(pos.y) + vy*factor ;
  
  x1=(float)(pos.x) + vx*factor ;
  y1=(float)(pos.y) - vy*factor ;
  
  
  switch(type){  
  case 0:
    PutPixel(screen,(int)((x+x1)*.5),(int)((y+y1)*.5),color);
    break;
  case 1:
    DrawLine(screen,(int)x,(int)y,(int)x1,(int)y1,color);
    break;
  case 2:
    DrawLine(screen,(int)x,(int)y,(int)x1,(int)y1,color);
    ang1=atan2(y1-y,x1-x);
    r=.5*sqrt((x-x1)*(x-x1)+(y-y1)*(y-y1));
    
    cos_O1=(float)cos(ang1);
    sin_O1=(float)sin(ang1);
    
    x2=-r*sin_O2;
    y2=-r*cos_O2;
    
    x3=x2*cos_O1-y2*sin_O1;
    y3=x2*sin_O1+y2*cos_O1;
    
    DrawLine(screen,(int)x1,(int)y1,(int)(x1+x3),(int)(y1+y3),color);    

    x2=r*sin_O3;
    y2=-r*cos_O3;
    x3=x2*cos_O1-y2*sin_O1;
    y3=x2*sin_O1+y2*cos_O1;

    DrawLine(screen,(int)x1,(int)y1,(int)(x1+x3),(int)(y1+y3),color);    
    break;
  default:
    break;
  }  
}

void DrawFile(SDL_Surface *screen,Uint32 color,Uint32 border,struct Rectangle rec,struct DataFile df){
  /*
    draw a status bar of an open file *fp 
    in the position given by rec with color color
   */
  int i,len;
  long files,filep;

  files=SizeFile(df.name); // this function opens again the file. look for another way.
  filep=ftell(df.fp);
  len=(int)((rec.l-2)*(float)filep/files);


  DrawRectangle(screen,rec.x,rec.y,rec.x+rec.l,rec.y+rec.h,border);
  for(i=1;i<(rec.h) && len>0;i++){
    DrawLine(screen,rec.x+1,rec.y+i,rec.x+len,rec.y+i,color);
  }


} /* DrawFile */


void DrawScale(SDL_Surface *screen,Uint32 *color,Uint32 bordercolor,struct Values val,int x,int y)
{
  /*
    draw a cale bar 
    in the position given by x,y 
    with color color
    with border of color bordercolor.
   */
  int i,j;
  int x0,y0,x1,y1;
  int w,h;
  char num[MAX_WORD_LEN]="";
  float value;
  float inc;
  int sign=0;
  w=10;
  h=2*MAX_NSCOLORS;
  x0=x1=x-1;y0=y-1;y1=y+h;
  DrawLine(screen,x0,y0,x1,y1,bordercolor);
  x0=x1;y0=y1;
  x1=x+w+1;y1=y0;
  DrawLine(screen,x0,y0,x1,y1,bordercolor);
  x0=x1;y0=y1;
  x1=x+w+1;y1=y-1;
  DrawLine(screen,x0,y0,x1,y1,bordercolor);
  x0=x1;y0=y1;
  x1=x-1;y1=y-1;
  DrawLine(screen,x0,y0,x1,y1,bordercolor);
  inc=(val.max_c-val.min_c)/10;
  for (i=0;i<=10;i++){
    x0=x+w+2;y0=y+(float)(h*i)/10;
    x1=x0+3;y1=y0;
    DrawLine(screen,x0,y0,x1,y1,bordercolor);
    sign=0;
    value=(10.-i)*inc+val.min_c;
    if (fabs(value)<inc/1000)value=0;
    snprintf(num,MAX_WORD_LEN,"%g",value);
    DrawText(screen,num,x0+5,y0-6,bordercolor);
  }

  for(i=MAX_NSCOLORS-1,j=0;i>=0;i--,j+=2){
    DrawLine(screen,x,y+j,x+w,y+j,color[i]);
    DrawLine(screen,x,y+j+1,x+w,y+j+1,color[i]);
  }

} /* --DrawScale */

void  GenColorTable(SDL_Surface *screen,Uint32 *colortable,int type){
  /*
    Generates a color table of type type
    type can be:
    RED: 
    GREEN:
    BLUE:
    GREY:
    SPECTRUM:
   */
  SDL_Color color1,color2;  

  color1.unused=0;
  color2.unused=0;
    

  switch(type){
  case RED:
    color1.r=255;
    color1.g=255;
    color1.b=0;

    color2.r=255;
    color2.g=0;
    color2.b=0;
    
    GenMiniColorTable(screen,colortable,MAX_NSCOLORS,color1,color2);
  break;
  case GREEN:

    color1.r=0;
    color1.g=30;
    color1.b=0;

    color2.r=0;
    color2.g=255;
    color2.b=0;
    
    GenMiniColorTable(screen,colortable,MAX_NSCOLORS,color1,color2);
    break;
  case BLUE:
    color1.r=0;
    color1.g=0;
    color1.b=30;

    color2.r=0;
    color2.g=0;
    color2.b=255;
    
    GenMiniColorTable(screen,colortable,MAX_NSCOLORS,color1,color2);
    break;
  case GREY:
    color1.r=30;
    color1.g=30;
    color1.b=30;

    color2.r=255;
    color2.g=255;
    color2.b=255;
    
    GenMiniColorTable(screen,colortable,MAX_NSCOLORS,color1,color2);

    break;
  case SPECTRUM:
  default:
    {
    float inc_color;
    int cont;
    int fin;
    
    cont=0;
    inc_color  =(float)MAX_NSCOLORS/5.;

    color1.r=255;
    color1.g=0;
    color1.b=0;

    color2.r=255;
    color2.g=255;
    color2.b=0;

    fin=(int)inc_color;
    GenMiniColorTable(screen,colortable,fin,color1,color2);

    cont += fin;
    color1.r=255;
    color1.g=255;
    color1.b=0;

    color2.r=0;
    color2.g=255;
    color2.b=0;
    fin=(int)(2.*inc_color-cont);
    GenMiniColorTable(screen,&colortable[cont],fin,color1,color2);

    cont += fin;
    color1.r=0;
    color1.g=255;
    color1.b=0;

    color2.r=0;
    color2.g=255;
    color2.b=255;
    fin=(int)(3.*inc_color-cont);
    GenMiniColorTable(screen,&colortable[cont],fin,color1,color2);

    cont += fin;
    color1.r=0;
    color1.g=255;
    color1.b=255;

    color2.r=0;
    color2.g=0;
    color2.b=255;
    fin=(int)(4.*inc_color-cont);
    GenMiniColorTable(screen,&colortable[cont],fin,color1,color2);

    cont += fin;
    color1.r=0;
    color1.g=0;
    color1.b=255;

    color2.r=255;
    color2.g=0;
    color2.b=255;
    fin=(int)(5.*inc_color-cont);
    GenMiniColorTable(screen,&colortable[cont],fin,color1,color2);
    }
    break;
  }
}

void GenMiniColorTable(SDL_Surface *screen,Uint32 *ct ,int n,SDL_Color color1,SDL_Color color2){
  /* gen a gradient colortable of n elements between color1 and color2 */
  int i;
  float ir,ig,ib;
  Uint8 r,g,b;
  ir=(float)(color2.r-color1.r)/(float)n;
  ig=(float)(color2.g-color1.g)/(float)n;
  ib=(float)(color2.b-color1.b)/(float)n;

  for(i=0;i<n;i++){
    r=(Uint8)(color1.r + ir*i);
    g=(Uint8)(color1.g + ig*i);
    b=(Uint8)(color1.b + ib*i);
    ct[i]=SDL_MapRGB(screen->format,r,g,b);
  }
}

void WriteBitmap(SDL_Surface *bitmap,char *fpname){
/*
  write bitmap to a file 
  to the file name is concatenated a number that increase each time
*/
  static int save_cont=0;
  char num_file[5]="";
  char name[MAX_WORD_LEN]="";
  char number[5]="";

  strncpy(name,fpname,MAX_WORD_LEN);
  if(save_cont<10)strncat(number,"0",1);
  if(save_cont<100)strncat(number,"0",1);
  if(save_cont<1000)strncat(number,"0",1);
  snprintf(num_file,5,"%d",save_cont);
  strncat(number,num_file,strlen(num_file));
  // strncat(name,num_file,4);
  strncat(name,number,4);
  strncat(name,".bmp",4);

  if(SDL_SaveBMP(bitmap,name)==-1){  
    printf("Error saving bitmap\n");  
  }  

  if(save_cont<MAXNFILES)save_cont++; 
  else{ 
    fprintf(stdout,"Warning: maximun number of files reached.\n"); 
  } 
  //  save_cont++;
  printf("saved: %s\n",name);
}


void DrawBox( w,box,dim,c_v,z,color) 
     struct Window w;
     struct Punto *box;
     int dim;
     struct Point c_v;
     float z;// zoom
     Uint32 color;
{ 
  /*
    Draw a border box 

   */
  
  struct Punto p[8];
  int i,j,k;
  int m,n;
  float minz;
  int alto;
  int width,height;
  static int box_d[8][3]={
    {2,5,7},
    {3,4,6},
    {0,5,7},
    {1,4,6},
    {1,3,6},
    {0,2,7},
    {1,3,4},
    {0,2,5}
  };
  static int box_l[8][3]={
    {1,3,4},
    {0,2,5},
    {1,3,6},
    {0,2,7},
    {0,5,7},
    {1,4,6},
    {2,5,7},
    {3,4,6}
  };

  width=w.w;
  height=w.h;
  alto=height;
  k=0;
  minz=box[0].z1;

  for(i=0;i<8;i++){ 
    p[i].x=(box[i].x1)*z + c_v.x;
    p[i].y=(float)(alto-((box[i].y1)*z + c_v.y));
    
    if(box[i].z1 < minz){
      minz=box[i].z1;
      k=i;
    }
  } 
  if(dim <= 2){ 
    DrawLine(w.screen,    
	     (int)p[3].x,(int)p[3].y,(int)p[2].x,(int)p[2].y,color);  
    DrawLine(w.screen,    
	     (int)p[2].x,(int)p[2].y,(int)p[6].x,(int)p[6].y,color); 
    DrawLine(w.screen,
	     (int)p[6].x,(int)p[6].y,(int)p[7].x,(int)p[7].y,color);  
    DrawLine(w.screen,
	     (int)p[7].x,(int)p[7].y,(int)p[3].x,(int)p[3].y,color);  
  } 
  if(dim == 3){ 
    for(i=0;i<3;i++){ 
      m=box_d[k][i];
      for(j=0;j<3;j++){ 
  	n=box_l[m][j];
	DrawLine(w.screen,  
		 (int)p[m].x,(int)p[m].y,(int)p[n].x,(int)p[n].y,color);     
	
      } 
    }
  } 
  DrawLine(w.screen,(int)(width*.5-5),(int)(height*.5),(int)(width*.5+5),(int)(height*.5),color);
  DrawLine(w.screen,(int)(width*.5),(int)(height*.5-5),(int)(width*.5),(int)(height*.5+5),color);
  
  
} /*        --DrawBox  */


void InfoColors(struct RGBColor *rgbcolortable){
  int i;

  printf("Colors that can be used:\n");
  printf("format in RGB mode:\nnumberID: red gren blue name\n");
  for(i=0;i<MAX_NCOLORS;i++){
    printf("%d: %d %d %d %s\n",   
  	   i,(int)rgbcolortable[i].r,(int)rgbcolortable[i].g,(int)rgbcolortable[i].b,rgbcolortable[i].name);   
  }   
}
