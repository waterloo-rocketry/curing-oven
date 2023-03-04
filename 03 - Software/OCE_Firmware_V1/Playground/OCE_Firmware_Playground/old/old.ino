// OCE Firmware V1
// Calvin DeKoter
// Power Ranchers, Group 41
// Code is designed to run on the Teensy 4.1, with a custom PCB and all the necessary hardware

// Stuff to add

// Things to fix

  // blinking state light doesn't transition properly - maybe consequence of analogwrite, maybe being backfed by 5V rail
  // need to machine vacuum sensor pickup
  // need to add code for pitot tube
  // need to add thermal fuse to the IL, need to look at adding panel opening detection to the IL

// Calibration routine for the user analog inputs with variable gain - use the user analog outputs
// Mode for datalogging/aux only - from control menu

// Used Libraries

#include <LiquidCrystal_Wire2_I2C.h> // need to switch the header in the library to use Wire2 - (library in ZIP file, move to general install location)
#include <TimeLib.h>
#include <IntervalTimer.h> // Library to create timer interrupt objects
#include <NativeEthernet.h> // Library to post data to the internet

#include <SD.h>
#include <SPI.h>

const int chipSelect = BUILTIN_SDCARD ;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0x04, 0xe9, 0xe5, 0x10, 0xbc, 0x26
};

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

// Teensy CAN

// Function Prototypes

void pushOUT() ;
void pushUPW() ;
void pushDWN() ;
void pushSLT() ;
void pushRUN() ;

void ILBRK() ;

void driveHTR1() ;

float interpTherm(int dataRead) ;

// Pin Assignments

const int AIN_c = 16 ; 
// number of analog input pins
int AIN[] = {40,41,14,15,16,17,18,19,20,21,22,23,38,39,27,26} ; // 24,25 for SCA/SCL
// physical pin numbers of each of the analog input pins
bool AIN_u[] = {true,true,true,true,true,true,true,true,true,false,false,false,false,false,false,false} ; // Array to select whether or not to collect data from the analog input
bool AIN_avg[] = {true,true,true,false,false,false,true,true,true,false,false,false,false,false,false,false}; // Array to select whether or not to include the thermistors in the oven temperarature average - note that the last 4 are always false, as they are not temperatures
// If the pin is in use, mark it to be read during the cure and cool states
const char *AIN_n[] = {"TH01","TH02","TH03","TH04","TH05","TH06","TH07","TH08","TH09","TH10","TH11","TH12","UA1","UA2","VS1","PIT1"} ;
// Names of all the analog input pins

const int mapTherm[2][102] = {{993,985,975,962,948,930,911,888,862,834,803,770,734,697,658,619,580,541,503,466,430,396,364,334,306,280,256,234,214,195,178,163,149,136,125,115,105,97,89,82,75,70,64,59,55,51,47,44,41,38,36},{0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,130,135,140,145,150,155,160,165,170,175,180,185,190,195,200,205,210,215,220,225,230,235,240,245,250}} ;
                              // 10k Thermistor Divider {{786,736,682,626,568,512,457,405,356,312,273,238,207,180,156,136,118,103,90,78,69,60,53,47,41,37,33,29,26,23,21,19,17,15,14,12,11,10,9,8,8,7,6,6,5,5,4,4,4,3,3},{0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,130,135,140,145,150,155,160,165,170,175,180,185,190,195,200,205,210,215,220,225,230,235,240,245,250}};



int AOUT[] =  {29,28} ;
const char *AOUT_n[] = {"UA3","UA4"} ;

int SW_c = 5 ;
int SW[] = {3,4,5,6,7} ;
const char *SW_n[] = {"OUT","UPW","DWN","SLT","RUN"} ;

IntervalTimer control_HTR1_Timer ;
IntervalTimer blinkLED ;

int DRV_c = 7 ;
int DRV[] = {11,13,12,32,8,9,10}; // Input Relay, Blower, Heater, Auxiliary, and LED Enable Pins 
const char *DRV_n[] = {"IRO1","BLW1","HTR1","AUX1","LED1","LED2","LED3"};

int state = 1 ; // Start in the startup state
const char *state_n[] = {"OFF","STARTUP","CURE","COOL","ERROR"} ;

int SEN_IL = 1 ;
int UA3 = 29 ;
int UA4 = 28 ;

int error[5] = {0,0,0,0,0} ;
int error_c = 5 ;
char *error_n[] = {"","IL BRK","TH BRK","TH OVT","VAC ERR","PGM NOK"} ; // Array to hold the error strings
int error_count = 0 ;

