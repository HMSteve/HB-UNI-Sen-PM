//- -----------------------------------------------------------------------------------------------------------------------
// AskSin++
// 2016-10-31 papa Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
// 2019-02-28 jp112sdl (Creative Commons)
//- -----------------------------------------------------------------------------------------------------------------------
// 2022-01-23 PM, HMSteve (cc)
//- -----------------------------------------------------------------------------------------------------------------------

//#define NDEBUG   // disable all serial debug messages  
#define SENSOR_ONLY

#define EI_NOTEXTERNAL

#include <EnableInterrupt.h>
#include <AskSinPP.h>
#include <LowPower.h>
#include <Register.h>

#include <MultiChannelDevice.h>
#include "sensors/Sens_SHT31.h"
#include "sensors/Sens_SPS30.h"

#define LED_PIN 6
#define CONFIG_BUTTON_PIN 5

#define PEERS_PER_CHANNEL 6

#define FAN_CLEANING_INTERVAL 5      //every 5 days


// all library classes are placed in the namespace 'as'
using namespace as;

// define all device properties
const struct DeviceInfo PROGMEM devinfo = {
  {0xf8, 0x24, 0x01},     // Device ID
  "SGSENPM001",           // Device Serial
  {0xf8, 0x24},           // Device Model 
  0x11,                   // Firmware Version
  as::DeviceType::THSensor, // Device Type
  {0x01, 0x00}            // Info Bytes
};


//Configure the used hardware
typedef AvrSPI<10, 11, 12, 13> SPIType;
typedef Radio<SPIType, 2> RadioType;
typedef StatusLed<LED_PIN> LedType;
typedef AskSin<LedType, NoBattery, RadioType> Hal;

Hal hal;


DEFREGISTER(Reg0, MASTERID_REGS, DREG_LEDMODE, DREG_TRANSMITTRYMAX, 0x20, 0x21)
class GDList0 : public RegList0<Reg0> {
  public:
  
    GDList0(uint16_t addr) : RegList0<Reg0>(addr) {}

    bool updIntervall (uint16_t value) const {
      return this->writeRegister(0x20, (value >> 8) & 0xff) && this->writeRegister(0x21, value & 0xff);
    }
    
    uint16_t updIntervall () const {
      return (this->readRegister(0x20, 0) << 8) + this->readRegister(0x21, 0);
    }


    void defaults () {
      clear();
      ledMode(1);
      transmitDevTryMax(3);           
      updIntervall(180);
    }
};

DEFREGISTER(Reg1, 0x01, 0x02, 0x03)
class SensorList1 : public RegList1<Reg1> {
  public:
    SensorList1 (uint16_t addr) : RegList1<Reg1>(addr) {}

    bool samplingPeriod (uint8_t value) const {
      return this->writeRegister(0x01, value);
    }
    
    uint16_t samplingPeriod () const {
      return this->readRegister(0x01, 0);
    }

    bool humidityLimit (uint16_t value) const {
      return this->writeRegister(0x02, (value >> 8) & 0xff) && this->writeRegister(0x03, value & 0xff);
    }
    
    uint16_t humidityLimit () const {
      return (this->readRegister(0x02, 0) << 8) + this->readRegister(0x03, 0);
    }
    
 
    void defaults () {
      clear();
      samplingPeriod(30); 
      humidityLimit(95);
    }
};


class PMDataMsg1 : public Message {
  public:
    void init(uint8_t msgcnt, int16_t temp, uint8_t humidity, uint16_t mc1p0, uint16_t mc2p5, uint16_t mc4p0, uint16_t mc10p0, uint8_t pm_valid) {
      // Message Length (first byte param.): 11 + payload. Max. payload: 17 Bytes (https://www.youtube.com/watch?v=uAyzimU60jw)
      uint8_t t1 = (temp >> 8) & 0x7f;
      uint8_t t2 = temp & 0xff;
      Message::init(21, msgcnt, 0x70, BIDI | WKMEUP, t1, t2);
      pload[0] = humidity;
      pload[1] = (mc1p0 >> 8) & 0xff;
      pload[2] = mc1p0 & 0xff;         
      pload[3] = (mc2p5 >> 8) & 0xff;
      pload[4] = mc2p5 & 0xff;         
      pload[5] = (mc4p0 >> 8) & 0xff;
      pload[6] = mc4p0 & 0xff;         
      pload[7] = (mc10p0 >> 8) & 0xff;
      pload[8] = mc10p0 & 0xff;     
      pload[9] = pm_valid;    
    }
};

