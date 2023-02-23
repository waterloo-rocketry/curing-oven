// OCE Firmware V1
// Calvin DeKoter
// Power Ranchers, Group 41
// Code is designed to run on the Teensy 4.1, with a custom PCB and all the necessary hardware

// Stuff to add

// Things to fix

  // todo need to add second thermistor screen with all thermistor values during cure cycle

// LOA

  //  LOA     List of Acronyms
  //  OCE     Oven Control Elements
  //  IL      Interlock Loop
  //  AIN     Analog INput
  //  AOUT    Analog OUTput
  //  SW      SWitch
  //  DRV     DRiVe output
  //  SEN_IL  Interlock Loop SENse
  //  IL_OK   Interlock Loop OKay
  //  PGM_OK  ProGraM OKay
  //  ERR     ERRor
  //  AUX     AUXiliary output
  //  VACC    VACCuum | Yes, I know this is a spelling error
  //  PIT     PITot tube
  //  UA*     User Analog
  //  HTR1    HeaTeR output 1
  //  CTRL    ConTRoL
  //  OUT     OUT
  //  UPW     UPWards
  //  DWN     DoWN
  //  SLT     SeLecT
  //  RUN     RUN
  //  RST     ReSeT
  //  CURS    CURSor
  //  CURS_CXT  CURSor_ConteXT | 
  //  Temp_Targ Temperature Target | The temperature that the oven will heat to and maintain for the scheduled cure time 
  //  EEPROM  Electronically Erasable PROgrammable Memory | Non-volatile memory to store curing information between startups
  //  OoS     Out of Scope | Cool feature that wasn't implemented
  
  // Shirley TODO

    /* 
    Events
    Duct disconnected
    Oven is opened and not closed fully
    Not unsafe, but could result in part failure
    Max temp would be around 60C
    SHould have some sort of check to ensure oven is recirculating
    High thermal gradient in oven
    Should ensure highest temperature area of oven does not exceed 171
    Thermistor located in high/low spot
    Leak springs in bag
    Causes aux to be on for full duration of cure
    Thermistor falls off part
    Something happens to the PCB?
    Documentation
    ambient set in firmware *?  

*/


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

  //  The board has a CAN bus on it, but it was never implemented - that would be really cool if you could integrate the controller to manage temps on the car or rocket
    
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
const int AIN_c = 16 ; // number of analog input pins
int AIN[] = {40,41,14,15,16,17,18,19,20,21,22,23,38,39,27,26} ; // 24,25 for SCA/SCL // physical pin numbers of each of the analog input pins
bool AIN_u[] = {true,true,true,true,true,true,true,true,true,false,false,true,false,false,false,false} ; // Array to select whether or not to collect data from the analog input - TH12 is the heater outlet temp
bool AIN_avg[] = {true,true,true,false,false,false,false,false,false,false,false,false,false,false,false,false}; // Array to select whether or not to include the thermistors in the oven temperarature average - note that the last 4 are always false, as they are not temperatures
// If the pin is in use, mark it to be read during the cure and cool states
const char *AIN_n[] = {"TH01","TH02","TH03","TH04","TH05","TH06","TH07","TH08","TH09","TH10","TH11","TH12","UA1","UA2","VS1","PIT1"} ;
// Names of all the analog input pins
bool SelfCheck = false ;

const int mapTherm[2][102] = {{993,985,975,962,948,930,911,888,862,834,803,770,734,697,658,619,580,541,503,466,430,396,364,334,306,280,256,234,214,195,178,163,149,136,125,115,105,97,89,82,75,70,64,59,55,51,47,44,41,38,36},{0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,130,135,140,145,150,155,160,165,170,175,180,185,190,195,200,205,210,215,220,225,230,235,240,245,250}} ;
                              // 10k Thermistor Divider (Old board rev) {{786,736,682,626,568,512,457,405,356,312,273,238,207,180,156,136,118,103,90,78,69,60,53,47,41,37,33,29,26,23,21,19,17,15,14,12,11,10,9,8,8,7,6,6,5,5,4,4,4,3,3},{0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,130,135,140,145,150,155,160,165,170,175,180,185,190,195,200,205,210,215,220,225,230,235,240,245,250}};

