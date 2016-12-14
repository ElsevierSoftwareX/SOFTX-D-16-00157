
#include "tfr-iir-analyzer.hh"

/* **************************************************************************** */
/* bank filter constructor and deconstructor */

tfr_iir_analyzer :: tfr_iir_analyzer(tfr_array_config* a_bank_conf, tfr_filter_config* a_filter_conf){
  
  /* this is constructor of tf (filter) analyzer object */

  /* check args */
  if(a_bank_conf==NULL or a_filter_conf==NULL)
    call_error("arg is null");

  /* check and set frequencies */
  if(a_bank_conf->min>=a_bank_conf->max)
    call_error("init frequency is grater than final");
  
  /* parse other parameters */
  if(a_bank_conf->length<1)
    call_error("filter number is negative or one");

  if(a_filter_conf->spread<=0)
    call_error("spread is smaller than 0");

  if(a_filter_conf->order<3)
    call_error("order is smaller than 3");

  /* copy config data */
  m_bank_conf = new tfr_array_config;
  m_filter_conf = new tfr_filter_config;
  memcpy(m_bank_conf, a_bank_conf, sizeof(tfr_array_config));
  memcpy(m_filter_conf, a_filter_conf, sizeof(tfr_filter_config));
  
  /* internal filter banks allocate */
  m_bank_array = new tfr_filter_bank*[5];
  for(int n=0; n<5; n++){
    a_filter_conf->order = m_filter_conf->order - 2 + n;
    m_bank_array[n] = new tfr_filter_bank(a_bank_conf, a_filter_conf);
  }

  a_filter_conf->order = m_filter_conf->order;
  m_input_signal = NULL;

  /* allocate slots */
  m_filtered_signals = new tfr_complex_signal**[5];
  for(int n=0; n<5; n++)
    m_filtered_signals[n] = NULL;
}

tfr_iir_analyzer :: ~tfr_iir_analyzer(){

  /* tf analyzer deconstructor */
  
  for(int n=0; n<5; n++){  
    if (m_filtered_signals[n]!=NULL){

      for(int k=0; k<m_bank_conf->length; k++)
	delete m_filtered_signals[n][k];      
      delete [] m_filtered_signals[n];
    }
  }
  delete [] m_filtered_signals;
  
  for(int n=0; n<5; n++)
    delete m_bank_array[n];
  delete [] m_bank_array;

  delete m_input_signal;
  delete m_filter_conf;
  delete m_bank_conf;
}

/* ************************************************************************* */

void tfr_iir_analyzer :: set_signal(tfr_complex_signal* a_sig, int a_hop){
  
  tfr_analyzer :: set_signal(a_sig, a_hop);
  tfr_signal_config* conf = m_input_signal->get_config();

  if(conf->rate!=m_filter_conf->rate)
    call_error("signal and filter sampling rates are different");
  
  delete conf;
}

int tfr_iir_analyzer :: get_width(){
  
  if(m_input_signal==NULL)
    call_error("signal is not loaded!");

  /* filtering */
  if(m_filtered_signals[2] == NULL)
    m_filtered_signals[2] = m_bank_array[2]->filtering(m_input_signal, m_hop);  

  /* get length from first signal */  
  tfr_signal_config* conf = m_filtered_signals[2][0]->get_config();
  int length = conf->length;
  delete conf;
  
  return length;
}

int tfr_iir_analyzer :: get_height(){
  
  return m_bank_conf->length;
}

/* ************************************************************************* */

tfr_raster_image* tfr_iir_analyzer :: create_empty_image(){

  /* generated image is empty 
     however it is configured */

  /* if signal is not loaded, 
   we do not know time range */
  if(m_input_signal==NULL)
    call_error("signal is not loaded!");

  tfr_signal_config* sig_conf = m_filtered_signals[2][0]->get_config();
  
  tfr_image_config img_conf;
  img_conf.height = get_height();
  img_conf.width = get_width();

  img_conf.min_time = sig_conf->start;  
  img_conf.max_time = sig_conf->start + (double)(sig_conf->length-1) / sig_conf->rate; 
  img_conf.min_frequency = m_bank_conf->min;
  img_conf.max_frequency = m_bank_conf->max;
  
  tfr_raster_image* output = new tfr_raster_image(&img_conf);

  delete sig_conf;
  return output;
}

/* ************************************************************************* */

