#ifndef _tfr_raster_image_
#define _tfr_raster_image_

/* headers */

#include "tfr-external.hh"
#include "tfr-misc.hh"

#include "tfr-real-array.hh"
struct tfr_array_config;
class tfr_real_array;

/* struct for image configuration */

struct tfr_image_config{
  
  double min_frequency;  
  double max_frequency;
  double min_time;
  double max_time;
  int height;
  int width;
};

/* time-frequency image class definition */

class tfr_raster_image{
private:

  /* configuration */
  tfr_image_config* m_image_conf;
  
public:
  tfr_raster_image(tfr_image_config*);
  ~tfr_raster_image();

  /* clear data */
  void clear_data();
  void clear();
  
  /* time-frequency configuration */
  bool compare_configuration(tfr_image_config*);
  bool compare_time_configuration(tfr_image_config*);
  tfr_image_config* get_image_configuration();
  tfr_image_config* get_config();
  
  /* time-frequency data */
  double **m_data;

  /* save to text file */
  void save_to_file(const char *);
  void save(const char *);

  /* get time or frequency value */  
  double get_time_by_index(int);
  double get_frequency_by_index(int);

  /* get the nearest indexes */
  int get_index_by_time(double);
  int get_index_by_frequency(double);

  /* get marginal distributions */
  tfr_real_array* get_horizontal_distribution(tfr_raster_image* =NULL);
  tfr_real_array* get_vertical_distribution(tfr_raster_image* =NULL);
};

#endif