int AOUT[] =  {29,28} ; // Analog Output Pin Numbers
const char *AOUT_n[] = {"UA3","UA4"} ; // Analog Output Pin Names

int SW_c = 5 ; // Number of Switches
int SW[] = {3,4,5,6,7} ; // Switch Pin Numbers
const char *SW_n[] = {"OUT","UPW","DWN","SLT","RUN"} ; // Switch Names

IntervalTimer control_HTR1_Timer ; // Timer to run the slow PWM function
IntervalTimer blinkLED ; // Timer to blink the LED

int DRV_c = 7 ; // Number of drive outputs
int DRV[] = {11,13,12,32,8,9,10}; // Input Relay, Blower, Heater, Auxiliary, and LED Enable Pins 
const char *DRV_n[] = {"IRO1","BLW1","HTR1","AUX1","LED1","LED2","LED3"}; // Drive Output Names

int state = 1 ; // Start in the startup state
const char *state_n[] = {"OFF","STARTUP","CURE","ERROR"} ; // State Names

int SEN_IL = 1 ;

int error[6] = {0,0,0,0,0,0} ; // Array to store error code indexes
int error_c = 6 ; // Length of the error array
char *error_n[] = {"","IL BRK","TH BRK","TH OVT","VAC ERR","PGM NOK","TH ERR"} ; // Array to hold the error strings IL BRK: Interlock Loop Break, TH BRK: Thermistor Break, TH OVT: Thermistor Overtemperature,  VAC ERR: Vacuum level less than critical point, PGM NOK: Program has invalid settings, TH ERR: One of the thermistors failed on oven startup. Let the oven cool down or turn off the selfcheck.
int error_count = 0 ; // Running tally of the errors
 
char clearline = "                    " ; // 20 spaces to clear the display

bool stateChange = false ; // Boolean to allow the oven to pass by the remaining states after a transition
bool ILOK = false ;     // Boolean to indicate the state of the interlock loop
bool PGMOK = false ;  // Boolean to indicate the validity of the parameters (all positive, nonzero, temp less than Temp_Max_Part)
bool CURE = false ; // Boolean to indicate that the oven is in the CURE state
bool ERR = false ; // Bool to indicate the that the oven is in the ERR state
bool AUX = false ; // Bool to store the state of the auxiliary device


// Button State Variables
volatile bool OUT = false ; // Button to move OUT of the submenu
volatile bool UPW = false ; // Button to move the cursor upwards
volatile bool DWN = false ; // Button to move the cursor downwards
volatile bool SLT = false ; // Button to move SeLecT the submenu or option
volatile bool RUN = false ; // Button to RUN the oven - need to be in the main menu
volatile bool blinkState = false ; // Variable to store the state of the blinking light

// Oven Properties
int Temp_Max = 175 ; // Degrees C, immediate shutdown if exceeded
int Temp_Max_Control = 140 ; // Maximum temperature that the oven will deliberately heat to
int Temp_HeaterOff = Temp_Max_Control + 10 ; // Set the oven power to zero - this shouldn't ever occur, otherwise the temperature may oscillate wildly
int Temp_Max_Part = 130 ; // Maximum part temperature - will throw a Program Not OK error if it is exceeded during the startup phase
int Temp_Min = 10 ; // Indicates an open thermistor circuit, does not necessarily cause a shutdown unless all but two circuits are open
int Temp_Tol = 5 ; // Temperature tolerance across all the thermistors - probably need to open this up a bit

int Time_heat = 5*60*1000 ; // Heating ramp time in milliseconds
int Time_hold = 6*3600*1000 ; // Hold time in milliseconds
int Time_cool = 5*60*1000 ; // Cool time in milliseconds

int Temp_set = 70 ; // Degrees C
float Temp_avg = 0 ; // Average Temperature