tfr_raster_image* tfr_iir_analyzer :: get_spectral_energy(){

  /* calculate energy estimate */

  /* check input signal */
  if(m_input_signal==NULL)
    call_error("signal is not loaded!");

  /* filtering  (2 bank) */
  if(m_filtered_signals[2] == NULL)
    m_filtered_signals[2] = m_bank_array[2]->filtering(m_input_signal, m_hop);  
  
  /* make empty output object */
  tfr_raster_image* output = create_empty_image();

  /* calc energy estimate */
  for(int n=0; n<get_width(); n++){
    for(int k=0; k<get_height(); k++)
      output->m_data[n][k] = pow(cabs(m_filtered_signals[2][k]->m_waveform[n]), 2.0);

    progress_bar(n+1, get_width(), "energy");
  }

  progress_bar(0, 0, "energy");
  return output;
}

/* ************************************************************************* */

tfr_raster_image* tfr_iir_analyzer :: get_instantaneous_frequency(){

  /* calculate if estimate */

  /* check input signal */
  if(m_input_signal==NULL)
    call_error("signal is not loaded!");

  /* filtering  (1 and 2 banks) */
  for(int n=1; n<3; n++)
    if(m_filtered_signals[n] == NULL)
      m_filtered_signals[n] = m_bank_array[n]->filtering(m_input_signal, m_hop);  
  
  /* make empty output object */
  tfr_raster_image* output = create_empty_image();

  /* calc instantaneous frequency estimate */
  for(int k=0; k<get_height(); k++){
    
    complex double pfactor = m_bank_array[2]->m_filter_array[k]->get_p_factor();
    for(int n=0; n<get_width(); n++){
      
      complex double y = m_filtered_signals[2][k]->m_waveform[n]; 
      complex double yD = m_filtered_signals[1][k]->m_waveform[n] / m_filter_conf->spread;
      yD += m_filtered_signals[2][k]->m_waveform[n] * pfactor;

      if(cabs(y)==0)
	output->m_data[n][k] = 1E300;
      else
	output->m_data[n][k] = cimag(yD/y) / TWO_PI;
    }    

    progress_bar(k+1, m_bank_conf->length, "i-freq");
  }

  progress_bar(0, 0, "i-freq");    
  return output;
}

/* ************************************************************************* */

tfr_raster_image* tfr_iir_analyzer :: get_spectral_delay(){

  /* calculate sd estimate */

  /* check input signal */
  if(m_input_signal==NULL)
    call_error("signal is not loaded!");

  /* filtering  (2 and 3 banks) */
  for(int n=2; n<4; n++)
    if(m_filtered_signals[n] == NULL)
      m_filtered_signals[n] = m_bank_array[n]->filtering(m_input_signal, m_hop);  

  /* make empty output object */
  tfr_raster_image* output = create_empty_image();

  /* calc spectral delay estimate */
  for(int k=0; k<get_height(); k++){
    for(int n=0; n<get_width(); n++){
      
      complex double y = m_filtered_signals[2][k]->m_waveform[n]; 
      complex double yT = m_filtered_signals[3][k]->m_waveform[n] * m_filter_conf->spread * m_filter_conf->order;
      
      output->m_data[n][k] = -creal(yT/y); // + 1.5/m_analyzed_signal->m_rate;
    }

    progress_bar(k+1, m_bank_conf->length, "s-delay");
  }

  progress_bar(0, 0, "s-delay");
  return output;
}

/* ************************************************************************* */

tfr_raster_image* tfr_iir_analyzer :: get_chirp_rate_by_k_estimator(){

  /* calculate cr estimate */
  /* k is from Krzysztof */

  /* check input signal */
  if(m_input_signal==NULL)
    call_error("signal is not loaded!");

  /* filtering  (1 and 3 banks) */
  for(int n=1; n<4; n++)
    if(m_filtered_signals[n] == NULL)
      m_filtered_signals[n] = m_bank_array[n]->filtering(m_input_signal, m_hop);  
  
  /* make empty output object */
  tfr_raster_image* output = create_empty_image();

  /* calc chirp rate estimate */
  for(int k=0; k<get_height(); k++){

    complex double pfactor = m_bank_array[2]->m_filter_array[k]->get_p_factor();
    for(int n=0; n<get_width(); n++){
      
      complex double y = m_filtered_signals[2][k]->m_waveform[n]; 
      complex double yT = m_filtered_signals[3][k]->m_waveform[n] * m_filter_conf->spread * m_filter_conf->order ;      
      complex double yD = m_filtered_signals[1][k]->m_waveform[n] / m_filter_conf->spread;
      yD += m_filtered_signals[2][k]->m_waveform[n] * pfactor;

      double nominative = creal(yD/y) / TWO_PI;
      double denominative = cimag(yT/y); 

      if(cabs(y)==0)
	output->m_data[n][k] = 1E300;
      else
	output->m_data[n][k] = nominative / denominative;
    }

    progress_bar(k+1, m_bank_conf->length, "c-rate");
  }

  progress_bar(0, 0, "c-rate");
  return output;
}

/* ************************************************************************* */

