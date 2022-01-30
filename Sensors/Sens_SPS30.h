//- -----------------------------------------------------------------------------------------------------------------------
// AskSin++ sensor library for Sensirion SPS30 particulate matter sensor
// 2018-04-03 papa Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
// sensor library for Sensirion SPS30 particulate matter sensor
// 23/01/2022 HMSteve, Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------

#ifndef __SENS_SPS30_h__
#define __SENS_SPS30_h__

#include <Sensors.h>
#include <sps30.h>

#define AUTO_CLEAN_DAYS 7


namespace as {

class Sens_SPS30 : public Sensor {


public:
  struct sps30_measurement m;

  Sens_SPS30 () {}

  void init () {
    int16_t ret;
    sensirion_i2c_init();
    while (sps30_probe() != 0) {
      DPRINTLN("SPS30 sensor probing failed!");
      delay(500);
    }
    ret = sps30_set_fan_auto_cleaning_interval_days(AUTO_CLEAN_DAYS);
    if (ret) {
      DPRINT("Error setting SPS30 the auto-clean interval: "); DDECLN(ret);
    }
    ret = sps30_start_measurement();
    if (ret < 0) {
      DPRINTLN("SPS30 error starting measurement!");
    } else {
      DPRINTLN("SPS30 ok, measurement started");
    }
  }


  bool measure (__attribute__((unused)) bool async=false) {
    int16_t ret = sps30_read_measurement(&m);
    if (ret != 0) {
      return false;
    } else {
      return true;
    }
  }


  bool stop_measurement (void) {
    int16_t ret = sps30_stop_measurement();
    if (ret != 0) {
      return false;
    } else {
      return true;
    }
  }


  bool start_measurement (void) {
    int16_t ret = sps30_start_measurement();
    if (ret != 0) {
      return false;
    } else {
      return true;
    }
  }


  bool start_manual_fan_cleaning (void) {
    int16_t ret = sps30_start_manual_fan_cleaning();
    if (ret != 0) {
      return false;
    } else {
      return true;
    }
  }


  uint16_t mc_1p0() { return m.mc_1p0 * 10; }
  uint16_t mc_2p5() { return m.mc_2p5 * 10; }
  uint16_t mc_4p0() { return m.mc_4p0 * 10; }
  uint16_t mc_10p0() { return m.mc_10p0 * 10; }
  uint16_t nc_0p5() { return m.nc_0p5 * 10; }
  uint16_t nc_1p0() { return m.nc_1p0 * 10; }
  uint16_t nc_2p5() { return m.nc_2p5 * 10; }
  uint16_t nc_4p0() { return m.nc_4p0 * 10; }
  uint16_t nc_10p0() { return m.nc_10p0 * 10; }
  uint16_t partsize() { return m.typical_particle_size * 100; }
};

}

#endif
