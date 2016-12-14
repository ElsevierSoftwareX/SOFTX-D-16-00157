
#include "tfr-analyzer.hh"

/* ************************************************************************* */
                   
void tfr_analyzer :: set_signal(tfr_complex_signal* a_sig, int a_hop){

  /* this constructor is used in other classes */
  
  /* check args */
  if(a_hop<1)
    call_warning("hop is smaller than 1");  
  m_hop = a_hop;

  if(a_sig == NULL)
    call_error("signal is null!");
  
  /* check input signal is assigned */
  if (m_input_signal != NULL)
    call_error("you can set only one signal!");

  tfr_signal_config* conf = a_sig->get_config();
  m_input_signal = new tfr_complex_signal(conf);
  m_input_signal->copy(a_sig);
  delete conf;
}

/* ************************************************************************* */

tfr_raster_image* tfr_analyzer :: get_chirp_rate(int a_estimator_code){
  
  /* this method allows to choose estimator of chirp rate */

  switch(a_estimator_code){
  case CR_K_ESTIMATOR: return get_chirp_rate_by_k_estimator();
  case CR_D_ESTIMATOR: return get_chirp_rate_by_d_estimator();
  case CR_F_ESTIMATOR: return get_chirp_rate_by_f_estimator();
  default:
    call_error("not known code");
  }
}

/* ************************************************************************* */


