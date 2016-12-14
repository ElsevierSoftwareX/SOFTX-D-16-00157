/* example of TF anayzer using synthetic signals */
 
/* external headers */
#include <stdlib.h>
#include <stdio.h>

/* internal tfr headers */
#include "tfr.hh"

int main(void){

  tf_print_info();

  /* empty signal construction */
  double rate = 44100.0;
  int length = 3e6;

  tfr_signal_config sig_conf;
  sig_conf.length = length; 
  sig_conf.rate = rate; 

  tfr_complex_signal* signal_ptr = new tfr_complex_signal(&sig_conf);

  /* noise generation */
  tfr_noise_generator* gener_ptr = new tfr_noise_generator();
  double noise_energy = gener_ptr->add_pink_using_range(signal_ptr, 10.0);  
  fprintf(stderr, "(info) added energy %e\n", noise_energy/length);

  /* save to file */
  signal_ptr->save("output.txt"); 
  signal_ptr->save_wav("output.wav"); 

  /* Bartlett method */
  tfr_array_config arr_conf;
  arr_conf.length = 2000;
  arr_conf.max = rate/2;
  arr_conf.min = 0.0;

  int hop = 200;

  tfr_bartlett_pergram *pergram_ptr = new tfr_bartlett_pergram(&arr_conf);
  pergram_ptr->add_signal(signal_ptr, hop);
  pergram_ptr->save("pink.txt");
  
  /* ending *************************************************** */

  delete signal_ptr; 
  delete pergram_ptr;
  return EXIT_SUCCESS;
}