int vacc_zero = 120 ; // Sensor output at ambient pressure
int vacc_fs = 700 ; // Sensor output at 29 in Hg - calibrate me with a nice gauge!
int vacc_low = 15 ; // change me to 10 or 20 inHg for actual use - low level threshold that triggers an error or slack message

int PIT1_zero = 114 ; // Sensor output at ambient pressure
int PIT1_fs = 400 ; // Sensor output at 1 in H2O // Need to calibrate properly, just sort of guessed

float UA1_gain = 1 ; // Gain for the two user inputs
float UA2_gain = 1 ; 

LiquidCrystal_Wire2_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// Controller Properties
float Kp[] = {0.5,1,5} ; // Proportional Gain
float Kd[] = {0.01,0.1,0.5} ; // Derivative Gain
float Ki[] = {0.1,0.1,0.5} ; // Integral Gain
int K_sz = 1 ; // Oven Size - Size two works really well @ 240V for the unit cell and 90 C
int K_Lim = 6 ; // Factor to scale back oven power as it goes over the Temp_Max_Control limit

volatile int cycle = 1 ; // Rolling counter to generate the PWM cycle
volatile int control_HTR1_count = 0 ; // Counter to log how many cycles the heater is on for
volatile int control_HTR1 = 0 ; // Range of time HTR1 is enabled, over 1 second - 0 - 59 out of 60 electrical cycles

// Data Structures
const int data_width = 1 + 1 + AIN_c + 1 ; // Time plus Temp_set plus analog plus control setpoint

float data[data_width] ;  // Array to store the data logged at each time step - only one row, data is immediately logged to the SD card