tfr_raster_image* tfr_iir_analyzer :: get_chirp_rate_by_d_estimator(){

  /* calculate cr estimate */
  /* d is from Dominique */

  /* check input signal */
  if(m_input_signal==NULL)
    call_error("signal is not loaded!");

  /* filtering  (0 and 3 banks) */
  for(int n=0; n<4; n++)
    if(m_filtered_signals[n] == NULL)
      m_filtered_signals[n] = m_bank_array[n]->filtering(m_input_signal, m_hop);  

  /* make empty output object */
  tfr_raster_image* output = create_empty_image();

  /* calc chirp rate estimate */
  for(int k=0; k<get_height(); k++){
    
    complex double pfactor = m_bank_array[2]->m_filter_array[k]->get_p_factor();    
    for(int n=0; n<get_width(); n++){
      
      complex double y = m_filtered_signals[2][k]->m_waveform[n]; 
      complex double y2 = cpow(m_filtered_signals[2][k]->m_waveform[n], 2.0); 
      complex double yT = m_filtered_signals[3][k]->m_waveform[n] * m_filter_conf->spread * m_filter_conf->order ;      
      complex double yD = m_filtered_signals[1][k]->m_waveform[n] / m_filter_conf->spread;
      yD += m_filtered_signals[2][k]->m_waveform[n] * pfactor;
      
      complex double yDT = m_filtered_signals[2][k]->m_waveform[n] * m_filter_conf->order;
      yDT += m_filtered_signals[3][k]->m_waveform[n] * m_filter_conf->order * m_filter_conf->spread * pfactor;
      
      complex double yD2 = m_filtered_signals[0][k]->m_waveform[n] / pow(m_filter_conf->spread, 2.0);
      yD2 += 2.0 * pfactor * m_filtered_signals[1][k]->m_waveform[n] / m_filter_conf->spread;
      yD2 += cpow(pfactor, 2.0) * m_filtered_signals[2][k]->m_waveform[n];
      
      double nominative = creal (yD2/y - yD*yD/y2) / TWO_PI;
      double denominative = cimag (yDT/y - yD*yT/y2);

      if(cabs(y)==0)
	output->m_data[n][k] = 1E300;
      else
	output->m_data[n][k] = nominative / denominative;
    }
    
    progress_bar(k+1, m_bank_conf->length, "c-rate");
  }
  
  progress_bar(0, 0, "c-rate");
  return output;
}

/* ************************************************************************* */

tfr_raster_image* tfr_iir_analyzer :: get_chirp_rate_by_f_estimator(){

  /* calculate cr estimate */
  /* f is from Francois */

  /* check input signal */
  if(m_input_signal==NULL)
    call_error("signal is not loaded!");

  /* filtering  (0 and 3 banks) */
  for(int n=1; n<5; n++)
    if(m_filtered_signals[n] == NULL)
      m_filtered_signals[n] = m_bank_array[n]->filtering(m_input_signal, m_hop);  

  /* make empty output object */
  tfr_raster_image* output = create_empty_image();

  /* calc chirp rate estimate */
  for(int k=0; k<get_height(); k++){
    
    complex double pfactor = m_bank_array[2]->m_filter_array[k]->get_p_factor();    
    for(int n=0; n<get_width(); n++){
 
      complex double y = m_filtered_signals[2][k]->m_waveform[n]; 
      complex double y2 = cpow(m_filtered_signals[2][k]->m_waveform[n], 2.0); 
      complex double yT = m_filtered_signals[3][k]->m_waveform[n] * m_filter_conf->spread * m_filter_conf->order ;      
      complex double yD = m_filtered_signals[1][k]->m_waveform[n] / m_filter_conf->spread;
      yD += m_filtered_signals[2][k]->m_waveform[n] * pfactor;
      
      complex double yDT = m_filtered_signals[2][k]->m_waveform[n] * m_filter_conf->order;
      yDT += m_filtered_signals[3][k]->m_waveform[n] * m_filter_conf->order * m_filter_conf->spread * pfactor;
      
      complex double yT2 = m_filter_conf->order * (m_filter_conf->order + 1) * m_filtered_signals[4][k]->m_waveform[n];
      yT2 *= pow(m_filter_conf->spread, 2.0);
      
      double nominative = cimag (yDT/y - yD*yT/y2) / TWO_PI;
      double denominative = creal (yT2/y - yT*yT/y2);

      if(cabs(y)==0)
	output->m_data[n][k] = 1E300;
      else
	output->m_data[n][k] = -nominative / denominative;
    }

    progress_bar(k+1, m_bank_conf->length, "c-rate");
  }
  
  progress_bar(0, 0, "c-rate");
  return output;
}

/* ************************************************************************* */

