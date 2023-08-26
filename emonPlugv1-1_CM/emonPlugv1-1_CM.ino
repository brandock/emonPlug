/* emonPlug v1-1 Continuous Sampling
 * Brandon Baldock August 2023
 * Firmware for version v1-1 of emonPlug
 * 
 * This is a modified version of emonTx34_CM by Robin Emley, Robert Wall, and Trystan Lea.
 * 
 * It sends MSG,Vrms,Irms,PowerFactor,Frequency,LogPeriod,RealPower,ApparentPower,WattHours.
 * 
 * The following entry is needed for emonHub.

[[28]]
  nodename = emonPlugCM
  [[[rx]]]
    names =  MSG,Vrms,Irms,PowerFactor,Frequency,LogPeriod,RealPower,ApparentPower,WattHours
    datacodes = L,h,h,h,h,h,h,h,L
    scales = 1,.01,.01,.01,.01,.01,1,1,1
    units = n,V,A,n,Hz,s,W,VA,Wh

 * The changes to the original emonTx34 sketch are as follows.
 * Changed default node from 15 to 28.
 * Changed network group to 100: byte networkGroup = 100;  
 * 
 * sendWithRetry(5, ...) -- my basestation is not typically 5, so I just broadcast to 0
        //radio.send(0, (const void*)(&emontx), sizeof(emontx));
        radio.sendWithRetry(0, (const void*)(&emontx), sizeof(emontx), 2, 30);
 * 
 * Set the default for the USA boolean to True. bool  USA=true;
 * 
 * Changed vCal_USA, i1Cal and i1Lead as follows
 *  float vCal_USA = 111.686;  
 *  float i1Cal = 14.68;                                       
 *  float i1Lead = 7.6;                                       
 *  ...
 *  float assumedVrms = 120.00;                               // Assumed Vrms when no a.c. is detected
 *  float lineFreq = 60; 
 *  
 * Added this to list_calibration in emonTx34_CM_config: if (USA) Serial.print(F("USA mode enabled. vCal_USA = ")); Serial.println(EEProm.vCal_USA); 
 * 
 * Removed all the pulse and temperature measurement code.
 * 
 * Removed CT2, CT3, and CT4, which are not present in emonPlug
 *
 * Change Log:
 * v1.0.0: First version for emonPlug, based on v2.4.0 emonTx34_CM firmware
 */

const char *firmware_version = {"1.1.0\n\r"};


#define RFM69_JEELIB_CLASSIC 1
#define RFM69_JEELIB_NATIVE 2
#define RFM69_LOW_POWER_LABS 3

#define RadioFormat RFM69_LOW_POWER_LABS

// Comment/Uncomment as applicable
#define DEBUG                                              // Debug level print out

// #define EEWL_DEBUG

#define FACTORYTESTGROUP 1                                 // Transmit the Factory Test on Grp 1 
                                                           //   to avoid interference with recorded data at power-up.
#define OFF HIGH
#define ON LOW

#define RFM69CW

#include <Arduino.h>
#include <avr/wdt.h>

#if RadioFormat == RFM69_LOW_POWER_LABS
  #include <RFM69.h>                             // RFM69 LowPowerLabs radio library
#elif RadioFormat == RFM69_JEELIB_CLASSIC
  #include <rfmTxLib.h>                          // RFM69 transmit-only library using "JeeLib RFM69 Native" message format
#elif RadioFormat == RFM69_JEELIB_NATIVE
  #include <rfm69nTxLib.h>                       // RFM69 transmit-only library using "JeeLib RFM69 Native" message format
#endif

#include <emonEProm.h>                                     // OEM EEPROM library
#include <emonLibCM.h>                                     // OEM Continuous Monitoring library

// Include emonTx34_CM_config.ino in the same directory for settings functions & data

// Radio - checks for traffic
const int busyThreshold = -97;                             // Signal level below which the radio channel is clear to transmit
const byte busyTimeout = 15;                               // Time in ms to wait for the channel to become clear, before transmitting anyway

