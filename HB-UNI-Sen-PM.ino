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


// all library classes are placed in the namespace 'as'
using namespace as;

// define all device properties
const struct DeviceInfo PROGMEM devinfo = {
  {0xf8, 0x24, 0x01},     // Device ID
  "SGSENPM001",           // Device Serial
  {0xf8, 0x24},           // Device Model 
  0x10,                   // Firmware Version
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
      updIntervall(10);
    }
};


class PMDataMsg1 : public Message {
  public:
    void init(uint8_t msgcnt, int16_t temp, uint8_t humidity, uint16_t mc1p0, uint16_t mc2p5, uint16_t mc4p0, uint16_t mc10p0) {
      // Message Length (first byte param.): 11 + payload. Max. payload: 17 Bytes (https://www.youtube.com/watch?v=uAyzimU60jw)
      uint8_t t1 = (temp >> 8) & 0x7f;
      uint8_t t2 = temp & 0xff;
      Message::init(20, msgcnt, 0x70, BIDI | WKMEUP, t1, t2);
      pload[0] = humidity & 0xff;
      pload[1] = (mc1p0 >> 8) & 0xff;
      pload[2] = mc1p0 & 0xff;         
      pload[3] = (mc2p5 >> 8) & 0xff;
      pload[4] = mc2p5 & 0xff;         
      pload[5] = (mc4p0 >> 8) & 0xff;
      pload[6] = mc4p0 & 0xff;         
      pload[7] = (mc10p0 >> 8) & 0xff;
      pload[8] = mc10p0 & 0xff;         
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


class SensChannel : public Channel<Hal, List1, EmptyList, List4, PEERS_PER_CHANNEL, GDList0>, public Alarm {
    PMDataMsg1 msg1;
    PMDataMsg2 msg2;
    Sens_SHT31<0x44> sht31;  //SG: GY breakout board standard address
    Sens_SPS30 sps30;
    uint16_t millis;

  public:
    SensChannel () : Channel(), Alarm(5), millis(0) {}
    virtual ~SensChannel () {}

    virtual void trigger (__attribute__ ((unused)) AlarmClock& clock) {
      // reactivate for next measure
      tick = seconds2ticks(10);
      clock.add(*this);
      sht31.measure();
      sps30.measure();
      DPRINT("temp / hum = ");DDEC(sht31.temperature());DPRINT(" / ");DDECLN(sht31.humidity());
      DPRINT("mass concentration 1 / 2.5 / 4 / 10 = ");DDEC(sps30.mc_1p0());DPRINT(" / ");DDEC(sps30.mc_2p5());;DPRINT(" / ");DDEC(sps30.mc_4p0());;DPRINT(" / ");DDECLN(sps30.mc_10p0());
      DPRINT("numb concentration 0.5 / 1 / 2.5 / 4 / 10 = ");DDEC(sps30.nc_0p5());DPRINT(" / ");DDEC(sps30.nc_1p0());DPRINT(" / ");DDEC(sps30.nc_2p5());;DPRINT(" / ");DDEC(sps30.nc_4p0());;DPRINT(" / ");DDECLN(sps30.nc_10p0());
      DPRINT("avg particle size = ");DDECLN(sps30.partsize());
            
      uint8_t msgcnt = device().nextcount();
      //msg1.init( msgcnt, sht31.temperature(), sht31.humidity(), sps30.mc_1p0(), sps30.mc_2p5(), sps30.mc_4p0(), sps30.mc_10p0());
      msg1.init( msgcnt, 0x0333, 0x55, sps30.mc_1p0(), sps30.mc_2p5(), sps30.mc_4p0(), sps30.mc_10p0());
      device().sendPeerEvent(msg1, *this);
      msgcnt = device().nextcount();
      msg2.init( msgcnt, sps30.nc_0p5(), sps30.nc_1p0(), sps30.nc_2p5(), sps30.nc_4p0(), sps30.nc_10p0(), sps30.partsize());
      device().sendPeerEvent(msg2, *this);      
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
      DPRINT(F("* Low Bat Limit        : ")); DDECLN(this->getList0().lowBatLimit()); 
      DPRINT(F("* Sendeversuche        : ")); DDECLN(this->getList0().transmitDevTryMax());          
      DPRINT(F("* SENDEINTERVALL       : ")); DDECLN(this->getList0().updIntervall());
    }
};


PMDevice sdev(devinfo, 0x20);
ConfigButton<PMDevice> cfgBtn(sdev);


void setup () {
  DINIT(57600, ASKSIN_PLUS_PLUS_IDENTIFIER);
  sdev.init(hal);
  buttonISR(cfgBtn, CONFIG_BUTTON_PIN);
  sdev.initDone();
}


void loop() {
  bool worked = hal.runready();
  bool poll = sdev.pollRadio();
  if ( worked == false && poll == false ) {
    // if nothing to do - go to sleep    
    hal.activity.savePower<Sleep<>>(hal);
  }
}
