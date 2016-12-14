
#include "tfr-fft-analyzer.hh"

/* **************************************************************************** */
/* bank filter constructor and deconstructor */

tfr_fft_analyzer :: tfr_fft_analyzer(tfr_array_config* a_bank_conf, tfr_window_config* a_window_conf){
  
  /* this is constructor of tf (fourier) analyzer object */

  /* check args */
  if(a_bank_conf==NULL or a_window_conf==NULL)
    call_error("arg is null");

  /* check and set frequencies */
  if(a_bank_conf->min>=a_bank_conf->max)
    call_error("init frequency is grater than final");
  
  if(a_bank_conf->max > a_window_conf->rate/2)
    call_error("max > rate/2");
  if(a_bank_conf->min < -a_window_conf->rate/2)
    call_error("min < -rate/2");

  /* parse other parameters */
  if(a_bank_conf->length<1)
    call_error("filter number is negative or one");

  if(a_window_conf->rate<=0)
    call_error("rate is smaller than 0");

  if(a_window_conf->length>a_bank_conf->length)
    call_error("win len > fft len");

  /* main bank config */
  m_bank_conf = new tfr_array_config;
  memcpy(m_bank_conf, a_bank_conf, sizeof(tfr_array_config));
  m_window_gen = new tfr_window_generator(a_window_conf);
  
  m_input_signal = NULL;

  /* update frequency range */
  double delta = a_window_conf->rate / a_bank_conf->length;
  int initial = (a_bank_conf->min+a_window_conf->rate/2) / delta;
  double new_f_min = initial * delta - a_window_conf->rate/2; 
  int finish = (a_bank_conf->max+a_window_conf->rate/2) / delta;
  double new_f_max = finish * delta - a_window_conf->rate/2; 
  if(new_f_min!=a_bank_conf->min or new_f_max!=a_bank_conf->max){ 
    call_warning("frequency is fixed:");
    call_warning("see configuration of returned distributions");
  }
  
  m_bank_conf->min = new_f_min;
  m_bank_conf->max = new_f_max;
  
  /* allocate slots */
  m_fourier_spectra = new complex double**[6];
  for(int s=0; s<6; s++)
    m_fourier_spectra[s] = NULL;
}

tfr_fft_analyzer :: ~tfr_fft_analyzer(){

  /* tf analyzer deconstructor */

  for(int s=0; s<6; s++){    
    if (m_fourier_spectra[s]!=NULL){

      for(int n=0;n<get_width();n++)
	delete [] m_fourier_spectra[s][n];    
      delete [] m_fourier_spectra[s];      
    }  
  }
  
  delete [] m_fourier_spectra;
  
  delete m_input_signal;
  delete m_window_gen;
  delete m_bank_conf;
}

/* ************************************************************************* */

complex double ** tfr_fft_analyzer :: transforming(int a_t_order, int a_d_order){

  complex double** stft = new complex double*[get_width()];
  for(int n=0;n<get_width();n++){
    stft[n] = new complex double[get_height()];
    int byte_size = get_height() * sizeof(complex double);
    memset(stft[n], 0x0, byte_size);
  }

  /* tmp signal for fft - only length is used */
  tfr_signal_config sig_conf;
  sig_conf.length = m_bank_conf->length;
    
  tfr_complex_signal* in_tmp = new tfr_complex_signal(&sig_conf);
  int start_index = (m_bank_conf->length-m_window_gen->get_length()) / 2;

  int byte_size_win = m_window_gen->get_length() * sizeof(complex double);
  int byte_size_out = get_height() * sizeof(complex double);

  /* get window waveform */
  tfr_complex_signal* window_waveform = m_window_gen->get_window(a_t_order, a_d_order);
  double delta = m_window_gen->get_rate() / m_bank_conf->length;
  int initial = (m_bank_conf->min + m_window_gen->get_rate()/2) / delta;
  
  /* calculating spectrua by fft */
  for(int n=0; n<get_width();n++){
    in_tmp->clear();
    
    int curr_index = n*m_hop;
    memcpy(&in_tmp->m_waveform[start_index], &m_input_signal->m_waveform[curr_index], byte_size_win);
    for(int m=0; m<m_window_gen->get_length(); m++)
      in_tmp->m_waveform[start_index+m] *= window_waveform->m_waveform[m];

    tfr_fourier_spectrum* out_tmp = in_tmp->get_spectrum();
    memcpy(stft[n], &out_tmp->m_spectrum[initial], byte_size_out);
    delete out_tmp;

    progress_bar(n+1, get_width(), "transform");
  }

  progress_bar(0, 0, "transform");  
  delete window_waveform;
  delete in_tmp;
  return stft;
}

