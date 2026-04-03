
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <time.h>
#include <iio/iio.h>
#include <unistd.h>
#include <lgpio.h>
#include <stdio.h>
#include <string.h>
#include "Touch.h"
#include "Graphics.h"
#include "Mouse.h"
#include "hmi.h"
#include "Morse.c"
#include <netdb.h>
#include <netinet/in.h>                                                                                                      
#include <arpa/inet.h>

#define MAX_FILENAME_LENGTH 255

//#define PLUTOIP "ip:pluto.local"

char plutoip[30];


void setFreq(double fr);
void displayFreq(double fr);
void setFreqInc();
void setTx(int ptt);
void setPtts(int p);
void setMode(int mode);
void setVolume(int vol);
void setMute(int m);
void setSquelch(int sql);
void setSSBMic(int mic);
void setFMMic(int mic);
void setAMMic(int mic);
void setRxFilter(int low,int high);
void setTxFilter(int low,int high);
void setBandBits(int b);
void processTouch();
void processMouse(int mbut);
void processHmi(int hm);
void setRotation(int rot);
void initGUI();
void sendFifo(char * s);
void initFifos();
void initUDP(void);
void initPluto(void);
void setPlutoRxFreq(long long rxfreq);
void setPlutoTxFreq(long long rxfreq);
void setHwRxFreq(double fr);
void setHwTxFreq(double fr);
void PlutoTxEnable(int txon);
void PlutoRxEnable(int rxon);
void detectHw();
int buttonTouched(int bx,int by);
void setKey(int k);
void displayMenu(void);
void showSettingsMenu(void);
void showExtraMenu(void);
void showNB1SettingsMenu(void);
void displayNB1Setting(int se);
void showCOMPSettingsMenu(void);
void displayCOMPSetting(int se);
void sendCOMPParams(void);
void changeCompSetting(void);
void sendNB1Params(void);
void displaySetting(int se);
void changeSetting(void);
void changeNB1Setting(void);
void processGPIO(void);
void initGPIO(void);
int readConfig(void);
int writeConfig(void);
int satMode(void);
int splitMode(void);
int txvtrMode(void);
int duplexMode(void);
int multMode(void);
void setMoni(int m);
void initSDR(void);
void waterfall(void);
void clearWaterfall(void);
void P_Meter(void);
void S_Meter(void);
void setRit(int rit);
void setInputMode(int n);
void gen_palette(char colours[][3],int num_grads);
void setPlutoTxAtt(int att);
void setPlutoRxGain(int gain);
int readPlutoRxGain(void);
void setBand(int b);
void setPlutoGpo(int p);
void setTxPin(int v);
void setAlsaCapture(int level);
long long runTimeMs(void);                                                    
void clearPopUp(void);
void displayPopupMode(void);
void displayPopupBand(void);
void send1750(void);
void setCTCSS(int t);
void displayError(char*st);
void flushUDP(void);
void setFFTBW(int bw);
void startGNURadio(void);
void restartGNURadio(void);

int wr_ch_lli(struct iio_channel *chn, const char* what, long long val);
int wr_ch_bool(struct iio_channel *chn, const char* what, bool val);
int wr_ch_dbl(struct iio_channel *chn, const char* what, double val);
int wr_ch_str(struct iio_channel *chn, const char* what, const char* str);
int wr_db_str(struct iio_device *dev, const char* what, const char* str);

int minGain(double freq);
int maxGain(double freq);
void setDialLock(int d);
void setBeacon(int b);

FILE *openFile( char *fn, char *mode );
void displayArrowButtons(void);
int arrowButtonTouched(int bx, int by);