class PMDataMsg2 : public Message {
  public:
    void init(uint8_t msgcnt, uint16_t nc0p5, uint16_t nc1p0, uint16_t nc2p5, uint16_t nc4p0, uint16_t nc10p0, uint16_t partsize) {
      // Message Length (first byte param.): 11 + payload. Max. payload: 17 Bytes (https://www.youtube.com/watch?v=uAyzimU60jw)
      Message::init(23, msgcnt, 0x53, BIDI | WKMEUP, 0, 0);
      pload[0] = (nc0p5 >> 8) & 0xff;
      pload[1] = nc0p5 & 0xff;
      pload[2] = (nc1p0 >> 8) & 0xff;
      pload[3] = nc1p0 & 0xff;         
      pload[4] = (nc2p5 >> 8) & 0xff;
      pload[5] = nc2p5 & 0xff;         
      pload[6] = (nc4p0 >> 8) & 0xff;
      pload[7] = nc4p0 & 0xff;         
      pload[8] = (nc10p0 >> 8) & 0xff;
      pload[9] = nc10p0 & 0xff;  
      pload[10] = (partsize >> 8) & 0xff;
      pload[11] = partsize & 0xff;               
    }
};


class FanCleanTimer : public Alarm {
  bool      tbCleaned;
  byte      dayct;

public:
  FanCleanTimer () : Alarm(0), tbCleaned(false), dayct(FAN_CLEANING_INTERVAL - 1) {}
  virtual ~FanCleanTimer() {}

  virtual void trigger (AlarmClock& clock) {
    dayct++;
    if(dayct >= FAN_CLEANING_INTERVAL) {
      DPRINTLN("SPS30 fan cleaning required.");
      tbCleaned = true;
      dayct = 0;
    }
    set(seconds2ticks((uint32_t)60 * 60 * 24)); 
    clock.add(*this);       
  }

  void init(uint32_t period_ticks, AlarmClock& clock) {
    set(period_ticks);
    clock.add(*this);
  }  

  bool CleaningReq(void) {
    return(tbCleaned);
  }

  void CleaningReq(bool req) {
    tbCleaned = req;
  }
}; 

FanCleanTimer fanCleanTimer;


class SensChannel : public Channel<Hal, SensorList1, EmptyList, List4, PEERS_PER_CHANNEL, GDList0>, public Alarm {
    PMDataMsg1 msg1;
    PMDataMsg2 msg2;
    Sens_SHT31<0x44> sht31;  //SG: GY breakout board standard address
    Sens_SPS30 sps30;
    uint16_t millis;
    uint8_t ct;
    uint8_t sampling_period;
    struct sps30_measurement spsdta;

    struct sps30_measurement_int {
        uint16_t mc_1p0;
        uint16_t mc_2p5;
        uint16_t mc_4p0;
        uint16_t mc_10p0;
        uint16_t nc_0p5;
        uint16_t nc_1p0;
        uint16_t nc_2p5;
        uint16_t nc_4p0;
        uint16_t nc_10p0;
        uint16_t typical_particle_size;
    } spsint;
    

  public:
    SensChannel () : Channel(), Alarm(5), millis(0), ct(0), sampling_period(30) {}
    virtual ~SensChannel () {}

    virtual void trigger (__attribute__ ((unused)) AlarmClock& clock) {
      if (ct == 0) {
        measure_start();
      }
      else if ((ct > 0)&&(ct <= sampling_period)) {
        measure_loop();
      }
      else if (ct == (sampling_period + 1)) {
        measure_end();
      }
      clock.add(*this);     
    }


