#ifndef _tfr_real_array_
#define _tfr_real_array_

/* headers */

#include "tfr-external.hh"
#include "tfr-misc.hh"

/* struct for array configuration */

struct tfr_array_config{
  double min;
  double max;
  int length;
};

class tfr_real_array{
private:

  /* configuration */
  tfr_array_config* m_array_conf;

public:

  tfr_real_array(tfr_array_config*);
  ~tfr_real_array(); 

  tfr_array_config* get_array_configuration();
  tfr_array_config* get_config();

  double get_arg_by_index(int);
  int get_index_by_arg(double);

  void save(const char*);
  void save_to_file(const char*);

  double* m_data;
};

#endif