int firstpass=1;
double freq;
double freqInc=0.001;
#define numband 24
int band=3;
#define nummode 6
double bandFreq[numband] = {70.200,144.200,432.200,1296.200,2320.200,2400.100,3400.100,5760.100,10368.200,24048.200,47088.2,10489.55,433.2,433.2,433.2,433.2,433.2,433.2,1296.2,1296.2,1296.2,1296.2,1296.2,1296.2};
double bandTxOffset[numband]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-9936.0,-23616.0,-46656.0,-10069.5,0,0,0,0,0,0,0,0,0,0,0,0};
double bandRxOffset[numband]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-9936.0,-23616.0,-46656.0,-10345.0,0,0,0,0,0,0,0,0,0,0,0,0};
double bandRepShift[numband]={0,-0.6,1.6,-6.0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int bandTxHarmonic[numband]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
int bandRxHarmonic[numband]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
int bandMode[numband]={0};
int bandBitsRx[numband]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
int bandBitsTx[numband]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
int bandSquelch[numband][nummode]={0};
int bandFFTRef[numband]={-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10,-10};
int bandFFTRange[numband]={80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80};
int bandTxAtt[numband]={-89,-89,-89,-89,-89,-89,-89,-89,-89,-89,-89,-89,-89,-89,-89,-89,-89,-89,-89,-89,-89,-89,-89,-89};
int bandRxGain[numband]={100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100};              //100 is automatic gain
int bandDuplex[numband]={0};
int bandCTCSS[numband]={0};
float bandSmeterZero[numband]={-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80,-80};
int bandSSBFiltLow[numband]={300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300};
int bandSSBFiltHigh[numband]={3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000};
int bandFFTBW[numband]={0};

#define minFreq 0.0
#define maxFreq 99999.99999
#define minHwFreq 69.9
#define maxHwFreq 5999.99999



int mode=0;
int lastmode=0;
char * modename[nummode]={"USB","LSB","CW ","CWN","FM ","AM "};
enum {USB,LSB,CW,CWN,FM,AM};

#define numSettings 28

char * settingText[numSettings]={"Rotate Screen = ","Rx Gain= ","SSB Mic Gain= ","FM Mic Gain= ","AM Mic Gain= ","Repeater Shift= ","CTCSS= "," Rx Offset= ","Rx Harmonic Mixing= "," Tx Offset= ","Tx Harmonic Mixing= ","Band Bits (Rx)= ","Band Bits (Tx)= ","Copy Band Bits to Pluto=","FFT Ref= ","Tx Att= ","S-Meter Zero= ", "SSB Rx Filter Low= ", "SSB Rx Filter High= ","CW Ident= ", "CWID Carrier= ", "CW Break-In Hang Time= ", "24 Bands= ", "GPIO PTT= ", "CW Key= ", "Pluto TX Out= ", "Mic Capture= ", "FFT Range= "};
enum {ROTATE,RX_GAIN,SSB_MIC,FM_MIC,AM_MIC,REP_SHIFT,CTCSS,RX_OFFSET,RX_HARMONIC,TX_OFFSET,TX_HARMONIC,BAND_BITS_RX,BAND_BITS_TX,BAND_BITS_TO_PLUTO,FFT_REF,TX_ATT,S_ZERO,SSB_FILT_LOW,SSB_FILT_HIGH,CWID,CW_CARRIER,BREAK_IN_TIME,BANDS24,ENABLE_GPIO_PTT,ENABLE_CW_KEY,ENABLE_PLUTO_TX,ALSA_CAPTURE,FFT_RANGE};
int settingNo=RX_GAIN;
int setIndex=0;
int maxSetIndex=10;

enum {FREQ,SETTINGS,VOLUME,SQUELCH,RIT,EXTRA,NB1_SETTINGS,COMP_SETTINGS};
int inputMode=FREQ;

#define NUMCTCSS 52
int CTCSSTone[NUMCTCSS] = {0,670,693,719,744,770,797,825,854,885,915,948,974,1000,1035,1072,1109,1148,1188,1230,1273,1318,1365,1413,1462,1500,1514,1567,1598,1622,1655,1679,1713,1738,1773,1799,1835,1862,1899,1928,1966,1995,2035,2065,2107,2181,2257,2291,2336,2418,2503,2541};


//GUI Layout values X and Y coordinates for each group of buttons.

#define volButtonX 660
#define volButtonY 300
#define sqlButtonX 30
#define sqlButtonY 300
#define ritButtonX 670
#define ritButtonY 60
#define funcButtonsY 429
#define funcButtonsX 30
#define buttonHeight 50
#define buttonSpaceY 55
#define buttonWidth 100
#define buttonSpaceX 105
#define freqDisplayX 80
#define freqDisplayY 55
#define freqDisplayCharWidth 48 
#define freqDisplayCharHeight 54
#define txX 600
#define txY 15
#define modeX 200
#define modeY 15
#define txvtrX 400
#define txvtrY 15
#define moniX 500
#define moniY 15
#define settingX 200
#define settingY 390
#define popupX 30
#define popupY 374
#define FFTX 140
#define FFTY 216
#define sMeterX 2
#define sMeterY 5
#define sMeterWidth 170
#define sMeterHeight 40
#define errorX 200
#define errorY 240
#define diallockX 316
#define diallockY 15

// Arrow buttons (35x35) arranged as a d-pad on the left side, just below RIT button 2 (y=165)
// Total height: 3*35 + 2*1 = 107px  =>  y=163 to y=269, clears SQL value display at y=275
#define arrowButtonSize 35
#define arrowUpX     51
#define arrowUpY    148
#define arrowDownX   51
#define arrowDownY  219
#define arrowLeftX   15
#define arrowLeftY  183
#define arrowRightX  87
#define arrowRightY 183



int gpiohandle;
int ptt=0;
int ptts=0;
int moni=0;
int fifofd;
int sendBeacon=0;
int dotCount=0;
int transmitting=0;
int dialLock=0;
int squelchGate=0;
int lastSquelchGate=0;

int rxFilterLow;
int rxFilterHigh;

int bands24 = 0;
int screenrotate = 0;

int keyDownTimer=0;
int CWIDkeyDownTime=1000;                     //time to put key down between CW Idents (100 per second)

#define configDelay 500                              //delay before config is written after tuning (5 Seconds)
int configCounter=configDelay;

int twoButTimer=0;
int lastBut=0;

int breakInTimer=0;
int breakInTime=100;

long long lastLOhz;

long long lastClock;
long progStartTime=0;

int lastKey=1;

int sMeterType = 0;

int volume=20;
#define maxvol 100

int squelch=20;
#define maxsql 100

int rit=0;
#define minrit -9990
#define maxrit 9990

int SSBMic=50;
#define maxSSBMic 100

int FMMic=50;
#define maxFMMic 100

int AMMic=20;
#define maxAMMic 100

int TxAtt=-89;

int enableGPIOPTT=0;    // GPIO 17 PTT enabled (0=disabled, 1=enabled)
int enableCWKey=0;      // CW key enabled (0=disabled, 1=enabled)
int enablePlutoTx=0;    // Pluto TX output enabled (0=disabled, 1=enabled)
int nb1Active=0;        // NB1 spectral noise reduction active
int nb1Algorithm=0;     // 0=Wiener 1=Over-subtract 2=MMSE
int nb1FftSize=256;     // 64/128/256/512/1024
int nb1Overlap=4;       // 2/4/8/16
int nb1Alpha=98;        // ×100 → 0.98
int nb1Beta=20;         // ×10  → 2.0
int nb1GainFloor=10;    // ×1000 → 0.010
int nb1SettingNo=0;
#define numNB1Settings 6
char * nb1SettingText[numNB1Settings]={"Algorithm= ","FFT Size= ","Overlap= ","Alpha= ","Beta= ","Gain Floor= "};

int compActive=0;         // TX compressor active
int compAgcAttack=41;     // ×1000 → 0.041
int compAgcDecay=33;      // ×1000 → 0.033
int compAgcRef=90;        // ×100  → 0.90
int compAgcMax=100;       // integer → 100.0
int compLpfCutoff=2700;   // Hz
int compEq1Freq=500;  int compEq1Gain=0;    // EQ band 1: ×10 dB → 0.0 dB
int compEq2Freq=1200; int compEq2Gain=40;   // EQ band 2: ×10 dB → +4.0 dB
int compEq3Freq=2500; int compEq3Gain=60;   // EQ band 3: ×10 dB → +6.0 dB
int compMicGain=50;                         // 0-100 → GNU Radio gain 0.0-1.0 when COMP active
int alsaCaptureLevel=70;                    // 0-100 % for ALSA Mic Capture Volume

int compSettingNo=0;
#define numCOMPSettings 12
char * compSettingText[numCOMPSettings]={"Attack= ","Decay= ","Ref= ","Max Gain= ","LPF Cut= ","EQ1 Freq= ","EQ1 Gain= ","EQ2 Freq= ","EQ2 Gain= ","EQ3 Freq= ","EQ3 Gain= ","Mic Gain= "};

int tuneDigit=8;
#define maxTuneDigit 11

#define TXDELAY 10000      //10ms delay between setting Tx output bit and sending tx command to SDR
#define RXDELAY 10000       //10ms delay between sending rx command to SDR and setting Tx output bit low. 

#define BurstLength 500000     //length of 1750Hz Burst   500ms

char mousePath[30];
char touchPath[30];
char hmiPath[30];
int mousePresent;
int touchPresent;
int plutoPresent;
int hmiPresent;
int portsdownPresent;
int V2Display =0 ;



int bandBitsToPluto=0;      //copy low 3 band bits to pluto IO1-IO3

int popupSel=0;
int popupFirstBand;
enum {NONE,MODE,BAND,BEACON};

#define pttPin 17       // Physical pin is 11
#define keyPin 18       // Physical pin is 12
#define txPin 21        // Physical pin is 40      
#define bandPin1 1      //Physical pin is 28
#define bandPin1alt 12  //Physical pin is 32     bandPin1 is copied to both of these pins to retain compatibility with Portsdown.
#define bandPin2 19     //Physical pin is 35
#define bandPin3 4      //Physical pin is 7
#define bandPin4 25     //Physical pin is 22
#define bandPin5 22     //Physical pin is 16
#define bandPin6 24     //Physical pin is 18
#define bandPin7 10     //Physical pin is 19
#define bandPin8 9      //Physical pin is 21


int plutoGpo=0;

//robs Waterfall

float inbuf[2];
FILE *fftstream;
FILE *txfftstream;
float buf[512][130];
int points=512;
int rows=130;
int FFTRef = -30;
int FFTRange = 80;
int spectrum_rows=80;
unsigned char * palette;
int HzPerBin=94;                        //calculated from FFT width and number of samples. Width=48000 number of samples =512
int bwBarStart=3;
int bwBarEnd=34;
float sMeter;                             //peak reading S meter.
float sMeterPeak;

struct iio_context *plutoctx;
struct iio_device *plutophy;

//UDP server to receive FFT data from GNU RAdio

#define RXPORT 7373
#define TXPORT 7474

#define FFTTIMEOUT 200                //timeout for FFT data 200 * 10ms = 2 seconds

int FFTTimeout;




int main(int argc, char* argv[])
{
  strcpy(plutoip,"ip:");
  char * penv = getenv("PLUTO_IP");
  if(penv==NULL)
    {
      strcpy(penv,"pluto.local");
    }
  strcat(plutoip,penv);

  printf("plutoip = %s\n",plutoip);

  initUDP();


  lastClock=0;
  readConfig();
  setRotation(screenrotate);
  detectHw();
  initPluto();
  startGNURadio();       //start GNU Radio after Pluto is initialised to avoid IIO contention
  initFifos();
  initScreen();
  initGPIO();
  printf("Initialising Touch at %s\n",touchPath);
  if(touchPresent) initTouch(touchPath);
  printf("Initialising Mouse at %s\n",mousePath);
  if(mousePresent) initMouse(mousePath);
  if(hmiPresent) initHmi(hmiPath);
  initGUI(); 
  initSDR(); 
  //              RGB Vals   Black >  Blue  >  Green  >  Yellow   >   Red     4 gradients    //number of gradients is varaible
  gen_palette((char [][3]){ {0,0,0},{0,0,255},{0,255,0},{255,255,0},{255,0,0}},4);

  
  while(1)
  {

    processGPIO();
                                                                                                                    
   if(touchPresent)
     {
       if(getTouch()==1)
        {
         processTouch();
        }
     }
    
    if(mousePresent)
      {
        int but=getMouse();
        if(but>0)
          {
             processMouse(but);
          }
       if(twoButTimer>0) 
       {
        twoButTimer--;
        if(twoButTimer==0)
          {
            lastBut=0;
          }
       }
           
      }
      
    if(hmiPresent)
      {
        int hr=getHmi();
        if(hr > 0)
          {
          processHmi(hr);
          }
      }
            
   
    if(sendBeacon==2)
      {
        dotCount=dotCount+1;
        if(dotCount==1)
          {
            setKey(1);
          }
        if(dotCount==12)
          {
            setKey(0);
          }
        if(dotCount==25)
          {
          dotCount=0;
          }
      } 
      


    if(sendBeacon==1)                                   //sending CWID
    {
      if(keyDownTimer>0)
        {    
          if((keyDownTimer>100) &&( keyDownTimer < CWIDkeyDownTime-100))                                //Key down between Idents
           {
            setKey(1);
           }
           else
           {
            setKey(0);
           }
        keyDownTimer--;
        }    
     else
      {
       int ret=morseKey();                              //get the next key from morse string
       if(ret==-1)                                      // Ident finished
        {
        keyDownTimer=CWIDkeyDownTime + 100;             //key down for this time between idents. Add 1 second to force a minimum key up gap between idents. 
        }
       else
        {  
          setKey(ret);
        }
      }
  
    }




    waterfall();

    if(configCounter>0)                                       //save the config after 5 seconds of inactivity.
    {
      configCounter=configCounter-1;                                                                                                  
      if(configCounter==0)
        {
        writeConfig();
        }
    }
    
   
   if(firstpass==1)
   {
   // Initialise Pluto RF chain without triggering the TX path (brief TX pulses damage power amplifiers)
   // RX LO already cycled in initPluto() before GNU Radio started — no IIO calls needed here.
   PlutoTxEnable(0);                  //ensure TX LO is off
   setHwTxFreq(freq+10.0);           //offset TX freq to suppress spurious
   setHwRxFreq(freq);                 //set initial RX frequency
   sendFifo("R");                     //ensure GNU Radio is in RX mode
   setMute(0);                        //unmute audio
   setBandBits(bandBitsRx[band]);     //set band outputs for RX
   plutoGpo=plutoGpo & 0xEF;
   setPlutoGpo(plutoGpo);             //clear TX GPO pin
   firstpass=0;
   }
   
   
   if(FFTTimeout >0 )                                   //check to see if the FFT transfers are still happening. 
    {
     FFTTimeout--;
    }
    else
    {
      restartGNURadio();                                         //attempt to restart GNU Radio.
    }
   
    
    while(runTimeMs() < (lastClock + 10))                //delay until the next iteration at 100 per second (10ms)
    {
    usleep(100);
    }
    lastClock=runTimeMs();
  }
}

long long runTimeMs(void)
{
struct timeval tt;
gettimeofday(&tt,NULL);
if(progStartTime==0)
  {
    progStartTime=tt.tv_sec;
  }
tt.tv_sec=tt.tv_sec - progStartTime;
return ((tt.tv_sec*1000) + (tt.tv_usec/1000));
}

void gen_palette(char colours[][3], int num_grads){
  //allocate some memory, size of palette
  palette = malloc(sizeof(char)*256*3);

  int diff[3];
  float scale=256/(num_grads);
  int pos=0;

  for(int i=0;i<num_grads;i++){
      //get differences in colours for current gradient
      diff[0]=(colours[i+1][0]-colours[i][0])/(scale-1);
      diff[1]=(colours[i+1][1]-colours[i][1])/(scale-1);
      diff[2]=(colours[i+1][2]-colours[i][2])/(scale-1);

      //create the palette built up of multiple gradients
      for(int n=0;n<scale;n++){
          palette[pos*3+2]=colours[i][0]+(n*diff[0]);
          palette[pos*3+1]=colours[i][1]+(n*diff[1]);
          palette[pos*3]=colours[i][2]+(n*diff[2]);
          pos++;
      }

  }
}

void flushUDP(void)
{
int ret;
  do
  {
  ret = fread(&inbuf,sizeof(float),1,txfftstream);
  }
  while (ret>0);
  
  do
  {
  ret = fread(&inbuf,sizeof(float),1,fftstream);
  }
  while (ret>0); 
}


// QO-100 NB transponder downlink band plan (AMSAT-DL)
static const struct {
    double        start;        // downlink start (MHz)
    double        end;          // downlink end (MHz)
    unsigned char r, g, b;     // display colour
    const char   *label;
} qo100bp[] = {
    { 10489.500, 10489.505, 255,   0,   0, "BCN" },  // Lower beacon
    { 10489.505, 10489.540,   0, 200,   0, "CW"  },  // CW only
    { 10489.540, 10489.580, 255, 140,   0, "DIG" },  // Digimodes 500 Hz max
    { 10489.580, 10489.650, 255, 210,   0, "DIG" },  // Digimodes 2700 Hz max
    { 10489.650, 10489.745,   0, 100, 255, "SSB" },  // SSB only
    { 10489.745, 10489.755, 255,   0,   0, "BCN" },  // Middle beacon
    { 10489.755, 10489.850,   0, 100, 255, "SSB" },  // SSB only
    { 10489.850, 10489.858, 200,   0, 200, "BCT" },  // Broadcast 10489.855
    { 10489.858, 10489.865, 255,   0, 128, "EMG" },  // Emergency 10489.860
    { 10489.865, 10489.990, 255, 200,   0, "MIX" },  // Mixed modes & special
    { 10489.990, 10489.997, 255,   0,   0, "BCN" },  // Multimedia beacon
    { 10489.997, 10490.000, 255,   0,   0, "BCN" },  // Upper beacon
};

void drawQO100BandPlan(void)
{
    static double lastDrawnFreq = -1.0;

    int nseg = (int)(sizeof(qo100bp) / sizeof(qo100bp[0]));
    int barY = FFTY + 20 + rows - 14;  // last 14 rows of waterfall
    int barH = 14;

    // Redraw only when freq has changed (avoids flicker on every waterfall frame)
    if (freq == lastDrawnFreq) return;
    lastDrawnFreq = freq;

    // Clear bar so it disappears when leaving the QO-100 range
    for (int y = 0; y < barH; y++)
        for (int x = 0; x < points; x++)
            setPixel(x + FFTX, barY + y, 0, 0, 0);

    // Show band plan only when tuned in the QO-100 NB passband (10489.490–10490.010 MHz)
    if (freq < 10489.490 || freq > 10490.010) return;

    // thin grey separator at top of bar
    for (int x = 0; x < points; x++)
        setPixel(x + FFTX, barY, 150, 150, 150);

    for (int i = 0; i < nseg; i++) {
        int x0 = (int)((qo100bp[i].start - freq) * 1e6 / HzPerBin) + points / 2;
        int x1 = (int)((qo100bp[i].end   - freq) * 1e6 / HzPerBin) + points / 2;

        if (x0 < 0)      x0 = 0;
        if (x1 > points) x1 = points;
        if (x0 >= x1)    continue;

        for (int y = 1; y < barH; y++)
            for (int x = x0; x < x1; x++)
                setPixel(x + FFTX, barY + y,
                         qo100bp[i].r, qo100bp[i].g, qo100bp[i].b);

        if (x1 - x0 > 18) {
            // inverse text: black on light backgrounds, white on dark
            int lum = qo100bp[i].r * 299 + qo100bp[i].g * 587 + qo100bp[i].b * 114;
            if (lum > 128000) setForeColour(0, 0, 0);
            else              setForeColour(255, 255, 255);
            // match back colour to segment so character "off" pixels don't appear black
            setBackColour(qo100bp[i].r, qo100bp[i].g, qo100bp[i].b);
            textSize = 1;
            gotoXY(FFTX + x0 + (x1 - x0) / 2 - 8, barY + 3);
            displayStr((char *)qo100bp[i].label);
            setBackColour(0, 0, 0);
        }
    }
}

void waterfall()
{
  int level,level2;
  int ret;
  int baselevel;
  int bwbaroffset;
  float scaling;
  int fftref;
  int centreShift=0;


     //check if data avilable to read
      if((transmitting==1) && (satMode()==0))
        {
        ret = fread(&inbuf,sizeof(float),1,txfftstream);
        fftref=10;
        }
      else
        {
          ret = fread(&inbuf,sizeof(float),1,fftstream);
          fftref=FFTRef;
        }
      
      if(ret>0)
    {
       FFTTimeout = FFTTIMEOUT;
    
        //shift buffer
        for(int r=rows-1;r>0;r--)
        {  
          for(int p=0;p<points;p++)
          {  
            buf[p][r]=buf[p][r-1];
          }
        }
    
        buf[0+points/2][0]=inbuf[0];    //use the read value
    
        //Read in float values, shift centre and store in buffer 1st 'row'
        for(int p=1;p<points;p++)
        {                                               
          if((transmitting==1) && (satMode()==0))
          {
          fread(&inbuf,sizeof(float),1,txfftstream);
          }
        else
          {
          fread(&inbuf,sizeof(float),1,fftstream);
          }
          
          if(p<points/2)
          {
            buf[p+points/2][0]=inbuf[0];
          }else
          {
            buf[p-points/2][0]=inbuf[0];
          }
        }
 
 
        if(((mode==CW)||(mode==CWN)) && (transmitting==1) && (satMode()==0))
          {
          bwbaroffset=800/HzPerBin;
          }
        else
          {
           bwbaroffset=0;
          }
        
 
        //use raw values to calculate S meter value
        //use highest reading within the receiver bandwidth
        
         sMeterPeak=-200;
         for (int p=points/2+bwBarStart-bwbaroffset;p<points/2+bwBarEnd-bwbaroffset;p++)
          {
           if(buf[p][0]>sMeterPeak)
             {
             sMeterPeak=buf[p][0];
             }
           
           }       
 
    
        //RF level adjustment
    
 
        baselevel=fftref-FFTRange;
        scaling = 255.0/(float)(fftref-baselevel);
        
        
    
        //draw waterfall (reserve last 14 rows for QO-100 band plan if needed)
        int bp_rows = (freq >= 10489.490 && freq <= 10490.010) ? 14 : 0;
        for(int r=0;r<rows - bp_rows;r++)
        {
          for(int p=0;p<points;p++)
          {
            //limit values displayed to range specified
            if (buf[p][r]<baselevel) buf[p][r]=baselevel;
            if (buf[p][r]>fftref) buf[p][r]=fftref;

            //scale to 0-255
            level = (buf[p][r]-baselevel)*scaling;
            setPixel(p+FFTX,FFTY+20+r,palette[level*3+2],palette[level*3+1],palette[level*3]);
          }
        }

        drawQO100BandPlan();

        //clear spectrum area
        for(int r=0;r<spectrum_rows+1;r++)
        { 
          for(int p=0;p<points;p++)
          {   
            setPixel(p+FFTX,FFTY-r,0,0,0);
          }
        }
    
        //draw spectrum line
        
        scaling = spectrum_rows/(float)(fftref-baselevel);
        for(int p=0;p<points-1;p++)
        {  
            //limit values displayed to range specified
            if (buf[p][0]<baselevel) buf[p][0]=baselevel;
            if (buf[p][0]>fftref) buf[p][0]=fftref;
    
            //scale to display height
            level = (buf[p][0]-baselevel)*scaling;   
            level2 = (buf[p+1][0]-baselevel)*scaling;
            drawLine(p+FFTX, FFTY-level, p+1+FFTX, FFTY-level2,255,255,255);
        }
          
          //draw Bandwidth indicator
          int p=points/2;
          
          bwBarStart=rxFilterLow/HzPerBin;
          bwBarEnd=rxFilterHigh/HzPerBin;
          
          if(bwBarStart < -255) bwBarStart = -255;
          if(bwBarEnd  > 255) bwBarEnd = 255;
          
        
          
          if (((mode==CW) || (mode==CWN)) && (transmitting==0 && satMode()== 0))
          {
           centreShift=800/HzPerBin;
          }
          else
          {
           centreShift=0;          
          }

          if(bwBarStart > -255) drawLine(p+FFTX+bwBarStart-bwbaroffset, FFTY-spectrum_rows+5, p+FFTX+bwBarStart-bwbaroffset, FFTY-spectrum_rows,255,140,0);
          drawLine(p+FFTX+bwBarStart-bwbaroffset, FFTY-spectrum_rows, p+FFTX+bwBarEnd-bwbaroffset, FFTY-spectrum_rows,255,140,0);
          
          if(bwBarEnd < 255) drawLine(p+FFTX+bwBarEnd-bwbaroffset, FFTY-spectrum_rows+5, p+FFTX+bwBarEnd-bwbaroffset, FFTY-spectrum_rows,255,140,0);  
          //draw centre line (displayed frequency)
          drawLine(p+FFTX+centreShift, FFTY-10, p+FFTX+centreShift, FFTY-spectrum_rows,255,0,0);  
          
          
          //draw x Axis. 
          int ticks[11] = {0,21,43,64,85,107,128,149,171,192,213};    // Rounded tick spacing at 21.333 pixels per tick 
          drawLine(FFTX,FFTY +3,FFTX + points,FFTY+3,0,255,0);
          for(int tick = 0;tick < 11;tick++)
            {
             drawLine(FFTX + p + ticks[tick],FFTY+3,FFTX + p + ticks[tick],FFTY +5,0,255,0);
             drawLine(FFTX + p - ticks[tick],FFTY+3,FFTX + p - ticks[tick],FFTY +5,0,255,0); 
            }
          
          //draw scale
          setForeColour(0,255,0);
          textSize=1;
          gotoXY(p+FFTX-12,FFTY+8);
          displayStr(" 0 ");
          switch(bandFFTBW[band])
          {
              case 0:                 //48KHz BW
              gotoXY(p+FFTX-ticks[5]-24,FFTY+8);
              displayStr(" -10k   ");
              gotoXY(p+FFTX-ticks[10]-24,FFTY+8);
              displayStr(" -20k   ");
              gotoXY(p+FFTX+ticks[5]-24,FFTY+8);
              displayStr(" +10k   ");                                                                                             
              gotoXY(p+FFTX+ticks[10]-24,FFTY+8);
              displayStr(" +20k   ");
            break;
            case 1:                 //24KHz BW
              gotoXY(p+FFTX-ticks[5]-20,FFTY+8);
              displayStr(" -5k   ");
              gotoXY(p+FFTX-ticks[10]-24,FFTY+8);
              displayStr(" -10k   ");
              gotoXY(p+FFTX+ticks[5]-20,FFTY+8);
              displayStr(" +5k   ");                                                                                             
              gotoXY(p+FFTX+ticks[10]-24,FFTY+8);
              displayStr(" +10k   ");
            break;
            case 2:                 //12KHz BW
              gotoXY(p+FFTX-ticks[5]-28,FFTY+8);
              displayStr(" -2.5k   ");
              gotoXY(p+FFTX-ticks[10]-20,FFTY+8);
              displayStr(" -5k   ");
              gotoXY(p+FFTX+ticks[5]-28,FFTY+8);
              displayStr(" +2.5k   ");                                                                                             
              gotoXY(p+FFTX+ticks[10]-20,FFTY+8);
              displayStr(" +5k   ");
            break;
            case 3:                 //6KHz BW
              gotoXY(p+FFTX-ticks[5]-32,FFTY+8);
              displayStr(" -1.25k   ");
              gotoXY(p+FFTX-ticks[10]-28,FFTY+8);
              displayStr(" -2.5k   ");
              gotoXY(p+FFTX+ticks[5]-32,FFTY+8);
              displayStr(" +1.25k   ");                                                                                             
              gotoXY(p+FFTX+ticks[10]-28,FFTY+8);
              displayStr(" +2.5k   ");
            break;
          }

 

          if((transmitting==0) || (satMode()==1))
          {
          S_Meter();
          }
          
          else
          {
          P_Meter();
          }
   }       
}


void S_Meter(void)
{

          char smStr[10];
          static int sMeterCount=0;
         
          sMeterPeak=sMeterPeak-bandSmeterZero[band];                   //adjust offset to give positive values for s-meter
          int dbOver=0;
          int sValue=0;
          
          if(bandRxGain[band]==100)                  //if we are in RF AGC mode
            {
             sMeterPeak=sMeterPeak + maxGain(freq) - readPlutoRxGain();       //compensate for reduced gain due to AGC action
            }       
            
          if(sMeterPeak < 0) sMeterPeak=0;
          if(sMeterPeak >= sMeter)
            {
            sMeter=sMeterPeak;                                    //fast attack
            } 
          else
            {
            if(sMeter > 0) sMeter=sMeter-2;                    //slow decay
            }
            
 
          if(sMeter<55)
            {
            sValue=sMeter/6;
            }
          else
            {
            sValue=9;
            dbOver=sMeter-54;
            }
 
          
          
          //Draw S meter
          drawLine(sMeterX,sMeterY,sMeterX+sMeterWidth,sMeterY,255,255,255);
          drawLine(sMeterX,sMeterY,sMeterX,sMeterY+sMeterHeight,255,255,255);
          drawLine(sMeterX,sMeterY+sMeterHeight,sMeterX+sMeterWidth,sMeterY+sMeterHeight,255,255,255);
          drawLine(sMeterX+sMeterWidth,sMeterY,sMeterX+sMeterWidth,sMeterY+sMeterHeight,255,255,255);          

          for(int ln=0;ln<10;ln++)
          {
          if(sMeter<55)
          {
          drawLine(sMeterX+5,sMeterY+5+ln,sMeterX+6+sMeter*2,sMeterY+5+ln,0,255,0);
          drawLine(sMeterX+6+sMeter*2,sMeterY+5+ln,sMeterX+6+160,sMeterY+5+ln,0,0,0);
          }
          else
          {
          int redbit=sMeterX+6+110+(sMeter-55)*2;
          if(redbit > (sMeterX+6+160)) redbit= sMeterX+6+160;
          drawLine(sMeterX+5,sMeterY+5+ln,sMeterX+6+110,sMeterY+5+ln,0,255,0);
          drawLine(sMeterX+6+110,sMeterY+5+ln,redbit,sMeterY+5+ln,255,0,0);
          drawLine(redbit,sMeterY+5+ln,sMeterX+6+160,sMeterY+5+ln,0,0,0);
          }

          }
    
          //draw the squelch setting bar
          for(int ln=0;ln<2;ln++)
          {
            if(squelch<55)
            {
            drawLine(sMeterX+5,sMeterY-4+ln,sMeterX+6+squelch*2,sMeterY-4+ln,0,255,0);
            drawLine(sMeterX+6+squelch*2,sMeterY-4+ln,sMeterX+6+160,sMeterY-4+ln,0,0,0);
            }
            else
            {
            int redbit=sMeterX+6+110+(squelch-55)*2;
            if(redbit > (sMeterX+6+160)) redbit= sMeterX+6+160;
            drawLine(sMeterX+5,sMeterY-4+ln,sMeterX+6+110,sMeterY-4+ln,0,255,0);
            drawLine(sMeterX+6+110,sMeterY-4+ln,redbit,sMeterY-4+ln,255,0,0);
            drawLine(redbit,sMeterY-4+ln,sMeterX+6+160,sMeterY-4+ln,0,0,0);
            }
          }       
 
          sMeterCount++;
          if(sMeterCount>5)
          {
              sMeterCount=0;
              textSize=2;
              setForeColour(0,255,0);
              gotoXY(sMeterX+10,sMeterY+20);
              if (sMeterType == 0)
              {
              sprintf(smStr,"S%d",sValue);
              displayStr(smStr);
              if(dbOver>0)
                {
                sprintf(smStr,"+%ddB  ",dbOver);
                displayStr(smStr);
                }
              else
                {
                displayStr("       ");
                }
              }
              else
              {
                sprintf(smStr,"%.0f dB    ",sMeter);
                displayStr(smStr);
              }
           }
 

            if((sMeter < squelch) && (squelch > 0))
            {
            squelchGate = 0;
            if(squelchGate != lastSquelchGate)
              {
              setMute(1);
              lastSquelchGate=squelchGate;
              }
            }
            else
            {
            squelchGate = 1;
            if(squelchGate != lastSquelchGate)
              {
              setMute(0);
              lastSquelchGate=squelchGate;
              }
            
            }



}

 void P_Meter(void)
{

          char smStr[10];
          static int sMeterCount=0;
         
          sMeterPeak=(sMeterPeak+50)*2.1;  
                  
          if(sMeterPeak < 0) sMeterPeak=0;
          if(sMeterPeak >100) sMeterPeak=100;     // Now Scaled between 0 and 100
          
        
        
          
          
          
          if(sMeterPeak >= sMeter)
            {
            sMeter=sMeterPeak;                                    //fast attack
            } 
          else
            {
            if(sMeter > 0) 
             {
             if((mode==CW) || (mode==CWN))
                {
                sMeter=sMeter-10;                               //fast decay for CW
                }
              else if((mode==USB) || (mode==LSB))
                {
                sMeter=sMeter-3;                                 //Slow Decay for SSB. 
                }
              else
                {
                sMeter=sMeter-0.5;                               //very slow decay
                }
              }
             
             }
            
             if(sMeter<0) sMeter=0;
          
          //Draw PO meter
          drawLine(sMeterX,sMeterY,sMeterX+sMeterWidth,sMeterY,255,255,255);
          drawLine(sMeterX,sMeterY,sMeterX,sMeterY+sMeterHeight,255,255,255);
          drawLine(sMeterX,sMeterY+sMeterHeight,sMeterX+sMeterWidth,sMeterY+sMeterHeight,255,255,255);
          drawLine(sMeterX+sMeterWidth,sMeterY,sMeterX+sMeterWidth,sMeterY+sMeterHeight,255,255,255);          

          for(int ln=0;ln<10;ln++)
          {
          drawLine(sMeterX+5,sMeterY+5+ln,sMeterX+6+sMeter*1.5,sMeterY+5+ln,0,255,0);
          drawLine(sMeterX+6+sMeter*1.5,sMeterY+5+ln,sMeterX+6+160,sMeterY+5+ln,0,0,0);
          }
 
          sMeterCount++;
          if(sMeterCount>5)
          {
              sMeterCount=0;
              textSize=2;
              setForeColour(0,255,0);
              gotoXY(sMeterX+10,sMeterY+20);
              displayStr("Tx Level");
           }
 
}




void detectHw()
{
  FILE * fp;
  char * ln=NULL;
  size_t len=0;
  ssize_t rd;
  int p;
  char handler[3][20];
  char * found;
  p=0;
  mousePresent=0;
  touchPresent=0;
  hmiPresent=0;
  portsdownPresent=0;
  fp=fopen("/proc/bus/input/devices","r");
   while ((rd=getline(&ln,&len,fp)!=-1))
    {
      if(ln[0]=='N')        //name of device
      {
        p=0;
        if((strstr(ln,"FT5406")!=NULL) || (strstr(ln,"pi-ts")!=NULL) || (strstr(ln,"ft5x06")!=NULL) || (strstr(ln,"Goodix")!=NULL))         //Found Raspberry Pi TouchScreen entry
          {
           p=1;                                //we have found the touchscreen
          }  
          
        if(strstr(ln,"C-Media")!=NULL)         //Found HMI device on the CM108 sound card
          {
           p=2;                                //hmi device on the sound card
          }                                                                 
      }
      
      if(ln[0]=='H')        //handlers
      {
         if(strstr(ln,"mouse")!=NULL)
         {
           found=strstr(ln,"event");        
           strcpy(handler[p],found); 
           handler[p][strlen(found)-2] = 0;         
           if(p==0)                                 //not the touch screen so assume it is a normal mouse
            {
              sprintf(mousePath,"/dev/input/%s",handler[0]);
              mousePresent=1;
              printf("Found Mouse at %s\n",mousePath); 
            }
           if(p==1)                                 //touch screen 
           {
             sprintf(touchPath,"/dev/input/%s",handler[1]);
             touchPresent=1;
             printf("Found Touch at %s\n",touchPath);
           }
         }
         if((strstr(ln,"kbd")!=NULL) && (p==2))             //found the HMI Entry for the CM108 Sound card
          {
           found=strstr(ln,"event");        
           strcpy(handler[p],found); 
           handler[p][strlen(found)-2] = 0;         
           sprintf(hmiPath,"/dev/input/%s",handler[2]);
           hmiPresent=1;
           printf("Found HMI Device at %s\n",hmiPath); 
          }       
      }   
    }
  fclose(fp);
  if(ln)  free(ln);
  
  if ((fp = openFile("rpidatv/bin/rpidatvgui", "r")))                      //test to see if Portsdown file is present. If so we change the exit behaviour. 
  {
    fclose(fp);
    portsdownPresent=1;
  }
    
    plutoPresent=1;      //this will be reset by setPlutoFreq if Pluto is not present.  
  
}

void displayError(char*st)
{
  gotoXY(errorX,errorY);
  setForeColour(255,0,0);
  textSize=2;
  displayStr(st);
}

void initPluto(void)
{
    plutoctx = iio_create_context(NULL,plutoip);
      if(iio_err(plutoctx)!=0)
      {
        plutoPresent=0;
        displayError("Pluto not responding");
        return;
      }
      else
      {
      plutophy = iio_context_find_device(plutoctx, "ad9361-phy");
      PlutoRxEnable(0);      //cycle RX LO before GNU Radio starts (no IIO stream yet = executes immediately)
      PlutoRxEnable(1);      //re-enable RX LO — both calls done before GNU Radio to avoid iiod queue delay
      }
}


/* write Pluto Channel attribute: long long int */
int wr_ch_lli(struct iio_channel *chn, const char* what, long long val)
{
	const struct iio_attr *attr = iio_channel_find_attr(chn, what);
  
    if((iio_attr_write_longlong(attr, val)) !=0)
      {
        return -1;
      }        
    else
      {
        return 0;
      }  
}


/* write Pluto Channel attribute: String */
int wr_ch_str(struct iio_channel *chn, const char* what, const char* str)
{
	const struct iio_attr *attr = iio_channel_find_attr(chn, what);
  
    if((iio_attr_write_string(attr, str)) !=0)
      {
        return -1;
      }        
    else
      {
        return 0;
      }  
}


/* write Pluto Debug attribute: String */
int wr_db_str(struct iio_device *dev, const char* what, const char* str)
{
	const struct iio_attr *attr = iio_device_find_debug_attr(dev, what);
  
    if((iio_attr_write_string(attr, str)) !=0)
      {
        return -1;
      }        
    else
      {
        return 0;
      }  
}


/* write Pluto Channel attribute: Double */
int wr_ch_dbl(struct iio_channel *chn, const char* what, double val)
{
	const struct iio_attr *attr = iio_channel_find_attr(chn, what);
  
    if((iio_attr_write_double(attr, val)) !=0)
      {
        return -1;
      }        
    else
      {
        return 0;
      }  
}




/* write Pluto Channel attribute: bool */
int wr_ch_bool(struct iio_channel *chn, const char* what, bool val)
{
	const struct iio_attr *attr = iio_channel_find_attr(chn, what);
  
    if((iio_attr_write_bool(attr, val)) !=0)
      {
        return -1;
      }        
    else
      {
        return 0;
      }  
}

/* Read Pluto Channel attribute: Double */
int rd_ch_dbl(struct iio_channel *chn, const char* what, double* val)
{
	const struct iio_attr *attr = iio_channel_find_attr(chn, what);
  
    if((iio_attr_read_double(attr, val)) !=0)
      {
        return -1;
      }        
    else
      {
        return 0;
      }  
}

void setPlutoRxFreq(long long rxfreq)
{
int ret;
   if(plutoPresent)
    { 
      ret=wr_ch_lli(iio_device_find_channel(plutophy, "altvoltage0", true),"frequency", rxfreq); //Rx LO Freq
      if(ret<0)
      {
      displayError("Pluto not responding");
      }
    }
  
}

void setPlutoTxFreq(long long txfreq)
{
int ret;
   if(plutoPresent)
    { 
      ret=wr_ch_lli(iio_device_find_channel(plutophy, "altvoltage1", true),"frequency", txfreq); //Tx LO Freq
   if(ret<0)
      {
      displayError("Pluto not responding");
      }
    }
  
}

void setPlutoTxAtt(int att)
{ 
  if(plutoPresent)
    {
      wr_ch_dbl(iio_device_find_channel(plutophy, "voltage0", true),"hardwaregain", (double)att); //set Tx Attenuator     
    }
}

void setPlutoRxGain(int gain)
{ 
  if(plutoPresent)
    {
     if(gain>maxGain(freq))
        {
          wr_ch_str(iio_device_find_channel(plutophy, "voltage0", false),"gain_control_mode", "slow_attack");  //set Auto Gain
        }
        else
        {
        wr_ch_str(iio_device_find_channel(plutophy, "voltage0", false),"gain_control_mode", "manual");  //set Manual  Gain control
        wr_ch_dbl(iio_device_find_channel(plutophy, "voltage0", false),"hardwaregain", (double)gain); //set Rx Gain 
        }
    } 
}


int readPlutoRxGain(void)
{
double ret;
      if(plutoPresent)
      {
        rd_ch_dbl(iio_device_find_channel(plutophy, "voltage0", false),"hardwaregain", &ret); //Read current Rx Gain
        return (int) ret; 
      }
      else
      {
      return 73;
      }

}



void PlutoTxEnable(int txon)
{
  if(plutoPresent)
    { 
      if(txon==0)
        {
        wr_ch_bool(iio_device_find_channel(plutophy, "altvoltage1", true),"powerdown", true); //turn off TX LO
        }
      else
        {
        wr_ch_bool(iio_device_find_channel(plutophy, "altvoltage1", true),"powerdown", false); //turn on TX LO
        }
    }

}

void PlutoRxEnable(int rxon)
{
  if(plutoPresent)
    {
      if(rxon==0)
        {
        wr_ch_bool(iio_device_find_channel(plutophy, "altvoltage0", true),"powerdown", true); //turn off RX LO
        }
      else
        {
        wr_ch_bool(iio_device_find_channel(plutophy, "altvoltage0", true),"powerdown", false); //turn on RX LO
        }
    }

}

void setPlutoGpo(int p)
{
  char pins[10]; 
   
  sprintf(pins,"0x27 0x%x0",p);
  pins[9]=0;

  if(plutoPresent)
    {
      wr_db_str(plutophy,"direct_reg_access",pins);
    }
}


void initUDP(void)
{
   struct sockaddr_in myaddr;
   int fdr; 
   int fdt; 
   
//initialise Receive UDP receiver for FFT Receiver stream
   fdr=socket(AF_INET,SOCK_DGRAM,0);
   memset((char *)&myaddr,0,sizeof(myaddr));                      //Set any valid address for receiving UDP packets
   myaddr.sin_family = AF_INET;                                  //Network Connection
   myaddr.sin_addr.s_addr = htonl(INADDR_ANY);                   //Any Address
   myaddr.sin_port = htons(RXPORT);                              //set UDP POrt to listen on
   bind(fdr,(struct sockaddr *)&myaddr,sizeof(myaddr));          //bind the socket to the address  
   fftstream=fdopen(fdr,"r");                                    //open as a stream
   fcntl(fileno(fftstream), F_SETFL, O_RDONLY | O_NONBLOCK);    //set it as nonblocking
   
//initialise Receive UDP receiver for FFT Transmitter stream
   fdr=socket(AF_INET,SOCK_DGRAM,0);
   memset((char *)&myaddr,0,sizeof(myaddr));                      //Set any valid address for receiving UDP packets
   myaddr.sin_family = AF_INET;                                   //Network Connection
   myaddr.sin_addr.s_addr = htonl(INADDR_ANY);                   //Any Address
   myaddr.sin_port = htons(TXPORT);                               //set UDP POrt to listen on
   bind(fdr,(struct sockaddr *)&myaddr,sizeof(myaddr));          //bind the socket to the address  
   txfftstream=fdopen(fdr,"r");                                  //open as a stream
   fcntl(fileno(txfftstream), F_SETFL, O_RDONLY | O_NONBLOCK);   //set it as nonblocking
      
}



void initFifos()
{
 if(access("/tmp/langstoneTRx",F_OK)==-1)   //does fifo exist already?
    {
        mkfifo("/tmp/langstoneTRx", 0666);
    }
}


void sendFifo(char * s)
{
  char fs[50];
  int ret;
  int retry;
  int success;
  strcpy(fs,s);
  strcat(fs,"\n");
  
  success = 0;

  fifofd=open("/tmp/langstoneTRx",O_WRONLY|O_NONBLOCK);
  retry=0;
    do
     {
       ret=write(fifofd,fs,strlen(fs));
       usleep(5000);
       retry++;
     }
   while((ret==-1)&(retry<20));    
  close(fifofd);
}

void initGPIO(void)
{
  gpiohandle = lgGpiochipOpen(4);
  lgGpioClaimInput(gpiohandle,LG_SET_PULL_UP,pttPin); 
  lgGpioClaimInput(gpiohandle,LG_SET_PULL_UP,keyPin); 
  lgGpioClaimOutput(gpiohandle,0,txPin,0); 
  lgGpioClaimOutput(gpiohandle,0,bandPin1,0); 
  lgGpioClaimOutput(gpiohandle,0,bandPin1alt,0); 
  lgGpioClaimOutput(gpiohandle,0,bandPin2,0); 
  lgGpioClaimOutput(gpiohandle,0,bandPin3,0); 
  lgGpioClaimOutput(gpiohandle,0,bandPin4,0); 
  lgGpioClaimOutput(gpiohandle,0,bandPin5,0); 
  lgGpioClaimOutput(gpiohandle,0,bandPin6,0); 
  lgGpioClaimOutput(gpiohandle,0,bandPin7,0); 
  lgGpioClaimOutput(gpiohandle,0,bandPin8,0); 
  lastKey=1;
}

void processGPIO(void)
{
int p1=1;
int k1=1;

  p1=lgGpioRead(gpiohandle,pttPin);
  k1=lgGpioRead(gpiohandle,keyPin);

  
    if(enableGPIOPTT)
        {
        if(p1==0)             //if the hardware PTT has been pressed
            {
              if(ptt==0)
                {
                  ptt=1;
                  if(ptts==1) setPtts(0);                       //turn off the touchscreen PTT.
                  setTx(ptt|ptts);
                }
            }
         else
            {
              if(ptt==1)
                {
                  ptt=0;
                  setTx(ptt|ptts);
                }
            }
        }

    if(enableCWKey)
        {
        if(k1!=lastKey)
            {
            setKey(!k1);
            lastKey=k1;
            }

        if((mode==CW) ||(mode==CWN))
          {
            if(k1==0)          //key down
              {
                if((ptt|ptts)==0)   //not transmitting
                  {
                    setTx(1);
                  }
                breakInTimer=breakInTime;
              }
            else
              {
                if((breakInTimer>0) & ((ptt|ptts)==0))
                {
                breakInTimer--;
                if(breakInTimer==0)
                  {
                  setTx(0);
                  }
                }
              }
           }
        }
     
}



void sqlButton(int show)
{
 char sqlStr[5]; 
  gotoXY(sqlButtonX,sqlButtonY);
  if(show==1)
    {
      setForeColour(0,255,0);
    }
  else
    {
      setForeColour(0,0,0);
    }
  displayButton("SQL");
  textSize=2;
  gotoXY(sqlButtonX+30,sqlButtonY-25);
  displayStr("   ");
  gotoXY(sqlButtonX+30,sqlButtonY-25);
  sprintf(sqlStr,"%d",squelch);
  displayStr(sqlStr);
}

void ritButton(int show)
{
 char ritStr[5]; 
 int to;
  if(show==1)
    {
      setForeColour(0,255,0);
    }
  else
    {
      setForeColour(0,0,0);
      gotoXY(ritButtonX,ritButtonY+buttonSpaceY);
      displayButton("Zero"); 
    }
  gotoXY(ritButtonX,ritButtonY);
  displayButton("RIT");
  textSize=2;
  to=0;
  if(abs(rit)>0) to=8;
  if(abs(rit)>9) to=16;
  if(abs(rit)>99) to=24;
  if(abs(rit)>999) to=32;
  gotoXY(ritButtonX,ritButtonY-25);
  displayStr("         ");
  gotoXY(ritButtonX+38-to,ritButtonY-25);
  if (rit==0)
  {
  sprintf(ritStr,"0");
  }
  else
  {
    sprintf(ritStr,"%+d",rit);
  }
  displayStr(ritStr);

}

void initGUI()
{
  clearScreen();


// Volume Button
  gotoXY(volButtonX,volButtonY);
  setForeColour(0,255,0);
  displayButton("Vol");
 

 //bottom row of buttons
  displayMenu();
  setBand(band);
  
//Squelch button now visible in all modes.
    sqlButton(1);

    displayArrowButtons();

    clearWaterfall();

}

void setRotation(int rot)
{
  rotateTouch(rot);
  rotateScreen(rot);
}

void clearWaterfall(void)
{
  for(int r=0;r<rows;r++)
   {  
   for(int p=0;p<points;p++)
    {  
    buf[p][r]=-100;
    }
   }
   flushUDP();
}


void initSDR(void)
{
  setBand(band);
  setMode(mode); 
  setVolume(volume);
  setSquelch(squelch);
  setRit(0);
  setSSBMic(SSBMic);
  setFMMic(FMMic);
  setAMMic(AMMic);
  setFreqInc();
  lastLOhz=0;
  setFreq(freq);
  setAlsaCapture(alsaCaptureLevel);
  setTx(0);
}

void displayMenu()
{
gotoXY(funcButtonsX,funcButtonsY);
  setForeColour(0,255,0);
  displayButton("BAND");
  displayButton("MODE");
  
  if((mode==FM) && (bandRepShift[band]!=0))
  {
  if((ptt | ptts) && (bandDuplex[band]>0))
    {
    displayButton("1750");
    }
  else
    {
    displayButton("DUP ");
    }
  }
  else
  {
  setForeColour(0,0,255);
  displayButton("EXTRA");
  setForeColour(0,255,0);
  }

  displayButton("SET");
  if(satMode()==1) 
    {
    displayButton("MONI");
    }
  else
  {
    displayButton("    ");
  }
  if(sendBeacon > 0)
    {
    setForeColour(255,0,0);
    if(sendBeacon==1)
      {
        displayButton("CWID");
      } 
     else
      {
        displayButton("DOTS");
      } 
    }
  else
    {
    setForeColour(0,255,0);
    displayButton("BEACON");
    }

  if(ptt|ptts)
    {
    setForeColour(255,0,0);  
    }
  else
    {
    setForeColour(0,255,0);
    }   
  displayButton("PTT");

}


void displayPopupMode(void)
{
clearPopUp();
gotoXY(popupX,popupY);
  setForeColour(0,255,0);
  for(int n=0;n<nummode;n++)
  {
    displayButton(modename[n]);
  }
popupSel=MODE;
}

void displayPopupBand(void)
{
char bstr[6];
int b;
clearPopUp();
gotoXY(popupX,popupY);
  setForeColour(0,255,0);
 displayButton("More..");   
  for(int n=0;n<6;n++)
  {
    b=bandFreq[n+popupFirstBand];
    sprintf(bstr,"%d", b);
    displayButton(bstr);
  }
popupSel=BAND;
}

void displayPopupBeacon(void)
{
clearPopUp();
gotoXY(popupX + buttonSpaceX*5,popupY);
setForeColour(0,255,0);
displayButton("DOTS");
displayButton("CWID");
popupSel=BEACON;
}


void clearPopUp(void)
{
for(int py=popupY;py<popupY+buttonHeight+1;py++)
{
  for(int px=0;px<800;px++)
  {
  setPixel(px,py,0,0,0);
  }
}
popupSel=NONE;
displayMenu();
}

void processHmi(int hm)
{
  if(hm==1)         //volume down button released
    {
      setPtts(0);
    }
   if(hm == 2)     //volume down button pressed
    {
       setPtts(1);
    }
}
                                                          
void processMouse(int mbut)
{
  if(mbut==128)       //scroll whell turned 
    {
      if((inputMode==FREQ) && (dialLock==0))
      {
        freq=freq+(mouseScroll*freqInc);
        mouseScroll=0;
        if(((freq + bandRxOffset[band])/bandRxHarmonic[band]) < minHwFreq) freq=(minHwFreq - bandRxOffset[band])/bandRxHarmonic[band];
        if(((freq + bandRxOffset[band])/bandRxHarmonic[band]) > maxHwFreq) freq=(maxHwFreq - bandRxOffset[band])/bandRxHarmonic[band];
        setFreq(freq);
        return;      
      }
      
      if(mouseScroll>0) mouseScroll=1;                     //prevent large changes when adjusting. 
      if(mouseScroll<0) mouseScroll=-1;
      
      if(inputMode==SETTINGS)
      {
        changeSetting();
        return;
      }
      if(inputMode==NB1_SETTINGS)
      {
        changeNB1Setting();
        return;
      }
      if(inputMode==COMP_SETTINGS)
      {
        changeCompSetting();
        return;
      }
      if(inputMode==VOLUME)
      {
        volume=volume+mouseScroll;
        mouseScroll=0;
        if(volume < 0) volume=0;
        if(volume > maxvol) volume=maxvol;
        setVolume(volume);
        return;      
      }    
      if(inputMode==SQUELCH)
      {
        squelch=squelch+mouseScroll;
        mouseScroll=0;
        if(squelch < 0) squelch=0;
        if(squelch > maxsql) squelch=maxsql;
        bandSquelch[band][mode]=squelch;
        setSquelch(squelch);
        return;      
      }   
      if(inputMode==RIT)
      {
        rit=rit+mouseScroll*10;
        mouseScroll=0;
        if(rit < minrit) rit=minrit;
        if(rit > maxrit) rit=maxrit;
        setRit(rit);
        return;      
      }     
    }
    
  if(mbut==1+128)      //Left Mouse Button down
    {
      if((inputMode==SETTINGS)&&((settingNo==CWID)||(settingNo==BAND_BITS_RX)||(settingNo==BAND_BITS_TX)))
       {
         setIndex=setIndex-1;
         if(setIndex<0) setIndex=0;
         displaySetting(settingNo);
       }
      else if(inputMode==NB1_SETTINGS)
       {
         nb1SettingNo--;
         if(nb1SettingNo < 0) nb1SettingNo=numNB1Settings-1;
         displayNB1Setting(nb1SettingNo);
       }
      else if(inputMode==COMP_SETTINGS)
       {
         compSettingNo--;
         if(compSettingNo < 0) compSettingNo=numCOMPSettings-1;
         displayCOMPSetting(compSettingNo);
       }
      else
       {
        tuneDigit=tuneDigit-1;
        if(tuneDigit<0) tuneDigit=0;
        if(tuneDigit==5) tuneDigit=4;
        if(tuneDigit==9) tuneDigit=8;
        setFreqInc();
        setFreq(freq);
        twoButTimer=20;
        lastBut=lastBut | 1;
        if((inputMode==SETTINGS) && (settingNo==BAND_BITS_RX))
          {
          displaySetting(BAND_BITS_RX);
          }
        if((inputMode==SETTINGS) && (settingNo==BAND_BITS_TX))
          {
          displaySetting(BAND_BITS_TX);
          }
       }  
    }
    
  if(mbut==2+128)      //Right Mouse Button down
    {
      if((inputMode==SETTINGS)&&((settingNo==CWID)||(settingNo==BAND_BITS_RX)||(settingNo==BAND_BITS_TX)))
       {
         setIndex=setIndex+1;
         if(setIndex>maxSetIndex) setIndex=maxSetIndex;
         displaySetting(settingNo);
       }
      else if(inputMode==NB1_SETTINGS)
       {
         nb1SettingNo++;
         if(nb1SettingNo==numNB1Settings) nb1SettingNo=0;
         displayNB1Setting(nb1SettingNo);
       }
      else if(inputMode==COMP_SETTINGS)
       {
         compSettingNo++;
         if(compSettingNo==numCOMPSettings) compSettingNo=0;
         displayCOMPSetting(compSettingNo);
       }
      else
       {
         tuneDigit=tuneDigit+1;
         if(tuneDigit > maxTuneDigit) tuneDigit=maxTuneDigit;
         if(tuneDigit==5) tuneDigit=6;
         if(tuneDigit==9) tuneDigit=10;
         setFreqInc();
         setFreq(freq); 
         twoButTimer=20;
         lastBut=lastBut | 2;
         if((inputMode==SETTINGS) && (settingNo==BAND_BITS_RX))
           {
             displaySetting(BAND_BITS_RX);
           }
         if((inputMode==SETTINGS) && (settingNo==BAND_BITS_TX))
           {
             displaySetting(BAND_BITS_TX);
           }
        }          
    }
 
  if((mbut==3+128) | (lastBut==3))       //Middle button down or both buttons within 100ms
     {
      if(dialLock==0)
        {
          setDialLock(1);
        }
      else
        {
         setDialLock(0); 
        }
       lastBut=0;
     }
    
  if(mbut==4+128)      //Extra Button down
    {
 
    }   
    
  if(mbut==5+128)      //Side Button down
    {
 
    }
  
    
}

void setFreqInc()
{
  if(tuneDigit==0) freqInc=10000.0;
  if(tuneDigit==1) freqInc=1000.0;
  if(tuneDigit==2) freqInc=100.0;
  if(tuneDigit==3) freqInc=10.0;
  if(tuneDigit==4) freqInc=1.0;
  if(tuneDigit==5) tuneDigit=6;
  if(tuneDigit==6) freqInc=0.1;
  if(tuneDigit==7) freqInc=0.01;
  if(tuneDigit==8) freqInc=0.001;                                    
  if(tuneDigit==9) tuneDigit=10;
  if(tuneDigit==10) freqInc=0.0001;
  if(tuneDigit==11) freqInc=0.00001;   
}
       


void processTouch()
{

// Arrow Buttons (d-pad) - equivalent to mouse wheel and left/right buttons

if(arrowButtonTouched(arrowUpX, arrowUpY))
  {
    mouseScroll=1;
    processMouse(128);
    return;
  }

if(arrowButtonTouched(arrowDownX, arrowDownY))
  {
    mouseScroll=-1;
    processMouse(128);
    return;
  }

if(arrowButtonTouched(arrowLeftX, arrowLeftY))
  {
    processMouse(1+128);
    twoButTimer=0;    // prevent accidental dial lock when tapping left then right
    lastBut=0;
    return;
  }

if(arrowButtonTouched(arrowRightX, arrowRightY))
  {
    processMouse(2+128);
    twoButTimer=0;    // prevent accidental dial lock when tapping right then left
    lastBut=0;
    return;
  }


// Volume Button   


if(buttonTouched(volButtonX,volButtonY))    //Vol
    {
      if(inputMode==VOLUME)
        {
        setInputMode(FREQ);
        }
      else
        {
        setInputMode(VOLUME);
        }
      return;
    }
                                                        


// Squelch Button   


if(buttonTouched(sqlButtonX,sqlButtonY))    //sql
    {
      if(inputMode==SQUELCH)
        {
        setInputMode(FREQ);
        }
      else
        {
        setInputMode(SQUELCH);
        }
      return;
    }


//RIT Button

if(buttonTouched(ritButtonX,ritButtonY))    //rit
    {
     if(mode!=4)
     {
      if(inputMode==RIT)
        {
        setInputMode(FREQ);
        }
      else
        {
        setInputMode(RIT);
        }
      return;
      }
    }

//RIT Zero Button

if(buttonTouched(ritButtonX,ritButtonY+buttonSpaceY))    //rit zero
    {
     setRit(0);
     setInputMode(FREQ);
    }
    
if(buttonTouched(sMeterX,sMeterY))                        //touch on s-Meter
{
   if(sMeterType == 0)
   {
     sMeterType=1;
   }
   else
   {
     sMeterType=0;
   }
}

//Function Buttons


if(buttonTouched(funcButtonsX,funcButtonsY))    //Button 1 = BAND or MENU
    {
    if((inputMode==FREQ) && (popupSel!=BAND))
    {
      writeConfig();
      displayPopupBand();
      return;
    }
    else if(inputMode==NB1_SETTINGS)
    {
      writeConfig();
      setInputMode(EXTRA);
      return;
    }
    else if(inputMode==COMP_SETTINGS)
    {
      writeConfig();
      setInputMode(EXTRA);
      return;
    }
    else
    {
      setInputMode(FREQ);
      clearPopUp();
      return;
    }
      
        
    }      
if(buttonTouched(funcButtonsX+buttonSpaceX,funcButtonsY))    //Button 2 = MODE or NB1
    {
     if((inputMode==FREQ) && (popupSel!=MODE))
      {
      displayPopupMode();
      return;
      }
     else if(inputMode==EXTRA)
      {
      nb1Active=!nb1Active;
      if(nb1Active) sendFifo("N1"); else sendFifo("N0");
      showExtraMenu();
      return;
      }
      else
      {
      setInputMode(FREQ);
      clearPopUp();
      return;
      }
    }
      
if(buttonTouched(funcButtonsX+buttonSpaceX*2,funcButtonsY))  // Button 3 = EXTRA or DUP or 1750 or NEXT
    {
     if(inputMode==FREQ)
      {
      if((mode==FM) && (bandRepShift[band]!=0))
        {
        if((ptt | ptts) && (bandDuplex[band]>0))
          {
          send1750();
          }
        else
          {
          if(bandDuplex[band]==0)
            {
            bandDuplex[band]=1;
            setMode(mode);
            }
          else
            {
            bandDuplex[band]=0;
            setMode(mode);
            }
          }
        }
      else
        {
        setInputMode(EXTRA);
        }
      return;
      }
      else if(inputMode==EXTRA)
      {
      compActive=!compActive;
      if(compActive) sendFifo("c1"); else sendFifo("c0");
      showExtraMenu();
      return;
      }
      else if(inputMode==SETTINGS)
      {
      settingNo=settingNo+1;
      if(settingNo==numSettings) settingNo=0;
      displaySetting(settingNo);
      return;
      }
      else if(inputMode==NB1_SETTINGS)
      {
      nb1SettingNo=nb1SettingNo+1;
      if(nb1SettingNo==numNB1Settings) nb1SettingNo=0;
      displayNB1Setting(nb1SettingNo);
      return;
      }
      else if(inputMode==COMP_SETTINGS)
      {
      compSettingNo=compSettingNo+1;
      if(compSettingNo==numCOMPSettings) compSettingNo=0;
      displayCOMPSetting(compSettingNo);
      return;
      }
      else
      {
      setInputMode(FREQ);
      }
    }
      
if(buttonTouched(funcButtonsX+buttonSpaceX*3,funcButtonsY))    // Button4 =SET or PREV
    {
     if(inputMode==FREQ)
      {
      setInputMode(SETTINGS);
      return;
      }
    else  if (inputMode==SETTINGS)
      {
      settingNo=settingNo-1;
      if(settingNo<0) settingNo=numSettings-1;
      displaySetting(settingNo);
      return;
      }
    else if(inputMode==NB1_SETTINGS)
      {
      nb1SettingNo=nb1SettingNo-1;
      if(nb1SettingNo<0) nb1SettingNo=numNB1Settings-1;
      displayNB1Setting(nb1SettingNo);
      return;
      }
    else if(inputMode==COMP_SETTINGS)
      {
      compSettingNo=compSettingNo-1;
      if(compSettingNo<0) compSettingNo=numCOMPSettings-1;
      displayCOMPSetting(compSettingNo);
      return;
      }
    else
      {
      setInputMode(FREQ);
      return;
      }
    }

if(buttonTouched(funcButtonsX+buttonSpaceX*4,funcButtonsY))    //Button 5 =MONI (only allowed in Sat mode)  or Restart
    {
    if(inputMode==FREQ)
      {
      if(satMode()==1)
        {
        if(moni==1) setMoni(0); else setMoni(1);
        }      
      return;
      } 
      else if (inputMode==SETTINGS)
      {
        restartGNURadio();
      }
      else if (inputMode==NB1_SETTINGS)
      {
        setInputMode(COMP_SETTINGS);
        return;
      }
      else if (inputMode==COMP_SETTINGS)
      {
        setInputMode(NB1_SETTINGS);
        return;
      }
    else
      {
      setInputMode(FREQ);
      }


    }

if(buttonTouched(funcButtonsX+buttonSpaceX*5,funcButtonsY))    //Button 6 = BEACON or APPLY or Exit to Portsdown
    {
    if(inputMode==FREQ)
      {
      if(sendBeacon==0)
        {
         displayPopupBeacon();
        }
      else
        {
        setBeacon(0);
        }

      return;
      }
      else if (inputMode==SETTINGS)
      {
         setBandBits(0);
         sendFifo("H0");        //unlock the flowgraph so that it can exit
         sendFifo("Q");       //kill the SDR
         clearScreen();
         writeConfig();
         iio_context_destroy(plutoctx);
         sleep(2);
         exit(0);
      }
      else if (inputMode==NB1_SETTINGS)
      {
        sendNB1Params();
        return;
      }
      else if (inputMode==COMP_SETTINGS)
      {
        sendCOMPParams();
        return;
      }
      else
      {
      setInputMode(FREQ);
      }
    } 
         
if(buttonTouched(funcButtonsX+buttonSpaceX*6,funcButtonsY))   //Button 7 = PTT or SET(EXTRA) or OFF
    {
    if(inputMode==EXTRA)
      {
      setInputMode(NB1_SETTINGS);
      return;
      }
    if(inputMode==FREQ)
      {
      if(ptts==0)
        {
          setPtts(1);
        }
      else
        {
          setPtts(0);
        }
      return;
      }
      else if (inputMode==SETTINGS)
      {
      setBandBits(0);
      sendFifo("H");        //unlock the flowgraph so that it can exit
      sendFifo("Q");       //kill the SDR 
      writeConfig();
      iio_context_destroy(plutoctx);
      system("$HOME/Langstone/Screen_Message ""Langstone Stopped""");
      sleep(2);
      system("sudo poweroff");                          
      return;      
      }
      else
      {
      setInputMode(FREQ);
      }
      

    }      


   
//Touch on Frequency Digits moves cursor to digit and sets tuning step. Removes dial lock if it is set.  

if((touchY>freqDisplayY) & (touchY < freqDisplayY+freqDisplayCharHeight) & (touchX>freqDisplayX) & (touchX < freqDisplayX+12*freqDisplayCharWidth))   
  {
    if(inputMode==FREQ) setDialLock(0);
    int tx=touchX-freqDisplayX;
    tx=tx/freqDisplayCharWidth;
    tuneDigit=tx;
    setFreqInc();
    setFreq(freq);
    return;
  }


//touch on spectrum display increments the FFT Bandwidth
if((touchY < FFTY) & (touchY > FFTY - spectrum_rows) & (touchX > FFTX) & (touchX < FFTX + points))   
  {
    bandFFTBW[band]++;
    if(bandFFTBW[band] > 3)  bandFFTBW[band] = 0;
    setFFTBW(bandFFTBW[band]);
    return;
  }



if(popupSel==MODE)
{
  for(int n=0;n<nummode;n++)
  {
     if(buttonTouched(popupX+(n*buttonSpaceX),popupY))                                
       {
       mode=n;
       setMode(mode);
       clearPopUp();
       }
  }
}

if(popupSel==BAND)
{
  if(buttonTouched(popupX,popupY))
  {
  popupFirstBand=popupFirstBand+6;
  if(bands24)
  {
    if(popupFirstBand>23) popupFirstBand=0;
  }
  else
  {
    if(popupFirstBand>11) popupFirstBand=0;
  }
  displayPopupBand();
  }
  
  for(int n=1;n<7;n++)
  {
     if(buttonTouched(popupX+(n*buttonSpaceX),popupY))                                
       {
       band=n-1+popupFirstBand;
       setBand(band);
       clearPopUp();
       }
  }

 
}


if(popupSel==BEACON)
  {
    if(buttonTouched(popupX+ 5* buttonSpaceX,popupY))       //DOTS
      {
      setBeacon(2);
      clearPopUp();
      }
     if(buttonTouched(popupX+ 6* buttonSpaceX,popupY))       //CWID
      {
      setBeacon(1);
      clearPopUp();
      }     
  }







}


int buttonTouched(int bx,int by)
{
  return ((touchX > bx) & (touchX < bx+buttonWidth) & (touchY > by) & (touchY < by+buttonHeight));
}

int arrowButtonTouched(int bx, int by)
{
  return ((touchX > bx) & (touchX < bx+arrowButtonSize) & (touchY > by) & (touchY < by+arrowButtonSize));
}

void displayArrowButton(char*s, int bx, int by)
{
  int x, y;
  for(x=0; x<=arrowButtonSize; x++)
  {
    setPixel(bx+x, by, foreColourR, foreColourG, foreColourB);
    setPixel(bx+x, by+arrowButtonSize, foreColourR, foreColourG, foreColourB);
  }
  for(y=0; y<=arrowButtonSize; y++)
  {
    setPixel(bx, by+y, foreColourR, foreColourG, foreColourB);
    setPixel(bx+arrowButtonSize, by+y, foreColourR, foreColourG, foreColourB);
  }
  for(x=1; x<arrowButtonSize; x++)
    for(y=1; y<arrowButtonSize; y++)
      setPixel(bx+x, by+y, backColourR, backColourG, backColourB);
  int sx = (arrowButtonSize - (int)strlen(s)*8*2) / 2;
  gotoXY(bx+sx, by+12);
  textSize=2;
  displayStr(s);
}

void displayArrowButtons(void)
{
  setForeColour(0, 255, 0);
  displayArrowButton("^", arrowUpX, arrowUpY);
  displayArrowButton("v", arrowDownX, arrowDownY);
  displayArrowButton("<", arrowLeftX, arrowLeftY);
  displayArrowButton(">", arrowRightX, arrowRightY);
}

void setBand(int b)
{
  freq=bandFreq[band];
  setFreq(freq);
  mode=bandMode[band];
  setMode(mode);
  setFFTBW(bandFFTBW[band]);
  setBandBits(bandBitsRx[band]);
  squelch=bandSquelch[band][mode];
  setSquelch(squelch);
  setCTCSS(bandCTCSS[band]);
  FFTRef=bandFFTRef[band];
  FFTRange=bandFFTRange[band];
  TxAtt=bandTxAtt[band];
  setPlutoTxAtt(TxAtt);
  setPlutoRxGain(bandRxGain[band]);
  configCounter=configDelay;
}

void setPtts(int p)
{
 if(p==1)
 {
  ptts=1;
  gotoXY(funcButtonsX+buttonSpaceX*6,funcButtonsY);
  setForeColour(255,0,0);
  displayButton("PTT"); 
  setTx(ptt|ptts);
 }
  else
 {
  ptts=0;
  if(sendBeacon>0)
   {
    setBeacon(0);
    setMode(mode);
    gotoXY(funcButtonsX+buttonSpaceX*5,funcButtonsY);
    setForeColour(0,255,0);
    displayButton("BEACON");  
    }

  setTx(ptt|ptts);
  gotoXY(funcButtonsX+buttonSpaceX*6,funcButtonsY);
  setForeColour(0,255,0);
  displayButton("PTT");
  gotoXY(funcButtonsX+buttonSpaceX*5,funcButtonsY);
  setForeColour(0,255,0);
  displayButton("BEACON");   
 }

}




void setBeacon(int b)
{
 if(b > 0)
   {
      sendBeacon=b;
      morseReset();
      keyDownTimer=300;
      lastmode=mode;
      mode=CW;
      setMode(mode);
      if(lastmode==USB) freq=freq+0.0008;         //If we are working SSb add an offset of 800Hz to bring pips into Rx bandwidth.
      if(lastmode==LSB) freq=freq-0.0008;
      setFreq(freq);                  
      if(!(ptt|ptts))                     //if not already transmitting
        {
          setTx(1);                          //goto transmit
        }
      ptts=1;                             //latch the transmit on
      gotoXY(funcButtonsX+buttonSpaceX*5,funcButtonsY);
      setForeColour(255,0,0);
      displayButton("BEACON");  
      gotoXY(funcButtonsX+buttonSpaceX*6,funcButtonsY);
      setForeColour(255,0,0);
      displayButton("PTT");       
   }
  else
    {
      sendBeacon=0;                                                                                           
      ptts=0;
      setTx(0);
      setKey(0);
      usleep(100000);
      mode=lastmode;
      setMode(mode);
      if(mode==USB) freq=freq-0.0008;
      if(mode==LSB) freq=freq+0.0008;
      setFreq(freq);
      gotoXY(funcButtonsX+buttonSpaceX*5,funcButtonsY);
      setForeColour(0,255,0);
      displayButton("BEACON");  
      gotoXY(funcButtonsX+buttonSpaceX*6,funcButtonsY);
      setForeColour(0,255,0);
      displayButton("PTT");       
    }

}

 
void setVolume(int vol)
{
  char volStr[10];
  sprintf(volStr,"V%d",vol);
  sendFifo(volStr);
  setForeColour(0,255,0);
  textSize=2;
  gotoXY(volButtonX+30,volButtonY-25);
  displayStr("   ");
  gotoXY(volButtonX+30,volButtonY-25);
  sprintf(volStr,"%d",vol);
  displayStr(volStr);
  
  configCounter=configDelay;
}

void setSquelch(int sql)
{
  char sqlStr[10];
  sprintf(sqlStr,"S%d",sql);
  setForeColour(0,255,0);
  textSize=2;
  gotoXY(sqlButtonX+30,sqlButtonY-25);
  displayStr("   ");
  gotoXY(sqlButtonX+30,sqlButtonY-25);
  sprintf(sqlStr,"%d",sql);
  displayStr(sqlStr);
  configCounter=configDelay;
}

void setInputMode(int m)

{
if(inputMode==SETTINGS)
  {
  gotoXY(0,settingY);
  setForeColour(255,255,255);
  textSize=2;
  displayStr("                                                ");
  gotoXY(0,settingY+8);
  displayStr("                                                ");
  writeConfig();
  displayMenu();
  }
if(inputMode==VOLUME)
  {
    gotoXY(volButtonX,volButtonY);
    setForeColour(0,255,0);
    displayButton("Vol");
  }
if(inputMode==SQUELCH)
  {
    gotoXY(sqlButtonX,sqlButtonY);
    setForeColour(0,255,0);
    displayButton("SQL"); 
  }
if(inputMode==RIT)
  {
    gotoXY(ritButtonX,ritButtonY);
    setForeColour(0,255,0);
    displayButton("RIT");
    gotoXY(ritButtonX,ritButtonY+buttonSpaceY);
    setForeColour(0,0,0);
    displayButton("Zero");
  }
  
inputMode=m;

if(inputMode==FREQ)
  {
  setFreq(freq);
  }

if(inputMode==SETTINGS)
  {
    clearPopUp();
    showSettingsMenu();
    mouseScroll=0;
    displaySetting(settingNo);
  }
if(inputMode==EXTRA)
  {
    clearPopUp();
    showExtraMenu();
  }
if(inputMode==NB1_SETTINGS)
  {
    clearPopUp();
    showNB1SettingsMenu();
    displayNB1Setting(nb1SettingNo);
  }
if(inputMode==COMP_SETTINGS)
  {
    clearPopUp();
    showCOMPSettingsMenu();
    displayCOMPSetting(compSettingNo);
  }
if(inputMode==VOLUME)
  {
    gotoXY(volButtonX,volButtonY);
    setForeColour(255,0,0);
    displayButton("Vol");
  }
if(inputMode==SQUELCH)
  {
    gotoXY(sqlButtonX,sqlButtonY);
    setForeColour(255,0,0);
    displayButton("SQL"); 
  }
if(inputMode==RIT)
  {
    gotoXY(ritButtonX,ritButtonY);
    setForeColour(255,0,0);
    displayButton("RIT");
    gotoXY(ritButtonX,ritButtonY+buttonSpaceY);
    setForeColour(255,0,0);
    displayButton("Zero");
  }

}

void setRit(int ri)
{
  char ritStr[10];
  int to;
  if(!((mode==FM) || (mode==AM)))
  {
  rit=ri;
  setForeColour(0,255,0);
  textSize=2;
  to=28; 
  gotoXY(ritButtonX,ritButtonY-25);
  displayStr("         ");
  gotoXY(ritButtonX+38-to,ritButtonY-25);
  if (rit==0)
  {
  sprintf(ritStr," 0.00");
  }
  else
  {
    sprintf(ritStr,"%+3.2f",rit/1000.0);
  }
  displayStr(ritStr);
  setFreq(freq);
  }
}


void setSSBMic(int mic)
{
  char micStr[10];
  sprintf(micStr,"G%d",mic);
  sendFifo(micStr);
}

void setFMMic(int mic)
{
  char micStr[10];
  sprintf(micStr,"g%d",mic);
  sendFifo(micStr);
}

void setAMMic(int mic)
{
  char micStr[10];
  sprintf(micStr,"d%d",mic);
  sendFifo(micStr);
}

void setCTCSS(int t)
{
  char ctStr[10];
  sprintf(ctStr,"C%d",CTCSSTone[t]);
  sendFifo(ctStr);
}

void setKey(int k)
{
  char kStr[5];
  sprintf(kStr,"K%d",k);
  sendFifo(kStr);
}

void setMute(int m)
{
  if(squelchGate == 0) m=1;
  char mStr[5];
  sprintf(mStr,"U%d",m);
  sendFifo(mStr);
}

void setRxFilter(int low,int high)
{
  char filtStr[10];
  sprintf(filtStr,"I%d",low);
  sendFifo(filtStr);                                             
  sprintf(filtStr,"F%d",high);
  sendFifo(filtStr);
  
  rxFilterLow = low;
  rxFilterHigh = high;  
}

void setTxFilter(int low,int high)
{
  char filtStr[10];
  sprintf(filtStr,"i%d",low);
  sendFifo(filtStr);
  sprintf(filtStr,"f%d",high);
  sendFifo(filtStr);  
}


void setFFTBW(int bw)
{
  char BWStr[10];
  sprintf(BWStr,"W%d",bw);
  sendFifo(BWStr);
  
  switch(bw)
  {
  case 0:             //48KHz   Sample rate
  HzPerBin = 94;
  break;
  case 1:             //24KHz   Sample rate
  HzPerBin = 47;
  break; 
  case 2:             //12KHz   Sample rate
  HzPerBin = 23;
  break;
  case 3:             //6KHz   Sample rate
  HzPerBin = 12;
  break;
  }
}


void setMode(int md)
{
  bandMode[band]=md;
  gotoXY(modeX,modeY);
  setForeColour(255,255,0);
  textSize=2;
  displayStr(modename[md]);
  if((md==FM)&&(bandDuplex[band]==1))
    {
    displayStr("DUP");
    }
  else
    {
    displayStr("   ");
    }      
  if(md==USB)
    {
    sendFifo("M0");    //USB
    setTxFilter(300,3000);    //USB Filter Setting
    setRxFilter(bandSSBFiltLow[band],bandSSBFiltHigh[band]);    //USB Filter Setting    configured in settings.
    setFreq(freq);    //set the frequency to adjust for CW offset.
    ritButton(1);
    setRit(0);
    } 
  
  if(md==LSB)
    {
    sendFifo("M1");    //LSB
    setTxFilter(-3000,-300); // LSB Filter Setting
    setRxFilter(-1*bandSSBFiltHigh[band],-1*bandSSBFiltLow[band]); // LSB Filter Setting
    setFreq(freq);    //set the frequency to adjust for CW offset.
    ritButton(1);
    setRit(0);
    } 
  
  if(md==CW)
    {
    sendFifo("M2");    //CW
    setRxFilter(bandSSBFiltLow[band],bandSSBFiltHigh[band]);   // USB filter settings used for CW Wide Filter
    setTxFilter(-100,100); // CW Filter Setting
    setFreq(freq);    //set the frequency to adjust for CW offset.
    ritButton(1);
    setRit(0);
    } 
    
  if(md==CWN)
    {
    sendFifo("M3");    //CWN
    setRxFilter(600,1000);    //CW Narrow Filter
    setTxFilter(-100,100); // CW Filter Setting
    setFreq(freq);    //set the frequency to adjust for CW offset.
    ritButton(1);
    setRit(0);
    } 
  if(md==FM)
    {
    sendFifo("M4");    //FM
    setRxFilter(-7500,7500);    //FM Filter
    setTxFilter(-7500,7500);    //FM Filter  
    setFreq(freq);    //set the frequency to adjust for CW offset.
    ritButton(0);
    setRit(0);
    } 
  if(md==AM)
    {
    sendFifo("M5");    //AM
    setRxFilter(-3000,3000);    //AM Filter 
    setTxFilter(-3000,3000);    //AM Filter 
    setFreq(freq);    //set the frequency to adjust for CW offset.
    ritButton(0);
    setRit(0);
    } 

    squelch=bandSquelch[band][mode];
    setSquelch(squelch);
                                                                 
configCounter=configDelay;
}

void setTxPin(int v)
{
  if(v==1) lgGpioWrite(gpiohandle,txPin,1); else lgGpioWrite(gpiohandle,txPin,0);  
}

void setTx(int pt)
{
  if((pt==1)&&(transmitting==0))
    {
      setTxPin(1);
      setBandBits(bandBitsTx[band]);
      plutoGpo=plutoGpo | 0x10;
      setPlutoGpo(plutoGpo);                               //set the Pluto GPO Pin 
      usleep(TXDELAY);
      setHwTxFreq(freq);
      if((mode==FM)&&(bandDuplex[band]==1))
        {
        displayFreq(freq+bandRepShift[band]);
        displayMenu();
        }
      if(enablePlutoTx) PlutoTxEnable(1);
      if (moni==0) setMute(1);                        //mute the receiver
      if(satMode()==0)
      {
        sMeter=0;
        setHwRxFreq(freq+10.0);              //offset the Rx frequency to prevent unwanted mixing. (happens even if disabled!)
        PlutoRxEnable(0);
      }
      if(satMode()==0)
      {
        clearWaterfall();
      }
      sendFifo("T");
      gotoXY(txX,txY);
      setForeColour(255,0,0);
      textSize=2;
      displayStr("Tx");
      transmitting=1;  
    }
  else if((pt==0)&&(transmitting==1))
    {
      if(satMode()==0)
      {
      sMeter=0;
      clearWaterfall();
      }
      
      sendFifo("R");
      setMute(0);                  //unmute the receiver 
      setHwTxFreq(freq+10.0);           //offset the Tx freq to prevent unwanted spurious
      PlutoTxEnable(0);
      PlutoRxEnable(1);
      setHwRxFreq(freq);
      if((mode==FM)&&(bandDuplex[band]==1))
        {
        displayFreq(freq);
        displayMenu();
        }
      gotoXY(txX,txY);
      setForeColour(0,255,0);
      textSize=2;
      displayStr("Rx");                                                                           
      transmitting=0;
      usleep(RXDELAY);
      setTxPin(0);
      setBandBits(bandBitsRx[band]);
      plutoGpo=plutoGpo & 0xEF;
      setPlutoGpo(plutoGpo);                               //clear the Pluto GPO Pin 
    }
}

void setHwRxFreq(double fr)
{
  long long rxoffsethz;
  long long LOrxfreqhz;
  long long rxfreqhz;
  double frRx;
  double frTx;
  
  frRx=fr+bandRxOffset[band];
  
  
  if(bandRxHarmonic[band] < 2)
  {
    if(frRx > maxHwFreq)
    {
    frRx = maxHwFreq;
    }
    if(frRx < minHwFreq)
    {
    frRx = minHwFreq;
    }
  }
  
  rxfreqhz=frRx*1000000;
  
  if (rxfreqhz<69900000) rxfreqhz=69900000;         //this is the lowest frequency we can receive with a pluto 
  
  if(rxfreqhz<70100000)
  {
  rxoffsethz=(rxfreqhz-70000000);        //Special case for receiving below 70.100     Use the offset of +-100KHz
  LOrxfreqhz=70000000;
  }
  else
  {
  rxoffsethz=(rxfreqhz % 100000)+50000;        //use just the +50Khz to +150Khz positive side of the sampled spectrum. This avoids seeing the DC hump .
  LOrxfreqhz=rxfreqhz-rxoffsethz;
  }

  if( bandRxHarmonic[band]>1)                                //allow for harmonic mixing for higher bands (10GHz)
    {
      LOrxfreqhz=LOrxfreqhz/bandRxHarmonic[band];
    }
  

  rxoffsethz=rxoffsethz+rit;
  if((mode==CW)|(mode==CWN))
    {
     rxoffsethz=rxoffsethz-800;         //offset  for CW tone of 800 Hz    
    }
  if(LOrxfreqhz!=lastLOhz)         
    {
      setPlutoRxFreq(LOrxfreqhz);          //Control Pluto directly to bypass problems with Gnu Radio Sink
      lastLOhz=LOrxfreqhz;
    }
  
  char offsetStr[32];
  sprintf(offsetStr,"O%d",rxoffsethz);   //send the rx offset tuning value 
  sendFifo(offsetStr);
}

void setHwTxFreq(double fr)
{                                                                        
  long long txfreqhz;
  double frTx;
  
  frTx=fr+bandTxOffset[band];
  
 
  if(bandTxHarmonic[band] < 2)
  {
    if(frTx > maxHwFreq)
    {
    frTx = maxHwFreq;
    }
    if(frTx < minHwFreq)
    {
    frTx = minHwFreq;
    }
  }
  
  if((mode==FM)&&(bandDuplex[band]==1))
    {
    frTx=frTx+bandRepShift[band];
    }
 
  txfreqhz=frTx*1000000;
   
  if(bandTxHarmonic[band]>1)                    //allow for Harmonic mixing for higher bands or for external multiplier such as Hydra
    {
    txfreqhz= txfreqhz/bandTxHarmonic[band];
    } 
  
  setPlutoTxFreq(txfreqhz);          //Control Pluto directly to bypass problems with Gnu Radio Sink
}

void displayFreq(double fr)
{
  long long freqhz;
  char digit[16]; 
  
  fr=fr+0.0000001;   // correction for rounding errors.
  freqhz=fr*1000000;    
  freqhz=freqhz+100000000000;     //force it to be 12 digits long
  sprintf(digit,"%lld",freqhz);
  
  gotoXY(freqDisplayX,freqDisplayY);
  setForeColour(0,0,255);
  textSize=6;
  if(digit[1]>'0') displayChar(digit[1]); else displayChar(' ');
  if((digit[1]>'0') | (digit[2]>'0')) displayChar(digit[2]); else displayChar(' ');
  if((digit[1]>'0') | (digit[2]>'0') | digit[3]>'0')  displayChar(digit[3]); else displayChar(' ');
  displayChar(digit[4]);
  displayChar(digit[5]);
  displayChar('.');
  displayChar(digit[6]);
  displayChar(digit[7]);
  displayChar(digit[8]);
  displayChar('.');
  displayChar(digit[9]);
  displayChar(digit[10]);
  
// Underline the currently selected tuning digit
  
  for(int dtd=0;dtd<12;dtd++)
    {
      gotoXY(freqDisplayX+dtd*freqDisplayCharWidth+4,freqDisplayY+freqDisplayCharHeight+5);
      int bb = 0;
      if (dtd==tuneDigit) bb=255;
      for(int p=0; p < freqDisplayCharWidth; p++)
        {
          setPixel(currentX+p,currentY+1,0,0,bb);
          setPixel(currentX+p,currentY+2,0,0,bb);
          setPixel(currentX+p,currentY+3,0,0,bb);
        }
    }

}

void setFreq(double fr)
{

  
  if(ptt | ptts) 
  {
    setHwTxFreq(fr);        //set Hardware Tx frequency if we are transmitting
  }
  else
  {
  setHwRxFreq(fr);       //set Hardware Rx frequency if we are receiving
  }

displayFreq(fr);
 
//set XVTR, SAT, SPLIT or MULT indication if needed.
  gotoXY(txvtrX,txvtrY);
  setForeColour(0,255,0);
  textSize=2;
  if(multMode()==1)
    {
    displayStr(" MULT  ");
    }  
  else if( txvtrMode()==1)
    {
     displayStr(" XVTR  "); 
    }
  else if(satMode()==1)
    {
    displayStr("  SAT  ");
    }
  else if(splitMode()==1)
    {
    displayStr(" SPLIT ");
    } 
  else
    {
      displayStr("       ");
    } 
 
   gotoXY(funcButtonsX+buttonSpaceX*4,funcButtonsY);
   setForeColour(0,255,0);
   if(inputMode==FREQ)
   {
     if(satMode()==1)
      {
       displayButton("MONI");
       setMoni(moni);
      }  
      else
      {
       displayButton("    ");
       setMoni(0);
      }
    }
 
 configCounter=configDelay;                       //write config after this amount of inactivity    
}

int satMode(void)
{
if(((abs(bandTxOffset[band]-bandRxOffset[band]) > 1 ) & (bandRxOffset[band]!=0) ) & bandRxHarmonic[band]<2  & bandTxHarmonic[band]<2 )     // if we have a differnt Rx and Tx offset and we are not multiplying then we must be in Sat mode. 
  {
  return 1;
  }
else
  {
  return 0;
  }
}

int txvtrMode(void)
{
if((abs(bandTxOffset[band]-bandRxOffset[band]) <1) & (abs(bandTxOffset[band]) >1 )  )       //if the tx and rx offset are the same and non zero then we are in Transverter mode
  {
  return 1;
  }
else
  {
  return 0;
  }
}

int splitMode(void)
{
if((abs(bandTxOffset[band])>0) & (bandRxOffset[band]==0))     //  if tx Offset is non zero and rxoffset is zero then we are in split mode. 
  {
  return 1;
  }
else
  {
  return 0;
  }
}

int multMode(void)
{
if((bandRxHarmonic[band]>1) || (bandTxHarmonic[band] >1))     //  if either of the Harmonic modes is set then we are in Mult mode. 
  {
  return 1;
  }
else
  {
  return 0;
  }
}

void setDialLock(int d)
{
  if(d==0)
  {
    dialLock=0;
    gotoXY(diallockX,diallockY);
    textSize=2;
    displayStr("    ");
  }
  else
  {
    dialLock=1;
    gotoXY(diallockX,diallockY);
    textSize=2;
    setForeColour(255,0,0);
    displayStr("LOCK");
  }
}

void setMoni(int m)
{
  if(m==1)
    {
     setMute(0);
     moni=1;
     gotoXY(moniX,moniY);
     textSize=2;
     setForeColour(0,255,0);
     displayStr("MONI");
    } 
  else
    {
     if (ptt | ptts) setMute(1);
     moni=0;
     gotoXY(moniX,moniY);
     textSize=2;
     displayStr("    ");
    }
}

void send1750(void)
{
sendFifo("B1");
gotoXY(funcButtonsX+buttonSpaceX*2,funcButtonsY);
setForeColour(255,0,0);
displayButton("1750");
usleep(BurstLength);
sendFifo("B0");
gotoXY(funcButtonsX+buttonSpaceX*2,funcButtonsY);
setForeColour(0,255,0);
displayButton("1750");
}

void setBandBits(int b)
{
    if(b & 0x01) 
        {
        lgGpioWrite(gpiohandle,bandPin1,1);
        lgGpioWrite(gpiohandle,bandPin1alt,1);
        }
    else
        {
        lgGpioWrite(gpiohandle,bandPin1,0);
        lgGpioWrite(gpiohandle,bandPin1alt,0);
        }
        
    if(b & 0x02) 
        {
        lgGpioWrite(gpiohandle,bandPin2,1);
        }
    else
        {
        lgGpioWrite(gpiohandle,bandPin2,0);
        }   
    
    if(b & 0x04) 
        {
        lgGpioWrite(gpiohandle,bandPin3,1);
        }
    else
        {
        lgGpioWrite(gpiohandle,bandPin3,0);
        }   
    
    if(b & 0x08) 
        {
        lgGpioWrite(gpiohandle,bandPin4,1);
        }
    else
        {
        lgGpioWrite(gpiohandle,bandPin4,0);
        }   
    
    if(b & 0x10) 
        {
        lgGpioWrite(gpiohandle,bandPin5,1);
        }
    else
        {
        lgGpioWrite(gpiohandle,bandPin5,0);
        }   
    
    if(b & 0x20) 
        {
        lgGpioWrite(gpiohandle,bandPin6,1);
        }
    else
        {
        lgGpioWrite(gpiohandle,bandPin6,0);
        }   
        
    if(b & 0x40) 
        {
        lgGpioWrite(gpiohandle,bandPin7,1);
        }
    else
        {
        lgGpioWrite(gpiohandle,bandPin7,0);
        }   
        
    if(b & 0x80) 
        {
        lgGpioWrite(gpiohandle,bandPin8,1);
        }
    else
        {
        lgGpioWrite(gpiohandle,bandPin8,0);
        }       

//  copy bits 0,1 and 2to Pluto GPO Pins if enabled

 if(bandBitsToPluto==1)
 {
  if(b & 0x01) 
      {
      plutoGpo=plutoGpo | 0x20;
      }
  else
      {
      plutoGpo=plutoGpo & 0xDF;
      }
      
  if(b & 0x02) 
      {
      plutoGpo=plutoGpo | 0x40;
      }
  else
      {
      plutoGpo=plutoGpo & 0xBF;
      }   
  
  if(b & 0x04) 
      {
      plutoGpo=plutoGpo | 0x80;
      }
  else
      {;
      plutoGpo=plutoGpo & 0x7F;
      }   
  setPlutoGpo(plutoGpo);
 } 
 else
 {
   plutoGpo=plutoGpo & 0x1F;
   setPlutoGpo(plutoGpo);
 }
 
  
}


void changeNB1Setting(void)
{
  if(nb1SettingNo==0)        // Algorithm
    {
    if(mouseScroll > 0) nb1Algorithm++;
    if(mouseScroll < 0) nb1Algorithm--;
    mouseScroll=0;
    if(nb1Algorithm < 0) nb1Algorithm=0;
    if(nb1Algorithm > 2) nb1Algorithm=2;
    displayNB1Setting(nb1SettingNo);
    }
  if(nb1SettingNo==1)        // FFT Size (powers of 2: 64→1024)
    {
    if(mouseScroll > 0 && nb1FftSize < 1024) nb1FftSize *= 2;
    if(mouseScroll < 0 && nb1FftSize >   64) nb1FftSize /= 2;
    mouseScroll=0;
    displayNB1Setting(nb1SettingNo);
    }
  if(nb1SettingNo==2)        // Overlap (powers of 2: 2→16)
    {
    if(mouseScroll > 0 && nb1Overlap < 16) nb1Overlap *= 2;
    if(mouseScroll < 0 && nb1Overlap >  2) nb1Overlap /= 2;
    mouseScroll=0;
    displayNB1Setting(nb1SettingNo);
    }
  if(nb1SettingNo==3)        // Alpha (×100: 80→99)
    {
    nb1Alpha += mouseScroll;
    mouseScroll=0;
    if(nb1Alpha < 80) nb1Alpha=80;
    if(nb1Alpha > 99) nb1Alpha=99;
    displayNB1Setting(nb1SettingNo);
    }
  if(nb1SettingNo==4)        // Beta (×10: 5→50)
    {
    nb1Beta += mouseScroll;
    mouseScroll=0;
    if(nb1Beta <  5) nb1Beta= 5;
    if(nb1Beta > 50) nb1Beta=50;
    displayNB1Setting(nb1SettingNo);
    }
  if(nb1SettingNo==5)        // Gain Floor (×1000: 1→100)
    {
    nb1GainFloor += mouseScroll;
    mouseScroll=0;
    if(nb1GainFloor <   1) nb1GainFloor=  1;
    if(nb1GainFloor > 100) nb1GainFloor=100;
    displayNB1Setting(nb1SettingNo);
    }
}

void changeSetting(void)
{
  if(settingNo==SSB_MIC)        //SSB Mic Gain
      {
      SSBMic=SSBMic+mouseScroll;
      mouseScroll=0;
      if(SSBMic<0) SSBMic=0;
      if(SSBMic>maxSSBMic) SSBMic=maxSSBMic;
      setSSBMic(SSBMic);
      displaySetting(settingNo);
      }
   if(settingNo==FM_MIC)        // FM Mic Gain
      {
      FMMic=FMMic+mouseScroll;
      mouseScroll=0;
      if(FMMic<0) FMMic=0;
      if(FMMic>maxFMMic) FMMic=maxFMMic;
      setFMMic(FMMic);
      displaySetting(settingNo);
      }
   if(settingNo==AM_MIC)        // AM Mic Gain
      {
      AMMic=AMMic+mouseScroll;
      mouseScroll=0;
      if(AMMic<0) AMMic=0;
      if(AMMic>maxAMMic) AMMic=maxAMMic;
      setAMMic(AMMic);
      displaySetting(settingNo);
      }
    if(settingNo==REP_SHIFT)        //Repeater Shift
      {
        bandRepShift[band]=bandRepShift[band]+mouseScroll*freqInc;
        mouseScroll=0;
        setFreq(freq);
        displaySetting(settingNo);
      }  
    if(settingNo==CTCSS)        //CTCSS Tone
      {
        bandCTCSS[band]=bandCTCSS[band]+mouseScroll;
        if(bandCTCSS[band] < 0 ) bandCTCSS[band]=0;
        if(bandCTCSS[band] > (NUMCTCSS - 1) ) bandCTCSS[band]= NUMCTCSS - 1;        
        mouseScroll=0;
        setCTCSS(bandCTCSS[band]);
        displaySetting(settingNo);
      }  
   if(settingNo==RX_OFFSET)        //Transverter Rx Offset 
      {
        bandRxOffset[band]=bandRxOffset[band]-mouseScroll*freqInc;
        if(bandRxOffset[band] > 99999.9) bandRxOffset[band]= 99999.9;
        if(bandRxOffset[band] < -99999.9) bandRxOffset[band]= -99999.9;
        freq=freq+mouseScroll*freqInc;
        if((freq + bandRxOffset[band]) > maxHwFreq )
        {
          freq = freq - ((freq + bandRxOffset[band]) - maxHwFreq); 
        }
        
        if((freq + bandRxOffset[band]) < minHwFreq )
        {
          freq = freq + ( minHwFreq - (freq + bandRxOffset[band])); 
        }  
            
        mouseScroll=0;
        setFreq(freq);
        displaySetting(settingNo);
      } 
 if(settingNo==RX_HARMONIC)        // RX Harmonic mixing number
      {
      if(mouseScroll>0)
      {
        bandRxHarmonic[band]=5;
      }
      if(mouseScroll<0)
      {
         bandRxHarmonic[band]=1;
      }
      mouseScroll=0;
      setFreq(freq);
      displaySetting(settingNo);                                                                 
      }    
   if(settingNo==TX_OFFSET)        //Transverter Tx Offset
      {
        bandTxOffset[band]=bandTxOffset[band]-mouseScroll*freqInc;
        if(bandTxOffset[band] > 99999.9) bandTxOffset[band]= 99999.9;
        if(bandTxOffset[band] < -99999.9) bandTxOffset[band]= -99999.9;
 
        freq=freq+mouseScroll*freqInc;
  
        if((freq + bandTxOffset[band]) > maxHwFreq )
        {
          freq = freq - ((freq + bandTxOffset[band]) - maxHwFreq); 
        }
        
        if((freq + bandTxOffset[band]) < minHwFreq )
        {
          freq = freq + ( minHwFreq - (freq + bandTxOffset[band])); 
        }
        
        mouseScroll=0;
        setFreq(freq);
        displaySetting(settingNo);
      }  
 if(settingNo==TX_HARMONIC)        // TX Harmonic mixing number or external multiplier
      {
      bandTxHarmonic[band]=bandTxHarmonic[band]+mouseScroll;
      if(bandTxHarmonic[band]<1) bandTxHarmonic[band]=1;
      if(bandTxHarmonic[band]>5) bandTxHarmonic[band]=5;
      if(mouseScroll>0)
      {
        if(bandTxHarmonic[band]==4) bandTxHarmonic[band]=5;
        if(bandTxHarmonic[band]==3) bandTxHarmonic[band]=5;
      }
      else
      {
        if(bandTxHarmonic[band]==4) bandTxHarmonic[band]=2;
        if(bandTxHarmonic[band]==3) bandTxHarmonic[band]=2;
      }
      mouseScroll=0;
      setFreq(freq);
      displaySetting(settingNo);  
      }      
   if(settingNo==BAND_BITS_RX)        // Band Bits Rx
      {
      if(setIndex==7)
        {
        bandBitsRx[band]=bandBitsRx[band] ^ 0x01; 
        }
      if(setIndex==6)
        {
        bandBitsRx[band]=bandBitsRx[band] ^ 0x02; 
        }
      if(setIndex==5)
        {
        bandBitsRx[band]=bandBitsRx[band] ^ 0x04; 
        }
      if(setIndex==4)
        {
        bandBitsRx[band]=bandBitsRx[band] ^ 0x08; 
        }
       if(setIndex==3)
        {
        bandBitsRx[band]=bandBitsRx[band] ^ 0x10; 
        }
      if(setIndex==2)
        {
        bandBitsRx[band]=bandBitsRx[band] ^ 0x20; 
        }
      if(setIndex==1)
        {
        bandBitsRx[band]=bandBitsRx[band] ^ 0x40; 
        }
      if(setIndex==0)
        {
        bandBitsRx[band]=bandBitsRx[band] ^ 0x80; 
        }
      mouseScroll=0;
      if(bandBitsRx[band]<0) bandBitsRx[band]=0;
      if(bandBitsRx[band]>255) bandBitsRx[band]=255;
      setBandBits(bandBitsRx[band]);
      displaySetting(settingNo);  
      }  
if(settingNo==BAND_BITS_TX)        // Band Bits Tx
      {
      if(setIndex==7)
        {
        bandBitsTx[band]=bandBitsTx[band] ^ 0x01; 
        }
      if(setIndex==6)
        {
        bandBitsTx[band]=bandBitsTx[band] ^ 0x02; 
        }
      if(setIndex==5)
        {
        bandBitsTx[band]=bandBitsTx[band] ^ 0x04; 
        }
      if(setIndex==4)
        {
        bandBitsTx[band]=bandBitsTx[band] ^ 0x08; 
        }
       if(setIndex==3)
        {
        bandBitsTx[band]=bandBitsTx[band] ^ 0x10; 
        }
      if(setIndex==2)
        {
        bandBitsTx[band]=bandBitsTx[band] ^ 0x20; 
        }
      if(setIndex==1)
        {
        bandBitsTx[band]=bandBitsTx[band] ^ 0x40; 
        }
      if(setIndex==0)
        {
        bandBitsTx[band]=bandBitsTx[band] ^ 0x80; 
        }
      mouseScroll=0;
      if(bandBitsTx[band]<0) bandBitsTx[band]=0;
      if(bandBitsTx[band]>255) bandBitsTx[band]=255;
      displaySetting(settingNo);  
      }  
    if(settingNo==BAND_BITS_TO_PLUTO)        // Copy Band Bits to Pluto
      {
      if(mouseScroll>0)
      {
        bandBitsToPluto=1;
      }
      if(mouseScroll<0)
      {
         bandBitsToPluto=0;
      }
      mouseScroll=0;
      displaySetting(settingNo);  
      }                 
   if(settingNo==FFT_REF)        // FFT Ref Level
      {
      FFTRef=FFTRef+mouseScroll;
      mouseScroll=0;
      if(FFTRef<-80) FFTRef=-80;
      if(FFTRef>30) FFTRef=30;
      bandFFTRef[band]=FFTRef;
      displaySetting(settingNo);
      }
   if(settingNo==FFT_RANGE)     // FFT Dynamic Range (dB)
      {
      FFTRange=FFTRange+mouseScroll;
      mouseScroll=0;
      if(FFTRange<10)  FFTRange=10;
      if(FFTRange>120) FFTRange=120;
      bandFFTRange[band]=FFTRange;
      displaySetting(settingNo);
      }
    if(settingNo==TX_ATT)        // Tx Attenuator
      {
      TxAtt=TxAtt+mouseScroll;
      mouseScroll=0;
      if(TxAtt<-89) TxAtt=-89;
      if(TxAtt>0) TxAtt=0;
      bandTxAtt[band]=TxAtt;
      setPlutoTxAtt(TxAtt);
      displaySetting(settingNo);  
      }  
     if(settingNo==RX_GAIN)        // Rx Gain Setting
      {
      if (bandRxGain[band] == 100)
        {
        bandRxGain[band]=maxGain(freq)+1+mouseScroll;
        }
      else
      {
        bandRxGain[band]=bandRxGain[band]+mouseScroll;
      }

      mouseScroll=0;
      if(bandRxGain[band]< minGain(freq)) bandRxGain[band]=minGain(freq);
      if(bandRxGain[band]> maxGain(freq)) bandRxGain[band]=100;
      setPlutoRxGain(bandRxGain[band]);
      displaySetting(settingNo);  
      }                   
    if(settingNo==S_ZERO)        // S Meter Zero
      {
      bandSmeterZero[band]=bandSmeterZero[band]+mouseScroll;
      mouseScroll=0;
      if(bandSmeterZero[band]<-140) bandSmeterZero[band]=-140;
      if(bandSmeterZero[band]>-30) bandSmeterZero[band]=-30;
      displaySetting(settingNo);  
      }   
    if(settingNo==SSB_FILT_LOW)        // SSB Filter Low
      {
      bandSSBFiltLow[band]=bandSSBFiltLow[band]+mouseScroll*10;
      mouseScroll=0;
      if(bandSSBFiltLow[band] < 0) bandSSBFiltLow[band]=0;
      if(bandSSBFiltLow[band] >1000) bandSSBFiltLow[band]=1000;
      setMode(mode);                 //refresh mode to set new filter settings
      displaySetting(settingNo);  
      }                    
    if(settingNo==SSB_FILT_HIGH)        // SSB Filter High
      {
      bandSSBFiltHigh[band]=bandSSBFiltHigh[band]+mouseScroll*10;
      mouseScroll=0;
      if(bandSSBFiltHigh[band] < 1000) bandSSBFiltHigh[band]=1000;
      if(bandSSBFiltHigh[band] > 5000) bandSSBFiltHigh[band]=5000;
      setMode(mode);                 //refresh mode to set new filter settings
      displaySetting(settingNo);  
      }     
    if(settingNo==CW_CARRIER)        // CWID Carrier time
      {
      CWIDkeyDownTime=CWIDkeyDownTime+mouseScroll*100;
      mouseScroll=0;
      if(CWIDkeyDownTime< 0) CWIDkeyDownTime=0;
      if(CWIDkeyDownTime> 12000) CWIDkeyDownTime=12000;
      displaySetting(settingNo);  
      }  
      
     if(settingNo==CWID)            //CW Ident string
     {
     int c;
     c= morseIdent[setIndex];
     c=c+mouseScroll;
     if(mouseScroll>0)
       {
         if(c>95) c=47;
         if((c>57)&&(c<65)) c=65;
         if(c>90) c=95;
       }
     if(mouseScroll<0)
       {
          if(c<47) c=95;
          if((c>90)&&(c<95)) c=90;
          if((c>57)&&(c<65)) c=57;
       }
     
     morseIdent[setIndex] = c;
     mouseScroll=0;
     displaySetting(settingNo); 
     }  
     
  if(settingNo==BREAK_IN_TIME)        // CW Break In Timer
      {
      breakInTime=breakInTime+mouseScroll;
      mouseScroll=0;
      if(breakInTime< 50) breakInTime=50;
      if(breakInTime> 200) breakInTime=200;
      displaySetting(settingNo);  
      } 
      
  if(settingNo==BANDS24)        // 24 band mode
      {
      if(mouseScroll > 0)   bands24= 1;
      if(mouseScroll < 0)   bands24= 0;
      mouseScroll=0;
      displaySetting(settingNo);
      }

  if(settingNo==ENABLE_GPIO_PTT)        // GPIO 17 PTT enable
      {
      if(mouseScroll > 0)   enableGPIOPTT= 1;
      if(mouseScroll < 0)   enableGPIOPTT= 0;
      mouseScroll=0;
      displaySetting(settingNo);
      }

  if(settingNo==ENABLE_CW_KEY)        // CW key enable
      {
      if(mouseScroll > 0)   enableCWKey= 1;
      if(mouseScroll < 0)   enableCWKey= 0;
      mouseScroll=0;
      displaySetting(settingNo);
      }

  if(settingNo==ENABLE_PLUTO_TX)        // Pluto TX output enable
      {
      if(mouseScroll > 0)   enablePlutoTx= 1;
      if(mouseScroll < 0)   enablePlutoTx= 0;
      mouseScroll=0;
      displaySetting(settingNo);
      }

  if(settingNo==ALSA_CAPTURE)           // ALSA Mic Capture Level (0→100 %)
      {
      alsaCaptureLevel += mouseScroll;
      mouseScroll=0;
      if(alsaCaptureLevel <   0) alsaCaptureLevel=  0;
      if(alsaCaptureLevel > 100) alsaCaptureLevel=100;
      setAlsaCapture(alsaCaptureLevel);
      displaySetting(settingNo);
      }

  if(settingNo==ROTATE)        // Rotate Screen
      {
      if(mouseScroll > 0)   screenrotate= 1;
      if(mouseScroll < 0)   screenrotate= 0;
      mouseScroll=0;
      setRotation(screenrotate);
      initGUI(); 
      showSettingsMenu(); 
      displaySetting(settingNo);  
      }                                                                                                                    
}

               
int minGain(double freq)
{
double rxfreq;

rxfreq=(freq+bandRxOffset[band])/bandRxHarmonic[band];

if(rxfreq<1300)
 {
 return -1;
 }
if((rxfreq>=1300) && (rxfreq<4000))
  {
  return -3;
  }
if(rxfreq>=4000)
  {
  return -10;
  }
  
return 0;
}

int maxGain(double freq)
{
double rxfreq; 

rxfreq=(freq+bandRxOffset[band])/bandRxHarmonic[band];

if(rxfreq<1300)
 {
 return 73;
 }
if((rxfreq>=1300) && (rxfreq<4000))
  {
  return 71;
  }
if(rxfreq>=4000)
  {
  return 62;
  }
  
return 73;
}


void showSettingsMenu(void)
  {
    gotoXY(funcButtonsX,funcButtonsY);
    setForeColour(0,255,0);
    displayButton("MENU");
    displayButton(" ");
    displayButton("NEXT");
    displayButton("PREV");
    setForeColour(255,0,0);
    displayButton1x12("RESTART");
    if (portsdownPresent==1)
    {
        displayButton2x12("EXIT TO","PORTSDOWN");
    }
    else
    {
        displayButton1x12("EXIT");
    }
    displayButton1x12("SHUTDOWN");
  }


void showExtraMenu(void)
  {
    gotoXY(funcButtonsX,funcButtonsY);
    setForeColour(0,255,0);
    displayButton("MENU");
    if(nb1Active) setForeColour(255,0,0); else setForeColour(0,0,255);
    displayButton("NB1");
    if(compActive) setForeColour(255,0,0); else setForeColour(0,0,255);
    displayButton("COMP");
    setForeColour(0,0,255);
    displayButton("    ");
    displayButton("    ");
    displayButton("    ");
    setForeColour(255,165,0);
    displayButton("SET");
  }


void showNB1SettingsMenu(void)
  {
    gotoXY(funcButtonsX,funcButtonsY);
    setForeColour(0,255,0);
    displayButton("MENU");
    setForeColour(0,0,255);
    displayButton("    ");
    displayButton("NEXT");
    displayButton("PREV");
    setForeColour(255,165,0);
    displayButton("COMP>");
    displayButton("APPLY");
    setForeColour(0,0,255);
    displayButton("    ");
  }


void showCOMPSettingsMenu(void)
  {
    gotoXY(funcButtonsX,funcButtonsY);
    setForeColour(0,255,0);
    displayButton("MENU");
    setForeColour(0,0,255);
    displayButton("    ");
    displayButton("NEXT");
    displayButton("PREV");
    setForeColour(255,165,0);
    displayButton("NB1>");
    displayButton("APPLY");
    setForeColour(0,0,255);
    displayButton("    ");
  }


void displayCOMPSetting(int se)
  {
    char valStr[30];
    gotoXY(0,settingY);
    textSize=2;
    setForeColour(255,165,0);
    displayStr("                                                ");
    gotoXY(0,settingY+8);
    displayStr("                                                ");
    gotoXY(settingX,settingY);
    displayStr(compSettingText[se]);
    if(se==0)
      { sprintf(valStr,"0.%03d",compAgcAttack); displayStr(valStr); }
    if(se==1)
      { sprintf(valStr,"0.%03d",compAgcDecay); displayStr(valStr); }
    if(se==2)
      { sprintf(valStr,"0.%02d",compAgcRef); displayStr(valStr); }
    if(se==3)
      { sprintf(valStr,"%d",compAgcMax); displayStr(valStr); }
    if(se==4)
      { sprintf(valStr,"%d Hz",compLpfCutoff); displayStr(valStr); }
    if(se==5)
      { sprintf(valStr,"%d Hz",compEq1Freq); displayStr(valStr); }
    if(se==6)
      { sprintf(valStr,"%+d.%d dB",compEq1Gain/10,abs(compEq1Gain%10)); displayStr(valStr); }
    if(se==7)
      { sprintf(valStr,"%d Hz",compEq2Freq); displayStr(valStr); }
    if(se==8)
      { sprintf(valStr,"%+d.%d dB",compEq2Gain/10,abs(compEq2Gain%10)); displayStr(valStr); }
    if(se==9)
      { sprintf(valStr,"%d Hz",compEq3Freq); displayStr(valStr); }
    if(se==10)
      { sprintf(valStr,"%+d.%d dB",compEq3Gain/10,abs(compEq3Gain%10)); displayStr(valStr); }
    if(se==11)
      { sprintf(valStr,"%d %%",compMicGain); displayStr(valStr); }
  }


void sendCOMPParams(void)
  {
    char cmd[20];
    sprintf(cmd,"s%d",compAgcAttack);  sendFifo(cmd);
    sprintf(cmd,"t%d",compAgcDecay);   sendFifo(cmd);
    sprintf(cmd,"u%d",compAgcRef);     sendFifo(cmd);
    sprintf(cmd,"v%d",compAgcMax);     sendFifo(cmd);
    sprintf(cmd,"w%d",compLpfCutoff);  sendFifo(cmd);
    sprintf(cmd,"h%d",compEq1Freq);    sendFifo(cmd);
    sprintf(cmd,"k%d",compEq1Gain);    sendFifo(cmd);
    sprintf(cmd,"m%d",compEq2Freq);    sendFifo(cmd);
    sprintf(cmd,"n%d",compEq2Gain);    sendFifo(cmd);
    sprintf(cmd,"o%d",compEq3Freq);    sendFifo(cmd);
    sprintf(cmd,"q%d",compEq3Gain);    sendFifo(cmd);
    sprintf(cmd,"r%d",compMicGain);    sendFifo(cmd);
    sendFifo("c2");    // apply: rebuild chain once if COMP active
  }


void changeCompSetting(void)
{
  if(compSettingNo==0)        // AGC Attack (×1000: 1→500)
    {
    compAgcAttack += mouseScroll;
    mouseScroll=0;
    if(compAgcAttack <   1) compAgcAttack=  1;
    if(compAgcAttack > 500) compAgcAttack=500;
    displayCOMPSetting(compSettingNo);
    }
  if(compSettingNo==1)        // AGC Decay (×1000: 1→500)
    {
    compAgcDecay += mouseScroll;
    mouseScroll=0;
    if(compAgcDecay <   1) compAgcDecay=  1;
    if(compAgcDecay > 500) compAgcDecay=500;
    displayCOMPSetting(compSettingNo);
    }
  if(compSettingNo==2)        // AGC Reference (×100: 10→99)
    {
    compAgcRef += mouseScroll;
    mouseScroll=0;
    if(compAgcRef < 10) compAgcRef=10;
    if(compAgcRef > 99) compAgcRef=99;
    displayCOMPSetting(compSettingNo);
    }
  if(compSettingNo==3)        // AGC Max Gain (1→1000)
    {
    compAgcMax += mouseScroll;
    mouseScroll=0;
    if(compAgcMax <    1) compAgcMax=   1;
    if(compAgcMax > 1000) compAgcMax=1000;
    displayCOMPSetting(compSettingNo);
    }
  if(compSettingNo==4)        // LPF Cutoff (500→5000 Hz, step 100)
    {
    compLpfCutoff += mouseScroll*100;
    mouseScroll=0;
    if(compLpfCutoff <  500) compLpfCutoff= 500;
    if(compLpfCutoff > 5000) compLpfCutoff=5000;
    displayCOMPSetting(compSettingNo);
    }
  if(compSettingNo==5)        // EQ1 Freq (200→4000 Hz, step 50)
    {
    compEq1Freq += mouseScroll*50;
    mouseScroll=0;
    if(compEq1Freq <  200) compEq1Freq= 200;
    if(compEq1Freq > 4000) compEq1Freq=4000;
    displayCOMPSetting(compSettingNo);
    }
  if(compSettingNo==6)        // EQ1 Gain (×10 dB: -120→+120 = -12.0→+12.0 dB)
    {
    compEq1Gain += mouseScroll;
    mouseScroll=0;
    if(compEq1Gain < -120) compEq1Gain=-120;
    if(compEq1Gain >  120) compEq1Gain= 120;
    displayCOMPSetting(compSettingNo);
    }
  if(compSettingNo==7)        // EQ2 Freq (200→4000 Hz, step 50)
    {
    compEq2Freq += mouseScroll*50;
    mouseScroll=0;
    if(compEq2Freq <  200) compEq2Freq= 200;
    if(compEq2Freq > 4000) compEq2Freq=4000;
    displayCOMPSetting(compSettingNo);
    }
  if(compSettingNo==8)        // EQ2 Gain (×10 dB: -120→+120)
    {
    compEq2Gain += mouseScroll;
    mouseScroll=0;
    if(compEq2Gain < -120) compEq2Gain=-120;
    if(compEq2Gain >  120) compEq2Gain= 120;
    displayCOMPSetting(compSettingNo);
    }
  if(compSettingNo==9)        // EQ3 Freq (200→4000 Hz, step 50)
    {
    compEq3Freq += mouseScroll*50;
    mouseScroll=0;
    if(compEq3Freq <  200) compEq3Freq= 200;
    if(compEq3Freq > 4000) compEq3Freq=4000;
    displayCOMPSetting(compSettingNo);
    }
  if(compSettingNo==10)       // EQ3 Gain (×10 dB: -120→+120)
    {
    compEq3Gain += mouseScroll;
    mouseScroll=0;
    if(compEq3Gain < -120) compEq3Gain=-120;
    if(compEq3Gain >  120) compEq3Gain= 120;
    displayCOMPSetting(compSettingNo);
    }
  if(compSettingNo==11)       // COMP Mic Gain (0→100, maps to GNU Radio gain 0.0→1.0)
    {
    compMicGain += mouseScroll;
    mouseScroll=0;
    if(compMicGain <   0) compMicGain=  0;
    if(compMicGain > 100) compMicGain=100;
    displayCOMPSetting(compSettingNo);
    }
}

void displayNB1Setting(int se)
  {
    char valStr[30];
    gotoXY(0,settingY);
    textSize=2;
    setForeColour(255,165,0);
    displayStr("                                                ");
    gotoXY(0,settingY+8);
    displayStr("                                                ");
    gotoXY(settingX,settingY);
    displayStr(nb1SettingText[se]);
    if(se==0)
      {
      if(nb1Algorithm==0) displayStr("Wiener");
      else if(nb1Algorithm==1) displayStr("Over-sub");
      else displayStr("MMSE");
      }
    if(se==1)
      { sprintf(valStr,"%d",nb1FftSize); displayStr(valStr); }
    if(se==2)
      { sprintf(valStr,"%d",nb1Overlap); displayStr(valStr); }
    if(se==3)
      { sprintf(valStr,"0.%02d",nb1Alpha); displayStr(valStr); }
    if(se==4)
      { sprintf(valStr,"%d.%d",nb1Beta/10,nb1Beta%10); displayStr(valStr); }
    if(se==5)
      { sprintf(valStr,"0.%03d",nb1GainFloor); displayStr(valStr); }
  }

void sendNB1Params(void)
  {
    char cmd[20];
    sprintf(cmd,"e%d",nb1Algorithm);  sendFifo(cmd);
    sprintf(cmd,"j%d",nb1FftSize);    sendFifo(cmd);
    sprintf(cmd,"p%d",nb1Overlap);    sendFifo(cmd);
    sprintf(cmd,"x%d",nb1Alpha);      sendFifo(cmd);
    sprintf(cmd,"y%d",nb1Beta);       sendFifo(cmd);
    sprintf(cmd,"z%d",nb1GainFloor);  sendFifo(cmd);
    sendFifo("N2");    // apply: rebuild chain once if NB1 active
  }

void displaySetting(int se)
{
  char valStr[30];
  gotoXY(0,settingY);
  textSize=2;                                                            
  setForeColour(255,255,255);
  displayStr("                                                ");
  gotoXY(0,settingY+8);
  displayStr("                                                ");
  gotoXY(settingX,settingY);
  if((se==TX_OFFSET)||(se==RX_OFFSET)||(se==CWID))
  {
  gotoXY(0,settingY);
  }
  displayStr(settingText[se]);
 
 if(se==SSB_MIC)
  {
  sprintf(valStr,"%d",SSBMic);
  displayStr(valStr);
  }
 if(se==FM_MIC)
  {
  sprintf(valStr,"%d",FMMic);
  displayStr(valStr);
  }
 if(se==AM_MIC)
  {
  sprintf(valStr,"%d",AMMic);
  displayStr(valStr);
  } 
if(se==REP_SHIFT)
  {
  sprintf(valStr,"%.5f",bandRepShift[band]);
  displayStr(valStr);
  }
if(se==CTCSS)
  {
  sprintf(valStr,"%.1f Hz",CTCSSTone[bandCTCSS[band]]/10.0);
  displayStr(valStr);
  }
  if(se==RX_OFFSET)
  {
  sprintf(valStr,"%.5f",bandRxOffset[band]);
  displayStr(valStr);
  displayStr(" Rx Freq= ");
  sprintf(valStr,"%.5f",freq+bandRxOffset[band]);
  displayStr(valStr);
  }
  if(se==RX_HARMONIC)
  {
  sprintf(valStr,"X%d",bandRxHarmonic[band]);
  displayStr(valStr);
  } 
  if(se==TX_OFFSET)
  {
  sprintf(valStr,"%.5f",bandTxOffset[band]);
  displayStr(valStr);
  displayStr(" Tx Freq= ");
  sprintf(valStr,"%.5f",freq+bandTxOffset[band]);
  displayStr(valStr);
  }
  if(se==TX_HARMONIC)
  {
  sprintf(valStr,"X%d",bandTxHarmonic[band]);
  displayStr(valStr);
  }   
  if(se==BAND_BITS_RX)
  { 
  maxSetIndex=7;
  setForeColour(255,255,255);
  for(int b=128;b>0;b=b>>1)
    {
      if(((setIndex==7)&&(b==1)) || ((setIndex==6)&&(b==2))  || ((setIndex==5)&&(b==4)) || ((setIndex==4)&&(b==8)) || ((setIndex==3)&&(b==16))  || ((setIndex==2)&&(b==32))  || ((setIndex==1)&&(b==64))  || ((setIndex==0)&&(b==128)))
        {
          setForeColour(0,255,0);
        }
      else
        {
          setForeColour(255,255,255);
        }
      if(bandBitsRx[band] & b)
        {
        displayChar('1');
        }
      else
        {
        displayChar('0');
        }
    } 
  }
if(se==BAND_BITS_TX)
  { 
  maxSetIndex=7;
  setForeColour(255,255,255);
  for(int b=128;b>0;b=b>>1)
    {
      if(((setIndex==7)&&(b==1)) || ((setIndex==6)&&(b==2))  || ((setIndex==5)&&(b==4)) || ((setIndex==4)&&(b==8)) || ((setIndex==3)&&(b==16))  || ((setIndex==2)&&(b==32))  || ((setIndex==1)&&(b==64))  || ((setIndex==0)&&(b==128)))
        {
          setForeColour(0,255,0);
        }
      else
        {
          setForeColour(255,255,255);
        }
      if(bandBitsTx[band] & b)
        {
        displayChar('1');
        }
      else
        {
        displayChar('0');
        }
    } 
  }
  if(se==BAND_BITS_TO_PLUTO)
  {
    if(bandBitsToPluto==1)
      {
      sprintf(valStr,"Yes");
      }
    else
      {
      sprintf(valStr,"No");
      }
   displayStr(valStr);  
  } 
  if(se==FFT_REF)
  {
  sprintf(valStr,"%d dB",FFTRef);
  displayStr(valStr);
  }
  if(se==FFT_RANGE)
  {
  sprintf(valStr,"%d dB",FFTRange);
  displayStr(valStr);
  }
  if(se==TX_ATT)
  {
  sprintf(valStr,"%d dB",TxAtt);
  displayStr(valStr);
  }
  if(se==RX_GAIN)
  {
    if(bandRxGain[band]>maxGain(freq))
    {
    sprintf(valStr,"Auto");
    }
    else
    {
    sprintf(valStr,"%d dB",bandRxGain[band]);
    }
  displayStr(valStr);
  }
  if(se==S_ZERO)
  {
  sprintf(valStr,"%.0f dB",bandSmeterZero[band]);
  displayStr(valStr);
  }
  if(se==SSB_FILT_LOW)
  {
  sprintf(valStr,"%d Hz",bandSSBFiltLow[band]);
  displayStr(valStr);
  }
  if(se==SSB_FILT_HIGH)
  {
  sprintf(valStr,"%d Hz",bandSSBFiltHigh[band]);
  displayStr(valStr);
  }
  if(se==CW_CARRIER)
  {
  sprintf(valStr,"%d Secs",CWIDkeyDownTime/100);
  displayStr(valStr);
  }
  if(se==CWID)
  {
    maxSetIndex=MORSEIDENTLENGTH-2;
   for(int c=0; c<MORSEIDENTLENGTH;c++)
    {
      if(setIndex==c)
        {
          setForeColour(0,255,0);
        }
      else
        {
          setForeColour(255,255,255);
        }
      if(morseIdent[c]>0)
        {
          if((morseIdent[c]==95) && (setIndex!=c))
            {
              displayChar(32);
            }
          else
            {
              displayChar(morseIdent[c]);
            }
        } 
      else
        {
          if(setIndex>=c)
            {
            setIndex=c;
            morseIdent[c]=95;
            setForeColour(0,255,0);
            displayChar(95);
            morseIdent[c+1]=0;          
            }
          break;
        }   
    }
  }
  
 if(se==BREAK_IN_TIME)
  {
  sprintf(valStr,"%d ms",breakInTime*10);
  displayStr(valStr);
  }  
  
if(se==BANDS24)
  {
    if(bands24 == 0)
    {
      displayStr("No");
    }
    else
    {
        displayStr("Yes");
    }
  }

if(se==ENABLE_GPIO_PTT)
  {
    displayStr(enableGPIOPTT ? "Enabled" : "Disabled");
  }

if(se==ENABLE_CW_KEY)
  {
    displayStr(enableCWKey ? "Enabled" : "Disabled");
  }

if(se==ENABLE_PLUTO_TX)
  {
    displayStr(enablePlutoTx ? "Enabled" : "Disabled");
  }

if(se==ALSA_CAPTURE)
  {
    sprintf(valStr,"%d %%",alsaCaptureLevel);
    displayStr(valStr);
  }

 if(se==ROTATE)
  {
    if(screenrotate == 0)
    {
      displayStr("No");
    }
    else
    {
        displayStr("Yes");
    }
  }
}

int readConfig(void)
{
FILE * conffile;
char variable[50];
char value[100];
char vname[20];

conffile = openFile("Langstone/Langstone_Pluto.conf", "r");

if(conffile==NULL)
  {
    return -1;
  }

while(fscanf(conffile,"%49s %99s [^\n]\n",variable,value) !=EOF)
  {
     
    if(strstr(variable,"CW_IDENT"))
      {
       value[MORSEIDENTLENGTH-1]=0;           //force to maximum length if necessary
       strcpy(morseIdent,value);
      }
    if(strstr(variable,"CWID_KEY_DOWN_TIME"))
      {
        sscanf(value,"%d",&CWIDkeyDownTime);
        CWIDkeyDownTime=CWIDkeyDownTime*100;
      }

    
    for(int b=0;b<numband;b++)
    {
    sprintf(vname,"bandFreq%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%lf",&bandFreq[b]);
    sprintf(vname,"bandMode%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandMode[b]);   
    sprintf(vname,"bandTxOffSet%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%lf",&bandTxOffset[b]); 
    sprintf(vname,"bandTxHarmonic%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandTxHarmonic[b]);             
    sprintf(vname,"bandRxOffSet%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%lf",&bandRxOffset[b]);      
    sprintf(vname,"bandRxHarmonic%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandRxHarmonic[b]);   
    sprintf(vname,"bandRepShift%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%lf",&bandRepShift[b]);
    sprintf(vname,"bandCTCSS%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandCTCSS[b]);
    sprintf(vname,"bandDuplex%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandDuplex[b]);  
 
 //handle old config file format by converting bandbits to bandbitsRx and BandbitsTx   
    sprintf(vname,"bandBits%02d",b);
    if(strstr(variable,vname)) 
    {
    sscanf(value,"%d",&bandBitsRx[b]); 
    sscanf(value,"%d",&bandBitsTx[b]);
    }
       
    sprintf(vname,"bandRxBits%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandBitsRx[b]); 
    sprintf(vname,"bandTxBits%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandBitsTx[b]);    
    sprintf(vname,"bandFFTRef%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandFFTRef[b]);
    sprintf(vname,"bandFFTRange%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandFFTRange[b]);     
    sprintf(vname,"bandSquelchUSB%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandSquelch[b][USB]);
    sprintf(vname,"bandSquelchLSB%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandSquelch[b][LSB]);
    sprintf(vname,"bandSquelchCW%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandSquelch[b][CW]);
    sprintf(vname,"bandSquelchCWN%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandSquelch[b][CWN]);
    sprintf(vname,"bandSquelchFM%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandSquelch[b][FM]);
    sprintf(vname,"bandSquelchAM%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandSquelch[b][AM]);     
    sprintf(vname,"bandTxAtt%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandTxAtt[b]);
    sprintf(vname,"bandRxGain%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandRxGain[b]);    
    sprintf(vname,"bandSmeterZero%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%f",&bandSmeterZero[b]);
    sprintf(vname,"bandSSBFiltLow%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandSSBFiltLow[b]); 
    sprintf(vname,"bandSSBFiltHigh%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandSSBFiltHigh[b]); 
    sprintf(vname,"bandFFTBW%02d",b);
    if(strstr(variable,vname)) sscanf(value,"%d",&bandFFTBW[b]);    
    }

    
    if(strstr(variable,"currentBand")) sscanf(value,"%d",&band);
    if(strstr(variable,"tuneDigit")) sscanf(value,"%d",&tuneDigit);   
    if(strstr(variable,"mode")) sscanf(value,"%d",&mode);
    if(strstr(variable,"SSBMic")) sscanf(value,"%d",&SSBMic);
    if(strstr(variable,"FMMic")) sscanf(value,"%d",&FMMic);
    if(strstr(variable,"AMMic")) sscanf(value,"%d",&AMMic);
    if(strstr(variable,"volume")) sscanf(value,"%d",&volume);
    if(strstr(variable,"breakInTime")) sscanf(value,"%d",&breakInTime);
    if(strstr(variable,"bandBitsToPluto")) sscanf(value,"%d",&bandBitsToPluto);
    if(strstr(variable,"bands24")) sscanf(value,"%d",&bands24);
    if(strstr(variable,"RotateScreen")) sscanf(value,"%d",&screenrotate);
    if(strstr(variable,"enableGPIOPTT")) sscanf(value,"%d",&enableGPIOPTT);
    if(strstr(variable,"enableCWKey")) sscanf(value,"%d",&enableCWKey);
    if(strstr(variable,"enablePlutoTx")) sscanf(value,"%d",&enablePlutoTx);
    if(strstr(variable,"nb1Algorithm")) sscanf(value,"%d",&nb1Algorithm);
    if(strstr(variable,"nb1FftSize"))   sscanf(value,"%d",&nb1FftSize);
    if(strstr(variable,"nb1Overlap"))   sscanf(value,"%d",&nb1Overlap);
    if(strstr(variable,"nb1Alpha"))     sscanf(value,"%d",&nb1Alpha);
    if(strstr(variable,"nb1Beta"))      sscanf(value,"%d",&nb1Beta);
    if(strstr(variable,"nb1GainFloor")) sscanf(value,"%d",&nb1GainFloor);
    if(strstr(variable,"compAgcAttack"))  sscanf(value,"%d",&compAgcAttack);
    if(strstr(variable,"compAgcDecay"))   sscanf(value,"%d",&compAgcDecay);
    if(strstr(variable,"compAgcRef"))     sscanf(value,"%d",&compAgcRef);
    if(strstr(variable,"compAgcMax"))     sscanf(value,"%d",&compAgcMax);
    if(strstr(variable,"compLpfCutoff"))  sscanf(value,"%d",&compLpfCutoff);
    if(strstr(variable,"compEq1Freq"))    sscanf(value,"%d",&compEq1Freq);
    if(strstr(variable,"compEq1Gain"))    sscanf(value,"%d",&compEq1Gain);
    if(strstr(variable,"compEq2Freq"))    sscanf(value,"%d",&compEq2Freq);
    if(strstr(variable,"compEq2Gain"))    sscanf(value,"%d",&compEq2Gain);
    if(strstr(variable,"compEq3Freq"))    sscanf(value,"%d",&compEq3Freq);
    if(strstr(variable,"compEq3Gain"))    sscanf(value,"%d",&compEq3Gain);
    if(strstr(variable,"compMicGain"))    sscanf(value,"%d",&compMicGain);
    if(strstr(variable,"alsaCaptureLevel")) sscanf(value,"%d",&alsaCaptureLevel);
    if(mode>nummode-1) mode=0;
            
  }

fclose(conffile);
return 0;

}

int writeConfig(void)
{
FILE * conffile;
char variable[80];
int value;

bandFreq[band]=freq;

for(int i=0;i<MORSEIDENTLENGTH;i++)                                                 //trim morse Ident to remove redundant spaces.
  {
  if((morseIdent[i]==95) && (morseIdent[i+1]==95))                    //find double space
    {
      morseIdent[i]=0;                                                //terminate string here
      break;
    }
  }


conffile = openFile("Langstone/Langstone_Pluto.conf", "w");

if(conffile==NULL)
  {
    return -1;
  }

fprintf(conffile,"CW_IDENT %s\n",morseIdent);
fprintf(conffile,"CWID_KEY_DOWN_TIME %d\n",CWIDkeyDownTime/100);

for(int b=0;b<numband;b++)
{
  fprintf(conffile,"bandFreq%02d %lf\n",b,bandFreq[b]);
  fprintf(conffile,"bandMode%02d %d\n",b,bandMode[b]);
  fprintf(conffile,"bandTxOffSet%02d %lf\n",b,bandTxOffset[b]);
  fprintf(conffile,"bandTxHarmonic%02d %d\n",b,bandTxHarmonic[b]);
  fprintf(conffile,"bandRxOffSet%02d %lf\n",b,bandRxOffset[b]);
  fprintf(conffile,"bandRxHarmonic%02d %d\n",b,bandRxHarmonic[b]);
  fprintf(conffile,"bandRepShift%02d %lf\n",b,bandRepShift[b]);
  fprintf(conffile,"bandCTCSS%02d %d\n",b,bandCTCSS[b]);
  fprintf(conffile,"bandDuplex%02d %d\n",b,bandDuplex[b]);
  fprintf(conffile,"bandRxBits%02d %d\n",b,bandBitsRx[b]);
  fprintf(conffile,"bandTxBits%02d %d\n",b,bandBitsTx[b]);
  fprintf(conffile,"bandFFTRef%02d %d\n",b,bandFFTRef[b]);
  fprintf(conffile,"bandFFTRange%02d %d\n",b,bandFFTRange[b]);
  fprintf(conffile,"bandSquelchUSB%02d %d\n",b,bandSquelch[b][USB]);
  fprintf(conffile,"bandSquelchLSB%02d %d\n",b,bandSquelch[b][LSB]);
  fprintf(conffile,"bandSquelchCW%02d %d\n",b,bandSquelch[b][CW]);
  fprintf(conffile,"bandSquelchCWN%02d %d\n",b,bandSquelch[b][CWN]);
  fprintf(conffile,"bandSquelchFM%02d %d\n",b,bandSquelch[b][FM]);
  fprintf(conffile,"bandSquelchAM%02d %d\n",b,bandSquelch[b][AM]);
  fprintf(conffile,"bandTxAtt%02d %d\n",b,bandTxAtt[b]);
  fprintf(conffile,"bandRxGain%02d %d\n",b,bandRxGain[b]);
  fprintf(conffile,"bandSmeterZero%02d %f\n",b,bandSmeterZero[b]);
  fprintf(conffile,"bandSSBFiltLow%02d %d\n",b,bandSSBFiltLow[b]);
  fprintf(conffile,"bandSSBFiltHigh%02d %d\n",b,bandSSBFiltHigh[b]);
  fprintf(conffile,"bandFFTBW%02d %d\n",b,bandFFTBW[b]);    
}

fprintf(conffile,"currentBand %d\n",band);
fprintf(conffile,"tuneDigit %d\n",tuneDigit);
fprintf(conffile,"mode %d\n",mode);
fprintf(conffile,"SSBMic %d\n",SSBMic);
fprintf(conffile,"FMMic %d\n",FMMic);
fprintf(conffile,"AMMic %d\n",AMMic);
fprintf(conffile,"volume %d\n",volume);
fprintf(conffile,"breakInTime %d\n",breakInTime);
fprintf(conffile,"bandBitsToPluto %d\n",bandBitsToPluto);
fprintf(conffile,"bands24 %d\n",bands24);
fprintf(conffile,"RotateScreen %d\n",screenrotate);
fprintf(conffile,"enableGPIOPTT %d\n",enableGPIOPTT);
fprintf(conffile,"enableCWKey %d\n",enableCWKey);
fprintf(conffile,"enablePlutoTx %d\n",enablePlutoTx);
fprintf(conffile,"nb1Algorithm %d\n",nb1Algorithm);
fprintf(conffile,"nb1FftSize %d\n",nb1FftSize);
fprintf(conffile,"nb1Overlap %d\n",nb1Overlap);
fprintf(conffile,"nb1Alpha %d\n",nb1Alpha);
fprintf(conffile,"nb1Beta %d\n",nb1Beta);
fprintf(conffile,"nb1GainFloor %d\n",nb1GainFloor);
fprintf(conffile,"compAgcAttack %d\n",compAgcAttack);
fprintf(conffile,"compAgcDecay %d\n",compAgcDecay);
fprintf(conffile,"compAgcRef %d\n",compAgcRef);
fprintf(conffile,"compAgcMax %d\n",compAgcMax);
fprintf(conffile,"compLpfCutoff %d\n",compLpfCutoff);
fprintf(conffile,"compEq1Freq %d\n",compEq1Freq);
fprintf(conffile,"compEq1Gain %d\n",compEq1Gain);
fprintf(conffile,"compEq2Freq %d\n",compEq2Freq);
fprintf(conffile,"compEq2Gain %d\n",compEq2Gain);
fprintf(conffile,"compEq3Freq %d\n",compEq3Freq);
fprintf(conffile,"compEq3Gain %d\n",compEq3Gain);
fprintf(conffile,"compMicGain %d\n",compMicGain);
fprintf(conffile,"alsaCaptureLevel %d\n",alsaCaptureLevel);

fclose(conffile);
return 0;

}


void setAlsaCapture(int level)
{
   char cmd[100];
   sprintf(cmd, "amixer -c 1 cset numid=4 %d%% > /dev/null 2>&1", level);
   system(cmd);
}

void startGNURadio(void)
{
   FFTTimeout = FFTTIMEOUT * 10;                               //allow time to start (20 seconds)
   if(system("ps -ax | grep -v grep | grep -q Lang_TRX_Pluto.py") == 0)
   {
      return;
   }
   system("python $HOME/Langstone/Lang_TRX_Pluto.py > /tmp/LangstoneTRX_Pluto.log 2>&1 &");
}

void restartGNURadio(void)
{
   setBandBits(0);
   sendFifo("H0");        //unlock the flowgraph so that it can exit
   sendFifo("Q");       //kill the SDR
   FFTTimeout = FFTTIMEOUT * 5;                                //allow time to restart
   displayError("Restarting GNURadio");
   sleep(2);
   startGNURadio();
   sleep(2);
   initSDR();
   sendNB1Params();
   if(nb1Active) sendFifo("N1");
   sendCOMPParams();
   if(compActive) sendFifo("c1");
}

FILE *openFile(char *fn, char *mode)
{
  char filename[MAX_FILENAME_LENGTH] = {0};
  int fnlen = 0;

  /* find user path i.e. /home/pi */
  char *userpath = getenv("HOME");
  if (userpath == NULL)
  {
    printf("Can read HOME env variable");      // TODO: Move to separate logfile
    return NULL;
  }

  /* allocate space for filename */
  fnlen = snprintf(NULL, 0, "%s/%s", userpath, fn);
  if (fnlen > MAX_FILENAME_LENGTH)
  {
    printf("Filename greater than %i characters\n", MAX_FILENAME_LENGTH);       // TODO: Move to separate logfile
    return NULL;
  }

  memset(filename, 0, MAX_FILENAME_LENGTH);       // trust no one

  /* put it all together */
  int retval = snprintf(filename, MAX_FILENAME_LENGTH, "%s/%s", userpath, fn);
  if (retval != fnlen)
  {
    printf("Can't generate filename correctly, ignored\n");     // TODO: Move to separate logfile
    return NULL;
  }

  /* try to get file handle */
  return fopen(filename, mode);
}