typedef struct {
    unsigned long Msg;
    //The library functions return decimal values, so these values need to be multiplied by 100 to store and transmit, then divided by 100 in emonHub config
    int Vrms;   //Avg Vrms over the reporting period
    int IRMS1;  //Avg Irms over the reporting period
    int PF1;    //Avg power factor over the reporting period
    int Freq;   //Avg frequency of the AC power over the reporting period
    int LogPeriod; //Reporting period in seconds 
    //The library function returns integer values for the following
    int P1;       //Average real power in watts over the reporting period
    int AP1;      //Average apparent power in watts over the reporting period
    long E1; 
} PayloadTX;
PayloadTX emontx;                                                  // create a data packet for the RFM
static void showString (PGM_P s);
 
 
//---------------------------- emonTx Settings - Stored in EEPROM and shared with config.ino ------------------------------------------------
struct {
  byte RF_freq = RF69_433MHZ;                               // Frequency of radio module can be RF69_433MHZ, RF69_868MHZ or RF69_915MHZ. 
  byte networkGroup = 100;                                 // wireless network group, must be the same as emonBase / emonPi and emonGLCD. OEM default is 210
  byte  nodeID = 27;                                       // node ID for this emonTx.
  byte  rf_on = 1;                                         // RF - 0 = no RF, 1 = RF on.
  byte  rfPower = 31;                                      // 7 = -10.5 dBm, 25 = +7 dBm for RFM12B; 0 = -18 dBm, 31 = +13 dBm for RFM69CW. Default = 25 (+7 dBm)
  float vCal  = 268.97;                                    // (240V x 13) / 11.6V = 268.97 Calibration for UK AC-AC adapter 77DB-06-09
  float vCal_USA = 111.686;                                  // (120V × 13) / 12.0V = 130.0  Calibration for US AC-AC adapter 77DA-10-09 
  float assumedVrms = 120.00;                               // Assumed Vrms when no a.c. is detected
  float lineFreq = 60;                                     // Line Frequency = 50 Hz
  float i1Cal = 14.68;                                      // (100 A / 50 mA / 22 Ohm burden) = 90.9
  float i1Lead = 7.6;                                      // 4.2° phase lead
  float period = 9.85;                                     // datalogging period - should be fractionally less than the PHPFINA database period in emonCMS
  bool  showCurrents = false;                              // Print to serial voltage, current & p.f. values
  bool  json_enabled = false;                              // JSON Enabled - false = key,Value pair, true = JSON, default = false: Key,Value pair.  
} EEProm;

uint16_t eepromSig = 0x0015;                               // oemEProm signature - see oemEProm Library documentation for details.

#ifdef EEWL_DEBUG
  extern EEWL EVmem;
#endif

#if RadioFormat == RFM69_LOW_POWER_LABS
  RFM69 radio;
#endif


bool  USA=true;

bool calibration_enable = true;                            // Enable on-line calibration when running. 
                                                           // For safety, thus MUST default to false. (Required due to faulty ESP8266 software.)

//---------------------------------CT availability status----------------------------------------
byte CT_count = 0;
bool CT1;     // Record if CT present during startup


