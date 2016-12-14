
#include "tfr-recursive-filter.hh"

/* **************************************************************************** */
/* filter constructors and deconstructor */

tfr_recursive_filter :: tfr_recursive_filter(tfr_filter_config* a_conf) {

  /* constructor */

  /* parse arguments */
  if(a_conf->order<1) call_error("order is smaller than 1");
  if(a_conf->spread<=0) call_error("spread is negative");
  if(a_conf->rate<0) call_error("rate is negative");

  if(a_conf->frequency>a_conf->rate/2) call_warning("freq > rate /2");
  if(a_conf->frequency<-a_conf->rate/2) call_warning("freq < -rate /2");

  /* filter configuration */
  m_filter_conf = new tfr_filter_config;
  memcpy(m_filter_conf, a_conf, sizeof(tfr_filter_config));
    
  /* internal buffers allocation */
  m_buffer = new complex double [m_filter_conf->order];
  m_bcoeff = new complex double [m_filter_conf->order];
  m_acoeff = new complex double [m_filter_conf->order + 1];
  int byte_size = m_filter_conf->order * sizeof(complex double);
  memset(m_buffer, 0x0, byte_size);  
  memset(m_bcoeff, 0x0, byte_size);
  byte_size = (m_filter_conf->order+1) * sizeof(complex double);
  memset(m_acoeff, 0x0, byte_size);

  m_processed = 0;

  /* coefficients calculation */
  complex double alpha = cexp(get_p_factor() / m_filter_conf->rate);
  for(int i=0; i<=m_filter_conf->order; i++)
    m_acoeff[i] = cpow(-alpha, i) *  calc_binominal (m_filter_conf->order, i);
  double normspread = m_filter_conf->spread * m_filter_conf->rate;
  double bfactor = 1.0 / (pow(normspread, m_filter_conf->order) * calc_factorial(m_filter_conf->order));  
  for(int i=0; i<m_filter_conf->order; i++)
    m_bcoeff[i] = bfactor * calc_eulerian(m_filter_conf->order, m_filter_conf->order-i) * cpow(alpha, i);  
}

tfr_recursive_filter :: ~tfr_recursive_filter(){

  delete m_filter_conf;

  delete [] m_buffer;
  delete [] m_acoeff;
  delete [] m_bcoeff;
}

/* **************************************************************************** */

tfr_filter_config* tfr_recursive_filter :: get_filter_configuration(){

  /* return filter parameters as structure */
  /* this is usable especially for bank */

  tfr_filter_config* conf = new tfr_filter_config;
  memcpy(conf, m_filter_conf, sizeof(tfr_filter_config));
	 
  return conf;
}

tfr_filter_config* tfr_recursive_filter :: get_config(){

  /* short name of get_filter_configuration() */
  
  return get_filter_configuration();
}

complex double tfr_recursive_filter :: get_p_factor(){

  complex double pfactor = I*TWO_PI * m_filter_conf->frequency -1.0/m_filter_conf->spread;
  return pfactor;
}

/* **************************************************************************** */

int tfr_recursive_filter :: get_processed_sample_number(){

  /* it returns number of processed samples */
  
  return m_processed;
}

complex double tfr_recursive_filter :: filtering (complex double a_sample){

  /* filtering - single sample */

  m_processed ++;

  m_output = m_bcoeff[0] * a_sample;
  m_output += m_buffer[0];
  
  for (int n=0; n<m_filter_conf->order-1; n++){
    m_buffer[n] = m_buffer[n+1];
    m_buffer[n] += m_bcoeff[n+1] * a_sample;
    m_buffer[n] -= m_acoeff[n+1] * m_output;
  }

  m_buffer[m_filter_conf->order-1] = -m_acoeff[m_filter_conf->order] * m_output;
  return m_output;
}

tfr_complex_signal * tfr_recursive_filter :: filtering (tfr_complex_signal *a_sig, int a_hop){
  
  /* filtering - the whole signal */

  /* parse arguments */
  if(a_hop<1)
    call_warning("hop is smaller than 1");  

  tfr_signal_config* conf = a_sig->get_config();
  if(conf->rate != m_filter_conf->rate)
    call_warning("rates are different");  
  
  /* construction of output signal */
  int in_length = conf->length;
  conf->length = (in_length-1)/a_hop; 
  conf->start = conf->start + (double)a_hop / conf->rate;
  conf->rate = conf->rate/a_hop;
  tfr_complex_signal* output = new tfr_complex_signal(conf);

  /* filtering process */
  int counter = 0;
  filtering(a_sig->m_waveform[0]);
  for(int n=1; n<in_length; n++){
    if (n%a_hop==0){
      output->m_waveform[counter] = filtering(a_sig->m_waveform[n]);
      counter++;
    }
    else
      filtering(a_sig->m_waveform[n]);
  }

  /* check filtered sample number */
  if(conf->length != counter)
    call_warning("internal error in filtering");

  delete conf;
  return output;
}

/* ************************************************************************* */