char clearline = "                    " ;

bool stateChange = false ;
bool ILOK = false ;     // Boolean to indicate the state of the interlock loop
bool PGMOK = false ;  // Boolean to indicate the validity of the parameters (all positive, nonzero, temp less than Temp_Max_Part)
bool CURE = false ;
bool COOL = false ;
bool ERR = false ;
bool AUX = false ; // Bool to store the state of the auxiliary device

volatile bool OUT = false ;
volatile bool UPW = false ;
volatile bool DWN = false ;
volatile bool SLT = false ;
volatile bool RUN = false ;
volatile bool blinkState = false ;

// Oven Properties

int Temp_Max = 150 ; // Degrees C, immediate shutdown if exceeded by all but two thermistors in the average.
int Temp_Max_Control = 140 ; // Maximum temperature that the oven will deliberately heat to
int Temp_Max_Part = 130 ; // Maximum part temperature
int Temp_Min = 10 ; // Indicates an open thermistor circuit, does not necessarily cause a shutdown unless all but two circuits are open

int Time_heat = 5*60*1000 ; // Heating ramp time in milliseconds
int Time_hold = 0.5*3600*1000 ; // Hold time in milliseconds
int Time_cool = 45*60*1000 ; // Cool time in milliseconds

int Temp_set = 90 ; // Degrees C
float Temp_avg = 0 ; // Average Temperature

int vacc_zero = 800 ;
int vacc_low = 0 ; // change me to 10 or 20 inHg for actual use

float UA1_gain = 1 ; // Gain for the two user inputs
float UA2_gain = 1 ; 

LiquidCrystal_Wire2_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// Controller Properties

float Kp[] = {0.5,1,5} ; // Will be challenging to find a good set of gains for every circumstance - will consider gain scheduling or a self-calibration sequence - size two works really well @240V for the unit cell and 90 C
float Kd[] = {0.01,0.1,0.5} ;
float Ki[] = {0.1,0.1,0.5} ;
int K_sz = 0 ; // Oven Size

volatile int cycle = 1 ;
volatile int control_HTR1_count = 0 ;
volatile int control_HTR1 = 0 ; // Range of time HTR1 is enabled, over 1 second - 0 - 59 out of 60 electrical cycles

// Data Structures
const int data_width = 1 + 1 + AIN_c + 1 ; // Time plus Temp_set plus analog plus control setpoint

float data[data_width] ;  

bool DataSave = 1 ;
bool DataUpload = 0 ;
char filename[] = "oven1.txt" ;

void setup() {

  Serial.begin(9600) ;

  lcd.init();
  lcd.backlight();

  lcd.clear();

  delay(200) ;

  for(int i=0;i<AIN_c; i++) // Assign the analog input pins to the analog mode
  {
    pinMode(AIN[i],INPUT) ;
  }


  for(int j=0;j<SW_c;j++) // Assign the switch inputs to input pullup mode
  {
    pinMode(SW[j],INPUT_PULLUP) ;
  }

  attachInterrupt(digitalPinToInterrupt(SW[0]), pushOUT, FALLING) ;
  attachInterrupt(digitalPinToInterrupt(SW[1]), pushUPW, FALLING) ;
  attachInterrupt(digitalPinToInterrupt(SW[2]), pushDWN, FALLING) ;
  attachInterrupt(digitalPinToInterrupt(SW[3]), pushSLT, FALLING) ;
  attachInterrupt(digitalPinToInterrupt(SW[4]), pushRUN, FALLING) ;
  
  for(int k=0;k<DRV_c; k++) // Assign the drive output to output mode
  {
    pinMode(DRV[k],OUTPUT) ;
  }

  pinMode(SEN_IL,INPUT_PULLDOWN) ;
  attachInterrupt(digitalPinToInterrupt(SEN_IL), ILBRK, FALLING) ;
  
  pinMode(UA3,OUTPUT) ;
  pinMode(UA4,OUTPUT) ;

  if(digitalRead(SEN_IL))
  {ILOK = true ;}

  if(DataSave) // DataSave above needs to be set to true if you want to be able to save data - you can always turn it off from the OCE menu
  {
    // see if the card is present and can be initialized:
    if (!SD.begin(chipSelect)) {
      Serial.println("Card failed, or not present");
      lcd.clear();
      delay(100) ;
      lcd.setCursor(3,1) ;
      lcd.print("NO SD Card") ;
      lcd.setCursor(0,2) ;
      lcd.print("RUN to Proceed") ;
      while (!RUN) {
        // No SD card, so don't do anything more - stay stuck here
      }
      delay(1000) ;
      RUN = false ;
      Serial.println("No SD Card") ;
      lcd.clear() ;
      lcd.setCursor(3,1) ;
      lcd.print("No Data Saving!") ;
    }
    Serial.println("card initialized.");
  
      // open the file.
    int file_i = 1 ;
      
   /* while(SD.exists(filename))
      {
        file_i =+ 1 ;
        filename = "oven".concat(file_i).concat(".txt") ;
      } 
    filename = filename.concat(file_i).concat(".txt") ; */
    
    File dataFile = SD.open(filename, FILE_WRITE);
  
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.println("Power Ranchers Modular Curing Oven");
      dataFile.close();
    } else {
      // if the file isn't open, pop up an error:
      Serial.println("error opening oven data file");
    }
  }

  // Setup Ethernet Connection
  if(DataUpload) //  DATA UPLOAD MUST BE SET TO 1 IN GLOBAL VARIABLES TO RUN THE ETHERNET PROCEDURE
  {
    Ethernet.begin(mac);
    
      // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
  
    // start the server
    server.begin();
    Serial.print("server is at ");
    Serial.println(Ethernet.localIP());
  }
}

