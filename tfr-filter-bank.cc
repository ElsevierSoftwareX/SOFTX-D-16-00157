
#include "tfr-filter-bank.hh"

/* **************************************************************************** */
/* bank filter constructors and deconstructor */

tfr_filter_bank :: tfr_filter_bank(tfr_array_config* a_bank_conf, tfr_filter_config* a_filter_conf){

  /* filter bank object constructor */

  /* check args */
  if(a_bank_conf==NULL or a_filter_conf==NULL)
    call_error("arg is null");
  
  /* check and set frequencies */
  if(a_bank_conf->min>=a_bank_conf->max)
    call_error("init frequency is grater than final");

  /* m_filter_array contains m_filter_number filters */
  if(a_bank_conf->length<1)
    call_error("filter number is negative or one");

  /* copy config data */
  m_bank_conf = new tfr_array_config;
  m_filter_conf = new tfr_filter_config;
  memcpy(m_bank_conf, a_bank_conf, sizeof(tfr_array_config));
  memcpy(m_filter_conf, a_filter_conf, sizeof(tfr_filter_config));
      
  m_filter_array = new tfr_recursive_filter* [m_bank_conf->length];
  for(int n=0; n<m_bank_conf->length; n++){

    tfr_filter_config* conf = new tfr_filter_config;
    memcpy(conf, a_filter_conf, sizeof(tfr_filter_config));

    double delta = (m_bank_conf->max - m_bank_conf->min) / (m_bank_conf->length-1);
    conf->frequency = m_bank_conf->min + n * delta;

    /* get configuration from internal structure */
    m_filter_array[n] = new tfr_recursive_filter(conf);

    delete conf;
  }
}

tfr_filter_bank :: ~tfr_filter_bank(){
  /* filter bank deconstructor */

  delete m_bank_conf;
  
  for(int n=0; n<m_bank_conf->length; n++)
    delete m_filter_array[n];
  delete [] m_filter_array;
}

/* ************************************************************************* */

tfr_array_config* tfr_filter_bank :: get_bank_configuration(){

  /* return bank parameters as structure */
  /* this is usable especially for analyzer */

  tfr_array_config* conf = new tfr_array_config;
  memcpy(conf, m_bank_conf, sizeof(tfr_array_config));
  
  return conf;
}

tfr_array_config* tfr_filter_bank :: get_config(){

  /* short name of get_bank_configuration() */

  return get_bank_configuration();
}

tfr_filter_config* tfr_filter_bank :: get_filter_configuration(int a_index){

  /* get single filter configuration */

  if(a_index<0 or a_index>=m_bank_conf->length)
    call_error("wrong index");
  
  tfr_filter_config* conf = m_filter_array[a_index]->get_config();
  return conf;
}

/* ************************************************************************* */

tfr_complex_signal** tfr_filter_bank :: filtering(tfr_complex_signal* a_sig, int a_hop){
  
  /* signal is introduced to each of component filter */

  tfr_complex_signal** output = new tfr_complex_signal* [m_bank_conf->length];
  for(int n=0; n<m_bank_conf->length; n++){

    output[n] = m_filter_array[n]->filtering(a_sig, a_hop);
    progress_bar(n+1, m_bank_conf->length, "filtering");
  }

  progress_bar(0, 0, "filtering");  
  return output;
}

void tfr_filter_bank :: filtering(complex double a_sample){

  /* a single sample is introduced to each of component filter 
   results can be taken from m_output internam variable 
   inside each component filter */

  for(int n=0; n<m_bank_conf->length; n++)
    m_filter_array[n]->filtering(a_sample);    
}

/* **************************************************************************** */

int tfr_filter_bank :: get_processed_sample_number(){

  /* it returns number of processed samples */
  /* printing warning if numbers are different */

  for(int n=1; n<m_bank_conf->length; n++){
    if(m_filter_array[0]->get_processed_sample_number() != m_filter_array[n]->get_processed_sample_number()){
      call_warning("numbers of processed samples are diffrent in diffrent filters");
      break;
    }
  }
  
  return m_filter_array[0]->get_processed_sample_number();
}

/* **************************************************************************** */