    void madd(struct sps30_measurement* m1, struct sps30_measurement* m2) {
      m1->mc_1p0 += m2->mc_1p0;
      m1->mc_2p5 += m2->mc_2p5;
      m1->mc_4p0 += m2->mc_4p0;
      m1->mc_10p0 += m2->mc_10p0;
      m1->nc_0p5 += m2->nc_0p5;      
      m1->nc_1p0 += m2->nc_1p0;   
      m1->nc_2p5 += m2->nc_2p5; 
      m1->nc_4p0 += m2->nc_4p0;   
      m1->nc_10p0 += m2->nc_10p0;    
      m1->typical_particle_size += m2->typical_particle_size;    
    }

    void mdiv(struct sps30_measurement* m1, float d) {
      m1->mc_1p0 /= d;
      m1->mc_2p5 /= d;
      m1->mc_4p0 /= d;
      m1->mc_10p0 /= d;
      m1->nc_0p5 /= d;      
      m1->nc_1p0 /= d;   
      m1->nc_2p5 /= d; 
      m1->nc_4p0 /= d;   
      m1->nc_10p0 /= d;    
      m1->typical_particle_size /= d;    
    }

    void mclr(struct sps30_measurement* m1) {
      m1->mc_1p0 = 0.0;
      m1->mc_2p5 = 0.0;
      m1->mc_4p0 = 0.0;
      m1->mc_10p0 = 0.0;
      m1->nc_0p5 = 0.0;      
      m1->nc_1p0 = 0.0;   
      m1->nc_2p5 = 0.0; 
      m1->nc_4p0 = 0.0;   
      m1->nc_10p0 = 0.0;    
      m1->typical_particle_size = 0.0;    
    }   

    void mint(struct sps30_measurement* m1, struct sps30_measurement_int* m2) {
      m2->mc_1p0 = round(m1->mc_1p0 * 10.0);
      m2->mc_2p5 = round(m1->mc_2p5 * 10.0);
      m2->mc_4p0 = round(m1->mc_4p0 * 10.0);
      m2->mc_10p0 = round(m1->mc_10p0 * 10.0);
      m2->nc_0p5 = round(m1->nc_0p5 * 10.0);      
      m2->nc_1p0 = round(m1->nc_1p0 * 10.0);   
      m2->nc_2p5 = round(m1->nc_2p5 * 10.0); 
      m2->nc_4p0 = round(m1->nc_4p0 * 10.0);   
      m2->nc_10p0 = round(m1->nc_10p0 * 10.0);    
      m2->typical_particle_size = round(m1->typical_particle_size * 100.0);       
    }
    

    void measure_start(void) {
       sampling_period = this->getList1().samplingPeriod();
       sht31.measure();
       if (sht31.humidity() <= this->getList1().humidityLimit()) {
         DPRINT("Starting SPS30, wait "); DDEC(sampling_period); DPRINTLN(" sec.");
         mclr(&spsdta);
         sps30.start_measurement();
         if(fanCleanTimer.CleaningReq()) {
          DPRINTLN("SPS30 fan cleaning started.");         
          sps30.start_manual_fan_cleaning();
          fanCleanTimer.CleaningReq(false);
          DPRINT("SPS30 next fan cleaning in "); DDEC(FAN_CLEANING_INTERVAL); DPRINTLN(" days.");         
         }         
         ct=1;
         tick = seconds2ticks(sampling_period); 
       }
       else {
        DPRINTLN("SPS30 not started, too wet.");       
        uint8_t msgcnt = device().nextcount();
        msg1.init( msgcnt, sht31.temperature(), sht31.humidity(), spsint.mc_1p0, spsint.mc_2p5, spsint.mc_4p0, spsint.mc_10p0, 0);
        device().sendPeerEvent(msg1, *this);        
        ct=0;
        tick = seconds2ticks(device().getList0().updIntervall());
       }
    }


    void measure_loop(void) {
      DPRINT("Taking measurement #"); DDEC(ct);
      sps30.measure();
      madd(&spsdta, &sps30.m);
      DPRINT("  PM10: "); DDECLN(spsdta.mc_10p0);
      ct++;
      tick = seconds2ticks(1); 
    }


