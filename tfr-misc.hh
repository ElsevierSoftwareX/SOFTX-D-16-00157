#ifndef _tfr_misc_
#define _tfr_misc_

/* headers */

#include "tfr-external.hh"

/* defined macros */

#define STANDARD_STRLEN 1024
#define TWO_PI   6.2831853071795864769252866
#define E_NUMBER 2.7182818284590452353602975
#define PG_BAR 100

#define TFR_VERSION "0-16"

#define WIN_BH_0  0.3232153788877343
#define WIN_BH_1 -0.4714921439576260
#define WIN_BH_2  0.175534129901972
#define WIN_BH_3 -0.02849699010614994
#define WIN_BH_4  0.001261357088292677


/* function signatures */

/* for diagnostic */
void tf_print_info();
void call_error(const char*);
void call_warning(const char*);
void call_info(const char*);
void progress_bar(int, int, const char *);

/* for calculations */
double rand_gauss(double =1.0);
int calc_factorial(int);
double calc_binominal (int, int);
double calc_eulerian (int, int);

double calc_blackman_harris(int, int, int =0);

#endif
