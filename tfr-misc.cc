/*
In this module, various independent (separate) and 
useful functions are imtroduced. It is intended to 
use them in other classes of this project.
*/

/* heder with signatures */
#include "tfr-misc.hh"

/* ***************************************************************/
/* for diagnostic */

void tf_print_info(){

  /* print info about this project */

  fprintf(stderr, "==============================================\n");
  fprintf(stderr, "       _______ ______ _____  \n");
  fprintf(stderr, "      |__   __|  ____|  __ \\ \n");
  fprintf(stderr, "  ___ ___| |  | |__  | |__) |\n");
  fprintf(stderr, " / __/ __| |  |  __| |  _  / \n");
  fprintf(stderr, "| (_| (__| |  | |    | | \\ \\ \n");
  fprintf(stderr, " \\___\\___|_|  |_|    |_|  \\_\\ v. %s\n\n", TFR_VERSION);
  fprintf(stderr, "==============================================\n");
}


void call_error(const char* a_short_info){

  /* this function shoud be called 
     if an error is detected */

  fprintf(stderr, "(error) %s\n", a_short_info);
  /* TODO: save this msg to a logfile */
  
  exit(EXIT_FAILURE);
}

void call_warning(const char* a_short_info){

  /* this function shoud be called 
     if a warning is detected */

  fprintf(stderr, "(warning) %s\n", a_short_info);
  /* TODO: save this msg to a logfile */
}

void call_info(const char* a_short_info){

  /* this function shoud be called 
     if aa info should be printed */

  fprintf(stderr, "(info) %s\n", a_short_info);
  /* TODO: save this msg to a logfile */
}

/* ********************************************************** */

void progress_bar(int a_curr, int a_len, const char *a_info){

  /* The function prints progress in text mode. 
     It can be useful for long calculation for 
     monitoring its progress. */

#ifdef PG_BAR

  if(a_curr % (1+(int)(a_len / PG_BAR) )) 
    return;
  for(int p=0;p<PG_BAR+3+strlen(a_info);p++) 
    fprintf(stderr,"\b");

  fprintf(stderr,"[");
  if(a_len>0){
    float progress = (float)a_curr / a_len;
    for(int p=0;p<PG_BAR;p++){
      if(p<progress*PG_BAR)
	fprintf(stderr,"=");
      else 
	fprintf(stderr," ");
    }
    fprintf(stderr,"] %s", a_info);
    fflush(stderr);
  }
  else{
    for(int p=0;p<PG_BAR;p++)
      fprintf(stderr,"=");    
    fprintf(stderr,"] %s\n", a_info);
  }

#endif
}

/* ********************************************************** */
/* for calculations */

double rand_gauss(double a_sigma){

  /* It returns a single random sample 
     with the normal distribution of probability.     
     Its origin is the Box-Muller transformation.
  */
  
  double u1 = (double)rand() / RAND_MAX;
  if (u1 > 0.99999999999)
    u1 = 0.99999999999;
  double u2 = a_sigma * sqrt( 2.0 * log( 1.0 / (1.0 - u1) ) );
  
  u1 = (double)rand() / RAND_MAX;
  if (u1 > 0.99999999999) 
    u1 = 0.99999999999;
  
  return (double) (u2 * cos(2 * M_PI * u1));
}

int calc_factorial(int a_number){

  /* It returns a factorial value of the argument. */
  
  if(a_number<0) 
    call_error("factorial < 0");
  if (a_number == 1 || a_number == 0) return 1;
  else return calc_factorial(a_number - 1) * a_number;
}

/* ********************************************************** */

double calc_binominal (int a_kval, int a_ival){

  /* Its returns a binominal coefficient. */

  double denominativ = calc_factorial(a_ival) * calc_factorial(a_kval-a_ival);
  double nominativ = calc_factorial(a_kval); 
  return nominativ / denominativ;
}

double calc_eulerian (int a_kval, int a_ival){

  /* It returns an Eulerian number. */

  double y = 0.0;
  for (int m=0; m<=a_ival; m++)
    y +=  pow(-1.0, m) * calc_binominal(a_kval+1, m) * pow(a_ival-m, a_kval);  
  return y;
}

/* ********************************************************** */

double calc_blackman_harris(int a_nr, int a_length, int a_d_order){

  if(a_d_order<0)
    call_error("not defined order in window calculation");

  switch(a_d_order){
  case 0:
    return
      WIN_BH_0
      + WIN_BH_1 * cos(1.0*TWO_PI*(double)a_nr/a_length)
      + WIN_BH_2 * cos(2.0*TWO_PI*(double)a_nr/a_length)
      + WIN_BH_3 * cos(3.0*TWO_PI*(double)a_nr/a_length)
      + WIN_BH_4 * cos(4.0*TWO_PI*(double)a_nr/a_length);  
    
  case 1:
    return
      - 1.0*TWO_PI * WIN_BH_1 * sin(1.0*TWO_PI * (double)a_nr/a_length)
      - 2.0*TWO_PI * WIN_BH_2 * sin(2.0*TWO_PI * (double)a_nr/a_length)
      - 3.0*TWO_PI * WIN_BH_3 * sin(3.0*TWO_PI * (double)a_nr/a_length)
      - 4.0*TWO_PI * WIN_BH_4 * sin(4.0*TWO_PI * (double)a_nr/a_length); 

  default: /* not tested for a_d_order > 2 */
    return
        pow(1.0*TWO_PI, (double)a_d_order) * WIN_BH_1 * cos(1.0*TWO_PI * (double)a_nr/a_length + M_PI*a_d_order)
      + pow(2.0*TWO_PI, (double)a_d_order) * WIN_BH_2 * cos(2.0*TWO_PI * (double)a_nr/a_length + M_PI*a_d_order)
      + pow(3.0*TWO_PI, (double)a_d_order) * WIN_BH_3 * cos(3.0*TWO_PI * (double)a_nr/a_length + M_PI*a_d_order)
      + pow(4.0*TWO_PI, (double)a_d_order) * WIN_BH_4 * cos(4.0*TWO_PI * (double)a_nr/a_length + M_PI*a_d_order);  
  }  
}

/* ********************************************************** */