    void measure_end(void) {
      DPRINTLN("SPS30 measurement cycle finished, sensor idle.");        
      mdiv(&spsdta, sampling_period);
      mint(&spsdta, &spsint);
      DPRINT("temp / hum = ");DDEC(sht31.temperature());DPRINT(" / ");DDECLN(sht31.humidity());
      DPRINT("mass concentration 1 / 2.5 / 4 / 10 = ");DDEC(spsdta.mc_1p0);DPRINT(" / ");DDEC(spsdta.mc_2p5);;DPRINT(" / ");DDEC(spsdta.mc_4p0);;DPRINT(" / ");DDECLN(spsdta.mc_10p0);
      DPRINT("numb concentration 0.5 / 1 / 2.5 / 4 / 10 = ");DDEC(spsdta.nc_0p5);DPRINT(" / ");DDEC(spsdta.nc_1p0);DPRINT(" / ");DDEC(spsdta.nc_2p5);;DPRINT(" / ");DDEC(spsdta.nc_4p0);;DPRINT(" / ");DDECLN(spsdta.nc_10p0);
      DPRINT("avg particle size = ");DDECLN(spsdta.typical_particle_size);     
            
      uint8_t msgcnt = device().nextcount();
      msg1.init( msgcnt, sht31.temperature(), sht31.humidity(), spsint.mc_1p0, spsint.mc_2p5, spsint.mc_4p0, spsint.mc_10p0, 1);
      device().sendPeerEvent(msg1, *this);
      msgcnt = device().nextcount();
      msg2.init( msgcnt, spsint.nc_0p5, spsint.nc_1p0, spsint.nc_2p5, spsint.nc_4p0, spsint.nc_10p0, spsint.typical_particle_size);
      device().sendPeerEvent(msg2, *this);    
      sps30.stop_measurement();
      ct=0;
      tick = seconds2ticks(max((int32_t)device().getList0().updIntervall() - 2 * sampling_period, 10));
      DPRINT("SPS next measurement cycle in "); DDEC(ticks2seconds(tick)); DPRINTLN(" seconds.");
    }


    void setup(Device<Hal, GDList0>* dev, uint8_t number, uint16_t addr) {
      Channel::setup(dev, number, addr);
      sht31.init();
      sps30.init();
      sysclock.add(*this);
    }

    uint8_t status () const {
      return 0;
    }

    uint8_t flags () const {
      return 0;
    }
};

class PMDevice : public MultiChannelDevice<Hal, SensChannel, 1, GDList0> {
  public:
    typedef MultiChannelDevice<Hal, SensChannel, 1, GDList0> TSDevice;
    
    PMDevice(const DeviceInfo& info, uint16_t addr) : TSDevice(info, addr) {}
    
    virtual ~PMDevice () {}

    virtual void configChanged () {
      TSDevice::configChanged();
      this->battery().low(this->getList0().lowBatLimit());
      DPRINTLN("* Config Changed       : List0");
      DPRINT(F("* LED Mode             : ")); DDECLN(this->getList0().ledMode());    
      DPRINT(F("* Sendeversuche        : ")); DDECLN(this->getList0().transmitDevTryMax());          
      DPRINT(F("* Sendeintervall       : ")); DDECLN(this->getList0().updIntervall());
    }
};


PMDevice sdev(devinfo, 0x20);
ConfigButton<PMDevice> cfgBtn(sdev);


void setup () {
  DINIT(57600, ASKSIN_PLUS_PLUS_IDENTIFIER);
  sdev.init(hal);
  buttonISR(cfgBtn, CONFIG_BUTTON_PIN);
  fanCleanTimer.init(seconds2ticks(10), sysclock);
  sdev.initDone();
}


void loop() {
  bool worked = hal.runready();
  bool poll = sdev.pollRadio();
  if ( worked == false && poll == false ) {
    hal.activity.savePower<Sleep<>>(hal);
  }
}
