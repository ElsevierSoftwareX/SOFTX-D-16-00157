
#include "tfr-real-array.hh"

tfr_real_array :: tfr_real_array(tfr_array_config* a_conf){

  /* constructor */

  /* check args */
  if(a_conf == NULL)
    call_error("arg is null");
  if (a_conf->length <= 0)
    call_error("length <= 0");
  if (a_conf->max<=a_conf->min)
    call_error("max < min and delta < 0");

  /* save arguments */
  m_array_conf = new tfr_array_config;
  memcpy(m_array_conf, a_conf, sizeof(tfr_array_config));
  
 /* allocate memory for samples */
  m_data = new double [m_array_conf->length];
  int byte_size = m_array_conf->length * sizeof(double);
  memset(m_data, 0x0, byte_size);
}

tfr_real_array :: ~tfr_real_array(){

  /* array deconstructor */
  /* free memory */

  delete m_array_conf;
  delete [] m_data;
}

/* **************************************************************************** */

tfr_array_config* tfr_real_array :: get_array_configuration(){

  /* return signal parameters as structure */

  tfr_array_config* conf = new tfr_array_config;
  memcpy(conf, m_array_conf, sizeof(tfr_array_config));
    
  return conf;
}

tfr_array_config* tfr_real_array :: get_config(){

  /* short name of get_signal_configuration() */
  
  return get_array_configuration();
}

/* **************************************************************************** */

double tfr_real_array :: get_arg_by_index(int a_index){
  
  /* get argument by index */

  double hop = (double)(m_array_conf->max - m_array_conf->min) / (m_array_conf->length - 1);  
  return m_array_conf->min + a_index * hop;
}

int tfr_real_array :: get_index_by_arg(double a_arg){

  /* get the nearest indexes */

  double hop = (double)(m_array_conf->max - m_array_conf->min) / (m_array_conf->length - 1);
  double tmp = (a_arg - m_array_conf->min);
  return round(tmp/hop);
}

/* ******************************************************************************** */

void tfr_real_array :: save(const char * a_fname){

  /* short */
  
  save_to_file(a_fname);
}

void tfr_real_array :: save_to_file(const char * a_fname){

  /* save array to a file */
  
  /* open file to write */
  FILE *fds = fopen(a_fname, "w");
  if (fds==NULL)
    call_error("cannot save");

  /* write start and stop */
  fprintf(fds, "#LENGTH=%e\n", m_array_conf->length);
  fprintf(fds, "#START=%e\n", m_array_conf->min);
  fprintf(fds, "#STOP=%e\n", m_array_conf->max);

  for(int n=0; n<m_array_conf->length; n++){
    
    double arg = get_arg_by_index(n);
    fprintf(fds, "%e\t%e\n", arg, m_data[n]);
  }
  
  fclose(fds);  
  fprintf(stderr, "(info) save file: %s\n", a_fname);
}

/* **************************************************************************** */