bool DataSave = 1 ; // Boolean to determine if data will be saved - this is sort of crusty, as we initialize the SD card header each time TODO - fix this
bool DataUpload = 0 ; // Boolean to determine if the data will be uploaded to the web server
char filename[] = "oven1.txt" ; // name of the file the data is saved to

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

  attachInterrupt(digitalPinToInterrupt(SW[0]), pushOUT, FALLING) ; // Attach the interrupt function to the pins
  attachInterrupt(digitalPinToInterrupt(SW[1]), pushUPW, FALLING) ;
  attachInterrupt(digitalPinToInterrupt(SW[2]), pushDWN, FALLING) ;
  attachInterrupt(digitalPinToInterrupt(SW[3]), pushSLT, FALLING) ;
  attachInterrupt(digitalPinToInterrupt(SW[4]), pushRUN, FALLING) ;
  
  for(int k=0;k<DRV_c; k++) // Assign the drive output to output mode
  {
    pinMode(DRV[k],OUTPUT) ;
  }

  pinMode(SEN_IL,INPUT_PULLDOWN) ; // Set up the interlock loop sense pin
  attachInterrupt(digitalPinToInterrupt(SEN_IL), ILBRK, FALLING) ; // Assign the pin to the falling interrupt - when it is HIGH, the IL is OK
  
  pinMode(AOUT[1],OUTPUT) ; // Setup the analog output pins
  pinMode(AOUT[2],OUTPUT) ;

  if(digitalRead(SEN_IL))  // Confirm the IL is in the correct state on startup
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
        delay(10) ;
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
      
   /* while(SD.exists(filename)) // TODO - fix the filenaming scheme
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
      lcd.print("IL LOOP OPEN") ; // If the Emergency Stop is open, or the thermal fuse is open, or disconnected, the IL loop will be broken - check the indicator in the top left of the board
      state = 1 ;
    }

    if(digitalRead(SEN_IL))
      {ILOK = true ;}

    int curs = 0 ; // High level menu cursor
    int curs_hrz = 0 ; // Horizontal cursor
    int curs_row = 1 ; // Vertical cursor
      
    while(ILOK && !RUN)
    {
      // Menu for setting all parameters
      lcd.setCursor(0,0) ;
      lcd.print("MAIN MENU    ") ;
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
        if(curs!=0) // Keep the cursor on the screen
          {curs -= 1 ;}  // Move the cursor upwards
        else{curs = 3;}  // Wrap the cursor to the bottom of the screen
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
      
      while(curs == 0 && !OUT && SLT)  // Enter the Cure Menu
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
        if(Time_heat<100){lcd.print(" ") ;}
        if(Time_heat<10){lcd.print(" ")  ;} 
        lcd.print(Time_heat/(60*1000)) ; // Need to fix the padding / leading zeros here with a case statement or helper function
        lcd.setCursor(16,0) ;
        lcd.print("[C]") ;
        lcd.setCursor(14,1) ;
        lcd.print("[min]") ;
        lcd.setCursor(0,2) ;
        lcd.print("Time Hold: ") ;
        lcd.print(Time_hold/(3600*1000)) ;
        lcd.setCursor(16,2) ;
        lcd.print("[h]") ;  // Todo if the time is less than one hour, change the units and print the time in minutes
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
              if(Temp_set<100){lcd.print(" ") ;} // good example of padding
              if(Temp_set<10){lcd.print(" ")  ;} 
              lcd.print(Temp_set) ;
            }
            else if(curs_cxt==1)
            {
              Time_heat += 1000*60*5 ;
              lcd.setCursor(11,curs_cxt) ;
              lcd.print(Time_heat/(60*1000)) ; // needs padding and unit switching
            }
            else if(curs_cxt == 2)
            {
              Time_hold += 1000*60*5 ;
              lcd.setCursor(11,curs_cxt) ;
              lcd.print(Time_hold/(3600*1000)) ; // needs padding and unit switching
            }
            else if(curs_cxt == 3)
            {
              Time_cool += 1000*60*5 ;
             lcd.setCursor(11,curs_cxt) ;
             lcd.print(Time_cool/(60*1000)) ; // needs padding and unit switching (same thing below)
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
        SLT = false ;
        // Sense Menu 
        Serial.println("Welcome to the Sense Menu") ; 
        lcd.clear() ;

        lcd.setCursor(0,0) ;
        lcd.print("TH SLT: ") ;
        
        for(int t = 0;t<12;++t) // print out whether or not the thermistors will have data logged from them
        {
          if(AIN_u[t])
          {lcd.print("1");}
          else{lcd.print("0");}
        }
        

        lcd.setCursor(0,2) ;
        lcd.print("A:") ;
        
        for(int ta = 0;ta<12;++ta) // print out whether or not the thermistors will be included in the part average
        {
          if(AIN_avg[ta])
          {lcd.print("1");}
          else{lcd.print("0");}
        }

        lcd.print("S:") ; // print out whether or not the other analog sensors are active

        for(int s = 12;s<16;++s)
        {
          if(AIN_u[s])
          {lcd.print("1");}
          else{lcd.print("0");}
        }

        lcd.setCursor(curs_hrz,curs_row) ;
        lcd.print("^") ;
        Serial.print(curs_hrz) ;
        Serial.println(curs_row) ;

        if(DWN)
        {
          Serial.println("UPW") ;
          if(curs_hrz < 20)
            {curs_hrz += 1;}
          else if(curs_row == 1)
          {
            curs_hrz = 0 ;
            curs_row = 3 ;  
          }
          else
          {
            curs_hrz = 0 ;
            curs_row = 1 ;
          }
          DWN = false ;
        }
        
        if(UPW)
        {
          Serial.println("DWN") ;
          if(curs_hrz != 0)
          {curs_hrz -= 1;}
          else if(curs_row == 3)
          {
            curs_hrz = 19;
            curs_row = 1 ;
          }
          else
          {
            curs_hrz = 19 ;
            curs_row = 3 ;
          }
          UPW = false ;
        }

        delay(500) ;

        if(SLT) // may have to fix this in the morning....
        {
          Serial.println("SLT") ;
          if(curs_row == 1 && curs_hrz > 7)
          {
            if(AIN_u[curs_hrz-8]){AIN_u[curs_hrz-8] = false;}
            else{AIN_u[curs_hrz-8] = true ;}
          }
          else if(curs_row == 3 && curs_hrz > 1 && curs_hrz < 15)
          {
            if(AIN_avg[curs_hrz-2]){AIN_avg[curs_hrz-2] = false;}
            else{AIN_avg[curs_hrz-2] = true ;}
          }
          else if(curs_row == 3 && curs_hrz > 16)
          {
            if(AIN_u[curs_hrz-4]){AIN_u[curs_hrz-4] = false;}
            else{AIN_u[curs_hrz-4] = true ;}
          }
        }
        
        
        SLT = true ;
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
        lcd.setCursor(0,2) ;
        lcd.print("Oven SelfCheck: ") ; // If enabled, the oven checks to make sure that the values of the thermistors are within Temp_Tol of eachother (typically 5 C)
        if(SelfCheck)
          {
            lcd.setCursor(2,17) ;
            lcd.print("Yes") ;
          }   
        else
          {
            lcd.setCursor(2,17) ;
            lcd.print("No ");
          } 

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
            else if(curs_cxt==2)
            {SelfCheck = true ;
              lcd.setCursor(13,2) ;
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
            else if(curs_cxt==2)
              {SelfCheck = false ;
              lcd.setCursor(13,2) ;
              lcd.print("No");}
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
    
    if(Temp_set<Temp_Max_Part) // Add more checks here
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
    float error_P_air = 0 ;
    float error_D_air = 0 ;
    float error_I_air = 0 ;
    float ambient = 20 ; // Guess at the first ambient temperature
    int avg_c = 0 ;
    int HeaterOffTime = 0 ;


    int curs_scrn = 0 ; 

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
      
      data[0] = now() ; // Returns the time in seconds since the program started - need to figure out RTC todo

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

          if(SelfCheck && d>0 && AIN_u[d+1])
          {
            for(int sc=0;sc<d;sc++)
            {
              if(abs(data[d+2] - data[sc+2]) > Temp_Tol)
              {
                error[error_count] = 6 ; // Thermistor outside Temp_Tol on startup, move to oven ERROR state
                error_count += 1 ;
                ERR = true ;
              }
            }
            SelfCheck = false ;
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
        data[14] = (analogRead(AIN[14]) - vacc_zero)*29.0/vacc_fs  ;

        if(data[14]<vacc_low)
        {
          ERR = true ;
          error[error_count] = 4 ;
          error_count += 1 ;
          
          Serial.println("VACC ERR") ;
          digitalWrite(DRV[5],LOW) ; // Turn off the VOK light 
        }
        else
        {
          digitalWrite(DRV[5],HIGH) ; // Turn on the VOK light
        }
      } 
    
    if(AIN_u[15])
      {data[15] = analogRead(AIN[15]) ;} // Pitot Tube PIT1
       
    
    Temp_avg =  Temp_avg/avg_c ; // Need to decide on average vs. particular temp for control
    data[1] = (Temp_set-ambient)*rampline + ambient ; // Log the setpoint
    // todo log the average temp
    avg_c = 0 ;

    if(abs(Temp_avg-data[1])>Temp_Tol) // If the temperature is within the 5 °C tolerance, turn on the "TEMP OK" light
    { digitalWrite(DRV[4],LOW)  ; }
    else
    { digitalWrite(DRV[4],HIGH) ; } 

    error_D = (((Temp_set-ambient)*rampline + ambient - Temp_avg) - error_P)/ts ; // New error_P minus old error_P divided by the timestep
    error_P = ((Temp_set-ambient)*rampline + ambient - Temp_avg) ;

    if(control_HTR1<58 && control_HTR1>1 && data[11+2] < Temp_Max_Control) // If the heater is not turned off and is not at maximum heating power, and the oven is not at the maximum temperature
    {
      error_I += error_P*ts ;
    }  

    if((AIN_u[11] && data[11+2] > Temp_HeaterOff) || HeaterOffTime > 0) // Over-temp cooldown - If T_HeaterOut is active, and the temperature is above the critical threshhold, turn off the heater for 30 seconds
    {
      control_HTR1 = 0; // shut down the heater power - stay for 10 s
      if(HeaterOffTime <= 0 && AIN_u[11] && data[11+2] > Temp_HeaterOff){HeaterOffTime = 10 ;} // If the heater off time is at zero or less, and the temperature has not yet declined to lesss than the max heater temp, then reset the counter
      else if(HeaterOffTime > 0){HeaterOffTime -= 1 ;}
    } 
    else
    {
      control_HTR1 = int(Kp[K_sz]*error_P + Kd[K_sz]*error_D + Ki[K_sz]*error_I) ; // First pass at control signal - basic PID loop with additional consideration for the Temp_Max_Control limit
    }
     if(control_HTR1>59)
     {
      control_HTR1=59 ;
     }
     else if(control_HTR1<0)
     {
      control_HTR1=0 ;
     }  

    control_HTR1 += K_Lim*min(Temp_Max_Control - data[11+2],0) ;

    data[data_width-1] = control_HTR1 ;

     // Display all the relevant parameters if the default screen is enabled

    if(UPW)
    {
      if(curs_scrn < 1)
      {
        curs_scrn += 1 ;
      }
      else{curs_scrn = 0 ;}
    }
    if(DWN)
    {
      if(curs_scrn > 0)
      {
        curs_scrn -= 1 ;
      }
      else{curs_scrn = 1 ; }
    }

    if(curs_scrn == 0)
    {
      lcd.setCursor(0,0) ;
      lcd.print("Temp Targ: ") ;
      lcd.print(int((Temp_set-ambient)*rampline+ambient)) ;
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
    }
  /*  else if(curs_scrn == 1)
    {
      lcd.setCursor(0,0) ;
      lcd.print("T01   T02   T03   ") ;
      lcd.setCursor(0,1) ;
      for(int th = 0; th<3; th++)
      {
        if(AIN_u[th+2])
        {
          if(data[th+2] < 100){lcd.print("0");}
          if(data[th+2] < 10){lcd.print("0") ;}
          lcd.print(data[th+2]) ;
        }
        else{lcd.print("   ") ;}

        lcd.print("   ") ;
      }

      lcd.setCursor(0,2) ;
      lcd.print("T04 T05 T06   ") ;
      lcd.setCursor(0,3) ;
      for(int th = 3; th<6; th++)
      {
        if(AIN_u[th+2])
        {
          if(data[th+2] < 100){lcd.print("0");}
          if(data[th+2] < 10){lcd.print("0") ;}
          lcd.print(data[th+2]) ;
        }
        else{lcd.print("   ") ;}

        lcd.print("   ") ;
      } 
    */
    /*else if(curs_scrn == 2)
    {
      lcd.setCursor(0,0) ;
      lcd.print("T07T08T09T10T11T12") ;
      lcd.setCursor(0,1) ;
      for(int th = 0; th<6; th++)
      {
        if(AIN_u[th+2])
        {lcd.print(sprintf("%03d",data[th+8])) ;}
        else{lcd.print("   ") ;}
      }
    } */
    
      delay(900) ; // Delay to avoid logging an obscene amount of data
      cycle = 1;
      control_HTR1_count = 0 ;
      lcd.clear() ;

     
      if(millis()>(Time_heat+Time_hold+Time_cool+Time_start))
      {
        CURE = false ;
      }


  // Run the Auxiliary Device if the SLT button is pushed
  
  if(SLT && !AUX) // TODO can we find some way to protect the auxiliary device if it is a vacuum pump or motor or something
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


    for(int s = 0;s<data_width-1;s++)
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
{
  //delayMicroseconds(10000) ;
  OUT = true;
}
void pushUPW()
{
  //delayMicroseconds(10000) ;
  UPW = true;
}
void pushDWN()
{
  //delayMicroseconds(10000) ;
  DWN = true;
}
void pushSLT()
{
  //delayMicroseconds(10000) ;
  SLT = true;
}
void pushRUN()
{
  //delayMicroseconds(10000) ;
  RUN = true;
}

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
    digitalWrite(DRV[6],LOW) ;
    blinkState = false ;
  }
  else
  {
    digitalWrite(DRV[6],HIGH) ;
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

// write all parameters to EEPROM todo