//----------------------------------------Setup--------------------------------------------------
void setup() 
{  
  wdt_enable(WDTO_8S);
    
  // Serial---------------------------------------------------------------------------------
  Serial.begin(115200);

  // ---------------------------------------------------------------------------------------
  #ifdef DEBUG
    Serial.print(F("emonPlug Continuous Monitoring V")); Serial.write(firmware_version);
    Serial.println(F("OpenEnergyMonitor.org"));
  #else
    Serial.println(F("describe:EmonTX3CM"));
  #endif
  
  #if RadioFormat == RFM69_JEELIB_CLASSIC
    EEProm.rf_on = 2;
  #endif
 
  load_config(true);                                                   // Load RF config from EEPROM (if any exists)
  
  if (EEProm.rf_on)
  {
    #ifdef DEBUG
      #ifdef RFM12B
      Serial.print(F("RFM12B "));
      #endif
      #ifdef RFM69CW
      Serial.print(F("RFM69CW "));
      #endif
      Serial.print(F(" Freq: "));
      if (EEProm.RF_freq == RF69_433MHZ) Serial.print(F("433MHz"));
      if (EEProm.RF_freq == RF69_868MHZ) Serial.print(F("868MHz"));
      if (EEProm.RF_freq == RF69_915MHZ) Serial.print(F("915MHz"));
      Serial.print(F(" Group: ")); Serial.print(EEProm.networkGroup);
      Serial.print(F(" Node: ")); Serial.print(EEProm.nodeID);
      Serial.println(F(" "));
    #endif
  }
  

  // Check connected CT sensors ------------------------------------------------------------
  if (analogRead(1) > 0) {CT1 = 1; CT_count++;} else CT1=0;            // check to see if CT is connected to CT1 input

  // ---------------------------------------------------------------------------------------

  if (EEProm.rf_on)
  {
    Serial.println(F("Factory Test"));
    #if RadioFormat == RFM69_LOW_POWER_LABS
      radio.initialize(RF69_433MHZ,EEProm.nodeID,EEProm.networkGroup);  
      radio.encrypt("89txbe4p8aik5kt3");                                                      // initialize RFM
      radio.setPowerLevel(EEProm.rfPower);
    #else
      rfm_init();                                                        // initialize RFM
    #endif
    for (int i=10; i>=0; i--)                                          // Send RF test sequence using test Group (for factory testing)
    {
      emontx.P1=i;
      #if RadioFormat == RFM69_LOW_POWER_LABS
        radio.send(1, (const void*)(&emontx), sizeof(emontx));  
        showString(PSTR(" -> "));   
        Serial.print((word) sizeof(emontx));    
        showString(PSTR(" b\n"));
      #else
        PayloadTX tmp = emontx;
        if (EEProm.rf_on == 2) {
            byte WHITENING = 0x55;
            for (byte i = 0, *p = (byte *)&tmp; i < sizeof tmp; i++, p++)
                *p ^= (byte)WHITENING;
        }
        rfm_send((byte *)&tmp, sizeof(tmp), FACTORYTESTGROUP, EEProm.nodeID, EEProm.RF_freq, EEProm.rfPower, busyThreshold, busyTimeout);
      #endif
      delay(100);
    }
    #if RadioFormat == RFM69_LOW_POWER_LABS
      radio.sleep();
    #endif
    emontx.P1=0;
    delay(EEProm.nodeID * 20);                                 // try to avoid r.f. collisions at start-up
  }
  
  // ---------------------------------------------------------------------------------------
  #ifdef DEBUG
    if (CT_count==0) {
      Serial.println(F("NO CT's detected"));
    } else {
      if (CT1) { Serial.print(F("CT1 detected, i1Cal:")); Serial.println(EEProm.i1Cal); }
    }
    delay(200);
  #endif

#ifdef EEWL_DEBUG
  Serial.print("End of mem=");Serial.print(E2END);
  Serial.print("  Avail mem=");Serial.print((E2END>>2) * 3);
  Serial.print("  Start addr=");Serial.print(E2END - (((E2END>>2) * 3) / (sizeof(mem)+1))*(sizeof(mem)+1));
  Serial.print("  Num blocks=");Serial.println(((E2END>>2) * 3) / 21);
  EVmem.dump_buffer();
#endif

  // ----------------------------------------------------------------------------
  // EmonLibCM config
  // ----------------------------------------------------------------------------
  EmonLibCM_SetADC_VChannel(0, EEProm.vCal);                           // ADC Input channel, voltage calibration - for Ideal UK Adapter = 268.97
  if (USA) EmonLibCM_SetADC_VChannel(0, EEProm.vCal_USA);              // ADC Input channel, voltage calibration - for US AC-AC adapter 77DA-10-09
  EmonLibCM_SetADC_IChannel(1, EEProm.i1Cal, EEProm.i1Lead);           // ADC Input channel, current calibration, phase calibration
  EmonLibCM_ADCCal(3.3);                                               // ADC Reference voltage, (3.3 V for emonTx,  5.0 V for Arduino)
  if (USA) EmonLibCM_cycles_per_second(60);                            // mains frequency 60Hz
  EmonLibCM_datalog_period(EEProm.period);                             // period of readings in seconds - normal value for emoncms.org  
  EmonLibCM_setAssumedVrms(EEProm.assumedVrms);

#ifdef EEWL_DEBUG
  EVmem.dump_control();
  EVmem.dump_buffer();  
#endif
  
  EmonLibCM_Init();                                                    // Start continuous monitoring.
  emontx.Msg = 0;

  long e0=0, e1=0, e2=0, e3=0;
  unsigned long p=0;
  recoverEValues(&e0,&e1,&e2,&e3,&p);
  EmonLibCM_setWattHour(0, e0);

  // Speed up startup by making first reading 2s
  EmonLibCM_datalog_period(2.0);
}