/* ************************************************************************* */

void tfr_fft_analyzer :: set_signal(tfr_complex_signal* a_sig, int a_hop){
  
  tfr_analyzer :: set_signal(a_sig, a_hop);
  tfr_signal_config* conf = m_input_signal->get_config();

  m_width = (conf->length - m_window_gen->get_length() + 1) / m_hop;

  double delta = conf->rate / m_bank_conf->length;
  m_height = (m_bank_conf->max-m_bank_conf->min) / delta;
  
  if(conf->rate!=m_window_gen->get_rate())
    call_error("signal and window sampling rates are different");

  if((conf->length-m_hop)<m_window_gen->get_length())
    call_error("signal is shorter than window");

  delete conf;
}

int tfr_fft_analyzer :: get_width(){
  
  if(m_input_signal==NULL)
    call_error("signal is not loaded!");

  return m_width;
}

int tfr_fft_analyzer :: get_height(){
  
  if(m_input_signal==NULL)
    call_error("signal is not loaded!");

  return m_height;
}

/* ************************************************************************* */

tfr_raster_image* tfr_fft_analyzer :: create_empty_image(){

  /* generated image is empty 
     however it is configured */

  /* if signal is not loaded, 
     time range is unknown */
  if(m_input_signal==NULL)
    call_error("signal is not loaded!");

  tfr_signal_config* sig_conf = m_input_signal->get_config();
  int win_width = m_window_gen->get_length();
  
  tfr_image_config img_conf;
  img_conf.height = get_height();
  img_conf.width = get_width();

  img_conf.min_time = sig_conf->start + (double)win_width * 0.5 / sig_conf->rate;  
  img_conf.max_time = img_conf.min_time + (double)((get_width()-1)*m_hop) / sig_conf->rate; 
  img_conf.min_frequency = m_bank_conf->min;
  img_conf.max_frequency = m_bank_conf->max;
  
  tfr_raster_image* output = new tfr_raster_image(&img_conf);

  delete sig_conf;
  return output;
}

/* ************************************************************************* */

tfr_raster_image* tfr_fft_analyzer :: get_spectral_energy(){

  /* calculate energy estimate */
  
  /* check input signal */
  if(m_input_signal==NULL)
    call_error("signal is not loaded!");

  /* filtering  (2 slot) */
  if(m_fourier_spectra[CODE_WIN] == NULL)
    m_fourier_spectra[CODE_WIN] = transforming(0, 0);
  
  /* make empty output object */
  tfr_raster_image* output = create_empty_image();
  
  /* calc energy estimate */
  for(int n=0; n<get_width(); n++){
    for(int k=0; k<get_height(); k++)
      output->m_data[n][k] = pow(cabs(m_fourier_spectra[CODE_WIN][n][k]), 2.0);

    progress_bar(n+1, get_width(), "energy");
  }
  
  progress_bar(0, 0, "energy");
  return output;
}

/* ************************************************************************* */