void loop() {
  
  if(state==1 && !stateChange) // Enter the STARTUP State
  {

    Serial.println("Welcome to the Startup State") ;
    if(!ILOK)
    {
      lcd.setCursor(1,1) ;
      lcd.print("IL LOOP OPEN") ;
      state = 1 ;
    }

    if(digitalRead(SEN_IL))
      {ILOK = true ;}

    int curs = 0 ; // High level menu cursor
    bool ENT = false ; // Bools to enter or exit the menus
    bool EXT = false ; 
      
    while(ILOK && !RUN)
    {
      // Menu for setting all parameters
      lcd.setCursor(0,0) ;
      lcd.print("04MAIN MENU  ") ;
      lcd.print("CURE") ;
      lcd.setCursor(12,1) ;
      lcd.print(" SENSE") ;
      lcd.setCursor(12,2) ;
      lcd.print(" DATA") ;
      lcd.setCursor(12,3) ;
      lcd.print(" CTRL") ;

      lcd.setCursor(0,3) ; // Write the state name
      lcd.print(state_n[state]) ; 

      lcd.setCursor(11,curs) ;
      lcd.print(">") ;

      int curs_cxt = 0 ; // Context Menu Cursor

      if(UPW)
      {
        if(curs!=0)
          {curs -= 1 ;}
        else{curs = 3;}
        delay(50) ;
        UPW = false ;
      }
      if(DWN)
      {
        if(curs<3)
          {curs += 1 ;}
        else{curs = 0;}
        delay(50) ;
        DWN = false ;
      }   
      
      while(curs == 0 && !OUT && SLT)
      {
        SLT = false ;
        OUT = false ;
        Serial.println("Welcome to the Cure Menu") ;
        lcd.clear() ;
        // Cure Menu
        lcd.setCursor(0,0) ;
        lcd.print("Temp Targ: ") ;
        lcd.print(Temp_set) ;
        lcd.setCursor(0,1) ;
        lcd.print("Time Heat: ") ;
        lcd.print(Time_heat/(60*1000)) ;
        lcd.setCursor(16,0) ;
        lcd.print("[C]") ;
        lcd.setCursor(14,1) ;
        lcd.print("[min]") ;
        lcd.setCursor(0,2) ;
        lcd.print("Time Hold: ") ;
        lcd.print(Time_hold/(3600*1000)) ;
        lcd.setCursor(16,2) ;
        lcd.print("[h]") ;
        lcd.setCursor(0,3) ;
        lcd.print("Time Cool: ") ;
        lcd.print(Time_cool/(60*1000)) ;
        lcd.setCursor(14,3) ;
        lcd.print("[min]")                 ;

        lcd.setCursor(19,curs_cxt) ;
        lcd.print("<") ;
        
        if(UPW)
        {
         if(curs_cxt!=0)
         {curs_cxt -= 1 ;}
         else{curs_cxt = 3;}
          delay(50) ;
          UPW = false ;
        }
        if(DWN)
        {
          if(curs_cxt<3)
          {curs_cxt += 1 ;}
          else{curs_cxt = 0;}
          delay(50) ;
          DWN = false ;
        }

        delay(500) ;

        while(SLT && !OUT)
        {
          if(UPW) // If the up button has been pressed, increment the values
          {
            if(curs_cxt==0)
            {
              Temp_set += 2;
              lcd.setCursor(11,curs_cxt) ;
              lcd.print(Temp_set) ;
            }
            else if(curs_cxt==1)
            {
              Time_heat += 1000*60*5 ;
              lcd.setCursor(11,curs_cxt) ;
              lcd.print(Time_heat/(60*1000)) ;
            }
            else if(curs_cxt == 2)
            {
              Time_hold += 1000*60*5 ;
              lcd.setCursor(11,curs_cxt) ;
              lcd.print(Time_hold/(3600*1000)) ;
            }
            else if(curs_cxt == 3)
            {
              Time_cool += 1000*60*5 ;
             lcd.setCursor(11,curs_cxt) ;
             lcd.print(Time_cool/(60*1000)) ;
            }

            delay(250) ;
            UPW = false ;  
          }
          else if(DWN) // if the down button has been pressed, decrement the value
          {
            if(curs_cxt==0)
            {
              Temp_set -= 2;
              lcd.setCursor(11,curs_cxt) ;
              lcd.print(Temp_set) ;
            }
            else if(curs_cxt==1)
            {
              Time_heat -= 1000*60*5 ;
              lcd.setCursor(11,curs_cxt) ;
              lcd.print(Time_heat/(60*1000)) ;
            }
            else if(curs_cxt == 2)
            {
              Time_hold -= 1000*60*30 ;
              lcd.setCursor(11,curs_cxt) ;
              lcd.print(Time_hold/(3600*1000)) ;
            }
            else if(curs_cxt == 3)
            {
              Time_cool -= 1000*60*5 ;
              lcd.setCursor(11,curs_cxt) ;
              lcd.print(Time_cool/(60*1000)) ;  
            }

            delay(250) ;
            DWN = false ;  
          }
          
           // Push the OUT button to exit the while loop
        }

        
        SLT = true ;
        
      }
        
      
      while(curs == 1 && SLT && !OUT)
      {
        // Sense Menu 
        Serial.println("Welcome to the Sense Menu") ; 
        lcd.clear() ;

        lcd.setCursor(0,0) ;
        lcd.print("TH SLT: ") ;
        
        for(int t = 0;t<12;++t)
        {
          if(AIN_u[t])
          {lcd.print("1");}
          else{lcd.print("0");}
        }
        
        // TODO make a horizontal cursor to turn sensors on and off
        
        delay(500) ;
      }
      
      while(curs == 2 && SLT && !OUT) // Complete
      {
        // Data Menu
        Serial.println("Welcome to the Data Menu") ;
        Serial.println(DataSave) ;
        Serial.println(DataUpload) ;
        lcd.clear() ;
        
        lcd.setCursor(0,0) ;
        lcd.print("Data Saving: ") ;
        if(DataSave)
          {lcd.print("Yes");}
        else{lcd.print("No ");}
        lcd.setCursor(0,1) ;
        lcd.print("Data Upload: ") ;
        if(DataUpload)
          {lcd.print("Yes");}   
        else{lcd.print("No ");} 

        if(UPW)
        {
         if(curs_cxt!=0)
         {curs_cxt -= 1 ;}
         else{curs_cxt = 3;}
          delay(50) ;
          UPW = false ;
        }
        if(DWN)
        {
          if(curs_cxt<3)
          {curs_cxt += 1 ;}
          else{curs_cxt = 0;}
          delay(50) ;
          DWN = false ;
        }

        lcd.setCursor(19,curs_cxt) ;
        lcd.print("<") ;

        SLT = false ;
        delay(500) ;
        while(SLT && !OUT)
        {
          if(UPW)
          {
            if(curs_cxt==0)
            {DataSave = true ;
            lcd.setCursor(13,0) ;
            lcd.print("Yes");}
            else if(curs_cxt==1)
            {DataUpload = true ;
             lcd.setCursor(13,1) ;
             lcd.print("Yes");}
             UPW = false ;
           }
         
          else if(DWN)
          {
            if(curs_cxt==0)
              {DataSave = false ;
              lcd.setCursor(13,0) ;
              lcd.print("No ");}
            else if(curs_cxt == 1)
              {DataUpload = false ;
              lcd.setCursor(13,1) ;
              lcd.print("No ");}
              DWN = false ;
          }
         delay(500) ;
        }
        SLT = true ;
        
        
      }
      
      while(curs == 3 && SLT && !OUT)
      {
        // Controller Menu
        
        SLT = false ;
        delay(500) ;
        lcd.clear() ;
        
        Serial.println("Welcome to the Controller Menu") ;
        

        lcd.setCursor(0,0) ;
        lcd.print("Oven Size: ") ;
        lcd.print(K_sz+1) ; 
        lcd.setCursor(0,1) ;
        lcd.print("P Gain: ") ; // Print out the controller gains
        lcd.print(Kp[K_sz]) ;
        lcd.setCursor(0,2) ;
        lcd.print("I Gain: ") ;
        lcd.print(Ki[K_sz]) ;
        lcd.setCursor(0,3) ;
        lcd.print("D Gain: ") ;
        lcd.print(Kd[K_sz]) ;
        

        lcd.setCursor(19,curs_cxt) ;
        lcd.print("<") ;

        if(UPW)
        {
         if(curs_cxt!=0)
         {curs_cxt -= 1 ;}
         else{curs_cxt = 3;}
          delay(50) ;
          UPW = false ;
        }
        if(DWN)
        {
          if(curs_cxt<3)
          {curs_cxt += 1 ;}
          else{curs_cxt = 0;}
          delay(50) ;
          DWN = false ;
        }
        
        while(SLT && !OUT)
        {

          if(curs_cxt==0)
          {
            if(UPW&&K_sz<2)
            {
              K_sz += 1 ;
              UPW = false ;
              lcd.setCursor(11,0) ;
              lcd.print(K_sz+1) ;
            }
            else if(DWN && K_sz>0)
            {
              K_sz -= 1 ;
              DWN = false ;
              lcd.setCursor(11,0) ;
              lcd.print(K_sz+1) ;
            }
            
          }
          else if(curs_cxt == 1) // Change the proportional gain
          {
            if(UPW)
            {
              Kp[K_sz] += 0.1 ;
              UPW = false ;
            }
            else if(DWN && Kp[K_sz]>0.1)
            {
              Kp[K_sz] -= 0.1 ;
              DWN = false ;
            }
            lcd.setCursor(8,1) ;
            lcd.print(Kp[K_sz]) ;
            
          }
          else if(curs_cxt == 2) // Change the integral gain 
          {
            if(UPW)
            {
              Ki[K_sz] += 0.01 ;
              UPW = false ;
            }
            else if(DWN && Ki[K_sz]>0.01)
            {
              Ki[K_sz] -= 0.01 ;
              DWN = false ;
            }
            lcd.setCursor(8,2) ;
            lcd.print(Ki[K_sz]) ;
            
          } 
          else if(curs_cxt == 3) // Change the derivative gain
          {
            if(UPW)
            {
              Kd[K_sz] += 0.1 ;
              UPW = false ;
            }
            else if(DWN && Kd[K_sz]>0.1)
            {
              Kd[K_sz] -= 0.1 ;
              DWN = false ;
            }
            lcd.setCursor(8,3) ;
            lcd.print(Kd[K_sz]) ;
            
          }

                              
          delay(500) ;
        }

        SLT = true ;
        
      }

      curs_cxt = 0 ;
      OUT = false ;
      SLT = false ;
      delay(500) ;
      lcd.clear() ;
    }
    
    if(Temp_set<Temp_Max_Part)
    {PGMOK = true ;}
    else{PGMOK = false ;}

    if(ILOK && RUN && PGMOK)
    {
      state = 2 ; // Proceed to the cure state
      CURE = true ; 
      stateChange = true ;
    }
    else
    {
      state = 4;
      error[error_count] = 5 ;
      error_count += 1 ;
      stateChange = true ;
    }
    
  }

  if(state == 2 && !stateChange) // Enter the CURE State
  {

    Serial.println("Welcome to the Cure State") ;
    delay(1000) ;

    RUN = false ;
    
    digitalWrite(DRV[0],HIGH) ; // ULQ is active HIGH // Switch on the oven input relay
    digitalWrite(DRV[1],HIGH) ; // Switch on the blower
    digitalWrite(DRV[2],LOW) ;  // Keep the heater off
    digitalWrite(DRV[3],LOW) ;  // Keep the auxiliary off

    int Time_start = millis() ;
    float rampline = 0 ;
    int ts = 1 ; // Time scale for the gains
    float error_P = 0 ;
    float error_I = 0 ;
    float error_D = 0 ;
    float ambient = 20 ; // Guess at the first ambient temperature
    int avg_c = 0 ;

    control_HTR1_Timer.begin(driveHTR1,16666) ;
    blinkLED.begin(blinkLEDState,1000000) ;

    String DataHeader ;

    DataHeader = "Time:,TempTarg:," ;

    for(int H=0;H<AIN_c;H++)
    {
      if(AIN_u[H])
      {
        DataHeader += AIN_n[H] ;
        DataHeader += ":," ;
      }
     }
     DataHeader += "CTRL:" ;

          
     Serial.println(DataHeader) ;
     Serial.println("Min:0,Max:"+(Temp_set+20)) ;
     
    if(DataSave)
    {     
      
      File dataFile = SD.open(filename, FILE_WRITE);
     
      // if the file is available, write to it:
      if (dataFile) {
        dataFile.println(DataHeader);
        dataFile.close();
      }
      else {
        // if the file isn't open, pop up an error:
        Serial.println("error opening oven data file");
      }
    }
     
  
    while(ILOK && CURE && !ERR)
    {
      lcd.setCursor(0,3) ; // Write the state name
      lcd.print(state_n[state]) ;
      
      data[0] = now() ; // Returns the time in seconds since 1970...

      rampline = float(millis()-Time_start)/Time_heat ;
      if(rampline > 1 && millis()<(Time_start+Time_heat+Time_hold))
        {rampline = 1;}
      else if(millis()>(Time_start+Time_heat+Time_hold))
        {rampline = 1 - float(millis()-Time_start-Time_heat-Time_hold)/Time_cool ;}
      
      Temp_avg = 0 ;

     if(millis()>(Time_start+Time_heat+Time_hold+Time_cool) && blinkState)
     {
      blinkLED.end() ;
      CURE = false ;
      digitalWrite(DRV[6],HIGH) ; // Turn on the STATE LED
     }

     int therm_remaining = 0 ;

     for(int therm = 0;therm<AIN_c;therm++)
     {
      if(AIN_avg[therm]){therm_remaining += 1; }
     }
      
     for(int d = 0;d<12;d++)
      {
        if(AIN_u[d])
        {
          data[d+2] = interpTherm(analogRead(AIN[d])) ; 

          if(AIN_avg[d])
          {
            Temp_avg += data[d+2] ;
            avg_c += 1;
          }        
          
          if(data[d+2]<Temp_Min && AIN_avg[d]) // If the thermistor is undertemperature and also is included in the control average, check to see if there are two or more thermistors remaining - if so, disable the thermistor, otherwise, throw an error
          {
                                               // Note that this isn't super correct, as you just start ignoring the thermistors instead of using them to make decisions - the idea is that in a true overtemperature condition, all the thermistors will fail in this manner
            if(therm_remaining>2){AIN_avg[d] = false ;} // Remove the thermistor from the average if there are more than 2 thermistors remaining
            else
            {
              error[error_count] = 2 ; // Thermistor disconnected, move to oven ERROR state
              error_count += 1 ;
              ERR = true ;
              Serial.print("Thermistor") ;
              Serial.print(d) ;
              Serial.println(" BRK") ;
            }
          }
          if(data[d+2]>Temp_Max  && AIN_avg[d]) // If the thermistor is overtemperature and also is included in the control average, check to see if there are two or more thermistors remaining - if so, disable the thermistor, otherwise, throw an error
          {

            if(therm_remaining>2){AIN_avg[d] = false ;} // Remove the thermistor from the average if there are more than 2 thermistors remaining
            else
            {
              error[error_count] = 1 ; // Overtemperature on thermistor, move to oven ERROR state
              error_count += 3 ; 
              ERR = true ;
              Serial.print("Thermistor") ;
              Serial.print(d) ;
              Serial.println("OVT") ;
            }
          }             
        }
      }

    if(AIN_u[12])
      {data[12] = analogRead(AIN[12]) ;} // UA1
      
    if(AIN_u[13])
      {data[13] = analogRead(AIN[13]) ;} // UA2 
    
    if(AIN_u[14]) // Vaccum VS1
      {
        data[14] = (vacc_zero - analogRead(AIN[14]))*29.0/vacc_zero  ;
        Serial.print("Vacuum Level: ") ;
        Serial.print(data[14]) ;
        Serial.println(" inHg") ;


        if(data[14]<vacc_low)
        {
          ERR = true ;
          error[error_count] = 4 ;
          error_count += 1 ;
          
          Serial.println("VACC ERR") ;
          analogWrite(DRV[5],0) ; // Turn off the VOK light 
        }
        else
        {
          analogWrite(DRV[5],25) ; // Turn on the VOK light
        }
      } 
    
    if(AIN_u[15])
      {data[15] = analogRead(AIN[15]) ;} // Pitot Tube PIT1
       
    
    Temp_avg =  Temp_avg/avg_c ; // Need to decide on average vs. particular temp for control
    data[1] = (Temp_set-ambient)*rampline + ambient ; // Log the setpoint
    avg_c = 0 ;

    if(Temp_avg-data[1]>5||data[1]-Temp_avg>5) // If the temperature is within the 5 °C tolerance, turn on the "TEMP OK" light
    { digitalWrite(DRV[4],LOW)  ; }
    else
    { analogWrite(DRV[4],25) ; } 

    error_D = ((Temp_set-ambient)*rampline + ambient - Temp_avg) - error_P ;
    error_P = ((Temp_set-ambient)*rampline + ambient - Temp_avg) ;

    if(control_HTR1<58 && control_HTR1>1)
    {
      error_I += error_P*ts ;
    }  
    
    control_HTR1 = int(Kp[K_sz]*error_P + Kd[K_sz]*error_D + Ki[K_sz]*error_I) ; // First pass at control signal - basic PID loop

     if(control_HTR1>59)
     {
      control_HTR1=59 ;
     }
     else if(control_HTR1<0)
     {
      control_HTR1=0 ;
     }     

     data[data_width-1] = control_HTR1 ;
        
    lcd.setCursor(0,0) ;
    lcd.print("Temp Targ: ") ;
    lcd.print(int((Temp_set-ambient)*rampline+ambient)) ;
    //Serial.print("Temp Targ: ") ;
    //Serial.println(int((Temp_set-ambient)*rampline+ambient)) ;
    lcd.print("  C: ") ;
    lcd.print(control_HTR1) ;
    lcd.setCursor(0,1) ;
    lcd.print("Temp Avg:  ") ; 
    lcd.print(Temp_avg) ;
    lcd.setCursor(0,2) ;
    lcd.print("Time: ") ;
    lcd.print((millis()-Time_start)/(1000*60)) ;
    lcd.print(" [min] V:") ;
    lcd.print(int(data[14])) ;
    lcd.setCursor(6,3) ;
    lcd.print("PGM: ");
    lcd.print((Time_heat+Time_hold)/(60*1000)) ;
    lcd.print(" [min]") ;

      delay(900) ; // Delay to avoid logging an obscene amount of data
      cycle = 1;
      control_HTR1_count = 0 ;
      lcd.clear() ;

     
      if(millis()>(Time_heat+Time_hold+Time_cool+Time_start))
      {
        CURE = false ;
      }


  // Run the Auxiliary Device if the SLT button is pushed
  
  if(SLT && !AUX)
  {
    SLT = false ;
    digitalWrite(DRV[3],HIGH) ; // Turn on the auxiliary device
    AUX = true ;    
  }
  else if(SLT && AUX)
  {
    SLT = false ;
    digitalWrite(DRV[3],LOW) ; // Turn off the auxilary device  
    AUX = false ;
  } 
  

    // Log the Data to the SD Card

    String ovendata = data[0] ;
    ovendata += "," ;
    ovendata += data[1] ;


    for(int s = 0;s<16;s++)
      {
          if(AIN_u[s])
          {
            ovendata += "," ;
            ovendata += data[s+2] ;
          }
      }

    ovendata += "," ;
    ovendata += control_HTR1 ;

   Serial.println(ovendata) ;
    
    if(DataSave)
    {     
      
      File dataFile = SD.open(filename, FILE_WRITE);
     
      // if the file is available, write to it:
      if (dataFile) {
        dataFile.println(ovendata);
        dataFile.close();
        // print to the serial port too:
        Serial.println(ovendata);
      } else {
        // if the file isn't open, pop up an error:
        Serial.println("error opening oven data file");
      }
    }

  // listen for incoming clients
  if(DataUpload)
  {
    EthernetClient client = server.available();
    if (client) {
      Serial.println("New Client");
      // an http request ends with a blank line
      boolean currentLineIsBlank = true;
      while (client.connected()) {
        if (client.available()) {
          char c = client.read();
          Serial.write(c);
          // if you've gotten to the end of the line (received a newline
          // character) and the line is blank, the http request has ended,
          // so you can send a reply
          if (c == '\n' && currentLineIsBlank) {
            // send a standard http response header
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");  // the connection will be closed after completion of the response
            client.println("Refresh: 2");  // refresh the page automatically every 2 sec
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            
            client.print("Time,T Set,TH01,TH02,TH03,TH04,TH05,TH06,TH07,TH08,TH09,TH10,TH11,TH12,UA1,UA2,VS1,PIT1,CTRL");
            client.println("<br />");
            client.print(ovendata);
            client.println("<br />");
            client.println("</html>");
            break;
          }
          if (c == '\n') {
            // you're starting a new line
            currentLineIsBlank = true;
          } else if (c != '\r') {
            // you've gotten a character on the current line
            currentLineIsBlank = false;
          }
        }
      }
      // give the web browser time to receive the data
      delay(1);
      // close the connection:
      client.stop();
      Serial.println("client disconnected");
    }
  }
      
  } 
    blinkLED.end();
    control_HTR1_Timer.end() ;
    
    digitalWrite(DRV[0],LOW) ; // ULQ is active HIGH // Switch on the oven input relay
    digitalWrite(DRV[1],LOW) ; // Switch on the blower
    digitalWrite(DRV[2],LOW) ;  // Keep the heater off
    digitalWrite(DRV[3],LOW) ;  // Keep the auxiliary off

    digitalWrite(DRV[6],HIGH) ; // Turn on the STATE LED
    
    stateChange = true ;

    if(!ERR && ILOK){state = 1 ;} // Enter the startup state
    else{state = 4;}

  }
  
  if(state == 4 && !stateChange) // Enter the ERROR State
  // TODO for error state - implement reworked error state
  {

    lcd.clear() ;
    Serial.println("Welcome to the Error State") ;
    Serial.println(state) ;

      for(int k = 0;k<DRV_c;k++)
    {
      digitalWrite(DRV[k],LOW) ; // Shut down all the oven outputs
    }

    SLT = false ;  
    
    lcd.setCursor(0,3) ; // Write the state name to the top right corner of the LCD
    lcd.print(state_n[state]) ; 

    lcd.setCursor(3,0) ;
    lcd.print("Error State") ;
    lcd.setCursor(3,1) ;
    lcd.print("SLT to RST") ;

    while(!SLT)
    {
      for(int e=0;e<error_c;e++)
      {
        if(error[e] != 0)
        {
          lcd.setCursor(0,2) ;
          lcd.print(clearline) ;
          lcd.setCursor(1,2) ;
          lcd.print("Error: ") ;
          lcd.print(e) ;
          lcd.setCursor(10,2) ;
          lcd.print(error_n[error[e]]) ;
          Serial.println(error_n[error[e]]) ;
          delay(1000) ;
        }
      }
      delay(1000) ;
      if(digitalRead(SEN_IL))
      {ILOK = true ;}
   }
   stateChange = true ;
   SLT = false ;
   ERR = false ;
   for(int err = 0;err<error_c;err++)
   { error[err] = 0 ; }
   state = 1 ; // Enter the startup state
  }
  Serial.println("Tried to leave the startup state") ;
  delay(1000) ;
  RUN = false ;
  stateChange = false ;
  lcd.clear() ;
}