void loop()             
{
  getSettings();
  
  if (EmonLibCM_Ready())   
  {
    #ifdef DEBUG
    if (emontx.Msg==0) 
    {

      EmonLibCM_datalog_period(EEProm.period); 
      if (EmonLibCM_acPresent())
        Serial.println(F("AC present - Real Power calc enabled"));
      else
      {
        Serial.print(F("AC missing - Apparent Power calc enabled, assuming ")); Serial.print(EEProm.assumedVrms); Serial.println(F(" V"));
      }
    }
    delay(5);
    #endif

    emontx.Msg++;

    if (EmonLibCM_acPresent()) {
      emontx.Vrms = EmonLibCM_getVrms() * 100;
    } else {
      emontx.Vrms = EmonLibCM_getAssumedVrms() * 100;
    }
    
    emontx.IRMS1 = EmonLibCM_getIrms(0) * 100;
    emontx.PF1 = EmonLibCM_getPF(0) * 100;

    emontx.Freq = EmonLibCM_getLineFrequency() * 100;
    emontx.LogPeriod = EmonLibCM_getDatalog_period() * 100;
    
    emontx.P1 = EmonLibCM_getRealPower(0); 
    emontx.AP1 = EmonLibCM_getApparentPower(0);
    emontx.E1 = EmonLibCM_getWattHour(0); 
   
    if (EEProm.rf_on)
    {
      #if RadioFormat == RFM69_LOW_POWER_LABS
        radio.sendWithRetry(0, (const void*)(&emontx), sizeof(emontx), 2, 30);
        //radio.sendWithRetry(5, (const void*)(&emontx), sizeof(emontx));
        //radio.send(0, (const void*)(&emontx), sizeof(emontx));
        radio.sleep();
      #else
        PayloadTX tmp = emontx;
        if (EEProm.rf_on == 2) {
            byte WHITENING = 0x55;
            for (byte i = 0, *p = (byte *)&tmp; i < sizeof tmp; i++, p++)
                *p ^= (byte)WHITENING;
        }
        rfm_send((byte *)&tmp, sizeof(tmp), EEProm.networkGroup, EEProm.nodeID, EEProm.RF_freq, EEProm.rfPower, busyThreshold, busyTimeout);     //send data
      #endif
      
      delay(50);
    }

    if (EEProm.json_enabled) {
      // ---------------------------------------------------------------------
      // JSON Format
      // ---------------------------------------------------------------------
      Serial.print(F("{\"MSG\":")); Serial.print(emontx.Msg);
      Serial.print(F(",\"Vrms\":")); Serial.print(emontx.Vrms*0.01);

      if (CT1) { Serial.print(F(",\"P1\":")); Serial.print(emontx.P1); }
      Serial.println(F("}"));
      delay(60);
      
    } else {
  
      // ---------------------------------------------------------------------
      // Key:Value format, used by EmonESP & emonhub EmonHubOEMInterfacer
      // ---------------------------------------------------------------------
      Serial.print(F("MSG:")); Serial.print(emontx.Msg);
      Serial.print(F(",Vrms:")); Serial.print(emontx.Vrms*0.01);
      
      if (CT1) { Serial.print(F(",P1:")); Serial.print(emontx.P1); }        
      if (CT1) { Serial.print(F(",E1:")); Serial.print(emontx.E1); }

      if (!EEProm.showCurrents) {
        Serial.println();
        delay(40);
      } else {
        // to show voltage, current & power factor for calibration:
        Serial.print(F(",I1:")); Serial.print(EmonLibCM_getIrms(EmonLibCM_getLogicalChannel(1)),3);
        Serial.print(F(",pf1:")); Serial.println(EmonLibCM_getPF(EmonLibCM_getLogicalChannel(1)),4);
         delay(80);
      }
    }

    // End of print out ----------------------------------------------------
    
    storeEValues(emontx.E1,0,0,0,0);  //EmonEprom will check if E1 is 200 Wh greater than the last stored value. If not, it skips this update. If so, it stores the new value. Saves on writes.
  }
  wdt_reset();
  delay(20);
}