tfr_raster_image* tfr_fft_analyzer :: get_instantaneous_frequency(){

  /* calculate if estimate */

  /* check input signal */
  if(m_input_signal==NULL)
    call_error("signal is not loaded!");

  /* transforming (1 and 2 slots) */
  if(m_fourier_spectra[CODE_WIN_D] == NULL)
    m_fourier_spectra[CODE_WIN_D] = transforming(1, 0);
  if(m_fourier_spectra[CODE_WIN] == NULL)
    m_fourier_spectra[CODE_WIN] = transforming(0, 0);
  
  /* make empty output object */
  tfr_raster_image* output = create_empty_image();

  /* calc instantaneous frequency estimate */
  for(int k=0; k<get_height(); k++){
    double freq = m_bank_conf->min + m_window_gen->get_rate() * (double)k/m_bank_conf->length;
    
    for(int n=0; n<get_width(); n++){
      
      complex double y = m_fourier_spectra[CODE_WIN][n][k]; 
      complex double yD = m_fourier_spectra[CODE_WIN_D][n][k]; 

      if(cabs(y)==0)
	output->m_data[n][k] = 1E300;
      else
	output->m_data[n][k] = freq-cimag(yD/y) / TWO_PI;
    }    

    progress_bar(k+1, get_height(), "i-freq");
  }

  progress_bar(0, 0, "i-freq");    
  return output;
}

/* ************************************************************************* */

tfr_raster_image* tfr_fft_analyzer :: get_spectral_delay(){

  /* calculate sd estimate */

  /* check input signal */
  if(m_input_signal==NULL)
    call_error("signal is not loaded!");

  /* transforming  (2 and 3 slots) */
  if(m_fourier_spectra[CODE_WIN_T] == NULL)
    m_fourier_spectra[CODE_WIN_T] = transforming(0, 1);
  if(m_fourier_spectra[CODE_WIN] == NULL)
    m_fourier_spectra[CODE_WIN] = transforming(0, 0);

  /* make empty output object */
  tfr_raster_image* output = create_empty_image();
  
  /* calc spectral delay estimate */
  for(int k=0; k<get_height(); k++){
    for(int n=0; n<get_width(); n++){

      complex double y = m_fourier_spectra[CODE_WIN][n][k]; 
      complex double yT = m_fourier_spectra[CODE_WIN_T][n][k]; 

      if(cabs(y)==0)
	output->m_data[n][k] = 1E300;
      else
	output->m_data[n][k] = creal(yT/y);
    }

    progress_bar(k+1, get_height(), "s-delay");
  }

  progress_bar(0, 0, "s-delay");
  return output;
}

/* ************************************************************************* */

tfr_raster_image* tfr_fft_analyzer :: get_chirp_rate_by_k_estimator(){

  /* calculate cr estimate */
  /* k is from Krzysztof */

  /* check input signal */
  if(m_input_signal==NULL)
    call_error("signal is not loaded!");

    /* transforming  (1, 2, and 3 slots) */
  if(m_fourier_spectra[CODE_WIN_T] == NULL)
    m_fourier_spectra[CODE_WIN_T] = transforming(0, 1);
  if(m_fourier_spectra[CODE_WIN] == NULL)
    m_fourier_spectra[CODE_WIN] = transforming(0, 0);
  if(m_fourier_spectra[CODE_WIN_D] == NULL)
    m_fourier_spectra[CODE_WIN_D] = transforming(1, 0);
  
  /* make empty output object */
  tfr_raster_image* output = create_empty_image();
  
  /* calc chirp rate estimate */
  for(int k=0; k<get_height(); k++){

      for(int n=0; n<get_width(); n++){

	complex double yD = m_fourier_spectra[CODE_WIN_D][n][k]; 
	complex double y = m_fourier_spectra[CODE_WIN][n][k]; 
	complex double yT = m_fourier_spectra[CODE_WIN_T][n][k]; 

	if(cabs(y)==0)
	  output->m_data[n][k] = 1E300;
	else{
	  double nominative = creal(yD/y) / TWO_PI; 
	  double denominative = cimag(yT/y); 
	  output->m_data[n][k] = nominative / denominative;
	}
      }
    
    progress_bar(k+1, get_height(), "c-rate");
  }

  progress_bar(0, 0, "c-rate");
  return output;
}

/* ************************************************************************* */