void pushOUT()
{OUT = true;}
void pushUPW()
{UPW = true;}
void pushDWN()
{DWN = true;}
void pushSLT()
{SLT = true;}
void pushRUN()
{RUN = true;}

void ILBRK() // Using an interrupt, watch for the SEN_IL pin falling from high to low
{
  ILOK = false ; // Indicate the IL state as broken
  state = 4 ;
  error[error_count] = 1 ; // record the error
  error_count += 1 ;


    for(int k = 0;k<DRV_c;k++)
      {
        digitalWrite(DRV[k],LOW) ; // Shut down all the oven outputs
      }
}

void driveHTR1()
{
  cycle += 1 ;

  if(control_HTR1_count<cycle*control_HTR1/60.0)
    {
      control_HTR1_count += 1 ;
      digitalWrite(DRV[2],HIGH) ; // HEATER ON
    }
  else
    {
      digitalWrite(DRV[2],LOW) ; // HEATER OFF
    }
}

void blinkLEDState()
{
  if(blinkState)
  {
    analogWrite(DRV[6],0) ;
    blinkState = false ;
  }
  else
  {
    analogWrite(DRV[6],25) ;
    blinkState = true ;
  }
}

float interpTherm(int dataRead)
{
  int t = 0 ;   
  while(mapTherm[0][t]>dataRead)
  {
    t += 1 ;
  }
  return 5.0*(mapTherm[0][t-1]-dataRead)/(mapTherm[0][t-1]-mapTherm[0][t])+mapTherm[1][t-1] ;
}

// https://learn.adafruit.com/sous-vide-powered-by-arduino-the-sous-viduino

// write all parameters to EEPROM