tfr_raster_image* tfr_fft_analyzer :: get_chirp_rate_by_d_estimator(){

  /* calculate cr estimate */
  /* d is from Dominique */
  
  /* check input signal */
  if(m_input_signal==NULL)
    call_error("signal is not loaded!");

  /* transforming  (0, 1, 2, 3, and 5 slots) */
  if(m_fourier_spectra[CODE_WIN_T] == NULL)
    m_fourier_spectra[CODE_WIN_T] = transforming(0, 1);
  if(m_fourier_spectra[CODE_WIN] == NULL)
    m_fourier_spectra[CODE_WIN] = transforming(0, 0);
  if(m_fourier_spectra[CODE_WIN_D] == NULL)
    m_fourier_spectra[CODE_WIN_D] = transforming(1, 0);
  if(m_fourier_spectra[CODE_WIN_D2] == NULL)
    m_fourier_spectra[CODE_WIN_D2] = transforming(2, 0);
  if(m_fourier_spectra[CODE_WIN_DT] == NULL)
    m_fourier_spectra[CODE_WIN_DT] = transforming(1, 1);
  
  /* make empty output object */
  tfr_raster_image* output = create_empty_image();
  
  /* calc chirp rate estimate */
  for(int k=0; k<get_height(); k++){

      for(int n=0; n<get_width(); n++){

	complex double yD2 = m_fourier_spectra[CODE_WIN_D2][n][k]; 
	complex double yD = m_fourier_spectra[CODE_WIN_D][n][k]; 
	complex double y = m_fourier_spectra[CODE_WIN][n][k]; 
	complex double yT = m_fourier_spectra[CODE_WIN_T][n][k]; 
	complex double yDT = m_fourier_spectra[CODE_WIN_DT][n][k]; 

	if(cabs(y)==0)
	  output->m_data[n][k] = 1E300;
	else{
	  double nominative = creal(yD2 / y - yD * yD / (y * y)) / TWO_PI;
	  double denominative = cimag(yDT / y - yD * yT / (y * y));
	  output->m_data[n][k] = nominative / denominative;
	}
      }
    
    progress_bar(k+1, get_height(), "c-rate");
  }

  progress_bar(0, 0, "c-rate");
  return output;
}

/* ************************************************************************* */

tfr_raster_image* tfr_fft_analyzer :: get_chirp_rate_by_f_estimator(){

  /* calculate cr estimate */
  /* f is from Francois */

  /* check input signal */
  if(m_input_signal==NULL)
    call_error("signal is not loaded!");

  /* transforming  (1, 2, 3, 4, and 5 slots) */
  if(m_fourier_spectra[CODE_WIN_T] == NULL)
    m_fourier_spectra[CODE_WIN_T] = transforming(0, 1);
  if(m_fourier_spectra[CODE_WIN] == NULL)
    m_fourier_spectra[CODE_WIN] = transforming(0, 0);
  if(m_fourier_spectra[CODE_WIN_D] == NULL)
    m_fourier_spectra[CODE_WIN_D] = transforming(1, 0);
  if(m_fourier_spectra[CODE_WIN_T2] == NULL)
    m_fourier_spectra[CODE_WIN_T2] = transforming(0, 2);
  if(m_fourier_spectra[CODE_WIN_DT] == NULL)
    m_fourier_spectra[CODE_WIN_DT] = transforming(1, 1);
  
  /* make empty output object */
  tfr_raster_image* output = create_empty_image();
  
  /* calc chirp rate estimate */
  for(int k=0; k<get_height(); k++){

      for(int n=0; n<get_width(); n++){

	complex double yD = m_fourier_spectra[CODE_WIN_D][n][k]; 
	complex double y = m_fourier_spectra[CODE_WIN][n][k]; 
	complex double yT = m_fourier_spectra[CODE_WIN_T][n][k]; 
	complex double yT2 = m_fourier_spectra[CODE_WIN_T2][n][k]; 
	complex double yDT = m_fourier_spectra[CODE_WIN_DT][n][k]; 

	if(cabs(y)==0)
	  output->m_data[n][k] = 1E300;
	else{
	  double nominative = cimag(yDT / y - yD * yT / (y * y))  / TWO_PI;
	  double denominative = creal(yT2 / y - yT * yT / (y * y));
	  output->m_data[n][k] = -nominative / denominative;
	}
      }
    
    progress_bar(k+1, get_height(), "c-rate");
  }

  progress_bar(0, 0, "c-rate");
  return output;
}

/* ************************************************************************* */

