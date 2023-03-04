// OCE Firmware V1
// Calvin DeKoter
// Power Ranchers, Group 41
// Code is designed to run on the Teensy 4.1, with a custom PCB and all the necessary hardware

// Stuff to add

// Calibration routine for the user analog inputs with variable gain - use the user analog outputs

// Used Libraries

#include <LiquidCrystal_I2C.h> // need to switch the header in the library to use Wire2
#include <TimeLib.h>
#include <Bounce.h> // Library to debounce switch inputs
#include <IntervalTimer.h> // Library to create timer interrupt objects
#include <Ethernet.h> // Library to post data to the internet
// Teensy CAN

// Function Prototypes

void pushOUT() ;
void pushUPW() ;
void pushDWN() ;
void pushSLT() ;
void pushRUN() ;

void ILBRK() ;

void driveHTR1() ;

// Pin Assignments

const int AIN_c = 16 ; 
// number of analog input pins
int AIN[] = {40,41,14,15,16,17,18,19,20,21,22,23,38,39,27,26} ;
// physical pin numbers of each of the analog input pins
bool AIN_u[] = {true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,} ;
// If the pin is in use, mark it to be read during the cure and cool states
char *AIN_n[] = {"TH01","TH02","TH03","TH04","TH05","TH06","TH07","TH08","TH09","TH10","TH11","TH12","UA1","UA2","VS1","PIT1"} ;
// Names of all the analog input pins

int AOUT[] =  {29,28} ;
char *AOUT_n[] = {"UA3","UA4"} ;

int SW_c = 5 ;
int SW[] = {3,4,5,6,7} ;
char *SW_n[] = {"OUT","UPW","DWN","SLT","RUN"} ;

IntervalTimer control_HTR1_Timer ;

int DRV_c = 7 ;
int DRV[] = {11,13,12,32,8,9,10}; // Input Relay, Blower, Heater, Auxiliary, and LED Enable Pins 
char *DRV_n[] = {"IRO1","BLW1","HTR1","AUX1","LED1","LED2","LED3"};

int state = 1 ; // Start in the startup state
char *state_n[] = {"OFF","STARTUP","CURE","COOL","ERROR"} ;

int SEN_IL = 1 ;
int UA3 = 29 ;
int UA4 = 28 ;

int error_c = 6 ;
char *error[] = {"None      ","          ","          ","          ","          ","          "} ; // Array to hold the error strings
int error_count = 0 ;

char clearline = "                                        " ;

bool stateChange = false ;
bool ILOK = false ;     // Boolean to indicate the state of the interlock loop
bool paramOK = false ;  // Boolean to indicate the validity of the parameters (all positive, nonzero, temp less than Temp_Max_Part)
bool CURE = false ;
bool COOL = false ;
bool ERR = false ;

volatile bool OUT = false ;
volatile bool UPW = false ;
volatile bool DWN = false ;
volatile bool SLT = false ;
volatile bool RUN = false ;

// Oven Properties

int Temp_Max = 200 ; // Degrees C, immediate shutdown if exceeded
int Temp_Max_Control = 180 ; // Maximum temperature that the oven will deliberately heat to
int Temp_Max_Part = 130 ; // Maximum part temperature
int Temp_Min = 0 ; // Indicates an open thermistor circuit, does not necessarily cause a shutdown unless all circuits are open

int Time_heat = 1*3600*1000 ; // Heating ramp time in milliseconds
int Time_hold = 1*3600*1000 ; // Hold time in milliseconds
int Time_cool = 1*3600*1000 ; // Cool time in milliseconds

int Temp_set = 80 ; // Degrees C
float Temp_avg = 0 ; // Average Temperature

float UA1_gain = 1 ; // Gain for the two user inputs
float UA2_gain = 2 ; 

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// Controller Properties

float Kp = 1 ; // Will be challenging to find a good set of gains for every circumstance - will consider gain scheduling or a self-calibration sequence
float Kd = 0.1 ;
float Ki = 0.1 ;

volatile int cycle = 1 ;
volatile int control_HTR1_count = 0 ;
volatile float control_HTR1 = 0 ; // Range of time HTR1 is enabled, over 1 second - 0 - 59 out of 60 electrical cycles

// Data Structures
const int data_width = 1 + AIN_c ;

float data[data_width] ;  

void setup() {

  for(int i=0;i<AIN_c;1) // Assign the analog input pins to the analog mode
  {
    pinMode(AIN[i],INPUT) ;
  }

  for(int j=0;j<SW_c;1) // Assign the switch inputs to input pullup mode
  {
    pinMode(SW[j],INPUT_PULLUP) ;
  }
  
  attachInterrupt(digitalPinToInterrupt(SW[1]), pushOUT, FALLING) ;
  attachInterrupt(digitalPinToInterrupt(SW[2]), pushUPW, FALLING) ;
  attachInterrupt(digitalPinToInterrupt(SW[3]), pushDWN, FALLING) ;
  attachInterrupt(digitalPinToInterrupt(SW[4]), pushSLT, FALLING) ;
  attachInterrupt(digitalPinToInterrupt(SW[5]), pushRUN, FALLING) ;

  for(int k=0;k<DRV_c;1) // Assign the drive output to output mode
  {
    pinMode(DRV[k],OUTPUT) ;
  }

  pinMode(SEN_IL,INPUT_PULLDOWN) ;
  attachInterrupt(digitalPinToInterrupt(SEN_IL), ILBRK, FALLING) ;
  
  pinMode(UA3,OUTPUT) ;
  pinMode(UA4,OUTPUT) ;  
}

void loop() {
  
  if(state = 1 && !stateChange) // Enter the STARTUP State
  {
    lcd.setCursor(30,0) ; // Write the state name to the top right corner of the LCD
    lcd.print(state_n[state]) ; 

    if(!ILOK)
    {
      lcd.setCursor(3,1) ;
      lcd.print("INTERLOCK LOOP OPEN") ;
      state = 1 ;
    }

      int curs = 0 ; // High level menu cursor
      
    while(ILOK && !RUN && !paramOK)
    {
      // Menu for setting all parameters
      lcd.setCursor(2,0) ;
      lcd.print("MAIN MENU   ") ;
      lcd.println(" CURE") ;
      lcd.setCursor(15,1) ;
      lcd.println(" SENSE") ;
      lcd.setCursor(15,2) ;
      lcd.println(" DATA") ;
      lcd.setCursor(15,3) ;
      lcd.println(" CTRL") ;

      lcd.setCursor(14,curs) ;
      lcd.print(">") ;

      int curs_cxt = 0 ; // Context Menu Cursor

      if(UPW)
      {
        curs += 1 ;
        delay(50) ;
        UPW = false ;
      }
      if(DWN)
      {
        curs -= 1 ;
        delay(50) ;
        DWN = false ;
      }
      
      delay(500) ;
      lcd.clear() ;
      
      while(curs == 0 && !OUT && SLT)
      {
        delay(50) ; 
        SLT = false ;
        
        // Cure Menu
        lcd.setCursor(2,0) ;
        lcd.print("Temp. Target: ") ;
        lcd.print(Temp_set) ;
        lcd.setCursor(2,1) ;
        lcd.print("Time Heat: ") ;
        lcd.print(Time_heat/(60*1000)) ;
        lcd.print("[min]") ;
        lcd.setCursor(2,2) ;
        lcd.print("Time Hold: ") ;
        lcd.print(Time_hold/(3600*1000)) ;
        lcd.print("[h]") ;
        lcd.setCursor(2,0) ;
        lcd.print("Time Cool: ") ;
        lcd.print(Time_cool/(60*1000)) ;
        lcd.print("[min]") ;

        lcd.setCursor(2,curs_cxt) ;
        
        if(UPW)
        {
          curs_cxt += 1 ;
          delay(50) ;
          UPW = false ;
        }
        if(DWN)
        {
          curs_cxt -= 1 ;
          delay(50) ;
          DWN = false ;
        }

        while(SLT && !OUT)
        {
          if(UPW) // If the up button has been pressed, increment the values
          {
            if(curs_cxt==0)
            {Temp_set += 2;}
            else if(curs_cxt==1)
            {Time_heat += 1000*60*5 ;}
            else if(curs_cxt == 2)
            {Time_hold += 1000*60*30 ;}
            else if(curs_cxt == 3)
            {Time_cool += 1000*60*5 ;}

            delay(50) ;
            UPW = false ;  
          }
          else if(DWN) // if the down button has been pressed, decrement the value
          {
            if(curs_cxt==0)
            {
              Temp_set -= 2;
              lcd.setCursor(16,curs_cxt) ;
              lcd.print(Temp_set) ;
            }
            else if(curs_cxt==1)
            {
              Time_heat -= 1000*60*5 ;
              lcd.setCursor(13,curs_cxt) ;
              lcd.print(Time_heat) ;
            }
            else if(curs_cxt == 2)
            {Time_hold -= 1000*60*30 ;}
            else if(curs_cxt == 3)
            {Time_cool -= 1000*60*5 ;}

            delay(50) ;
            DWN = false ;  
          }
          
           // Push the OUT button to exit the while loop
        }

        delay(50) ;
        OUT = false ;

        
        SLT = true ;
         
      }
      while(curs == 1 && SLT)
      {
        // Sense Menu  
        
        if(OUT)
        {
          SLT = false ;
        }
      }
      while(curs == 2 && SLT)
      {
        // Data Menu 
        
        if(OUT)
        {
          SLT = false ;
        } 
      }
      while(curs == 3 && SLT)
      {
        // Controller Menu 
        
        if(OUT)
        {
          SLT = false ;
        } 
      }
    }

    stateChange = true ;
    state = 2 ; // Proceed to the cure state
    
  }

  if(state = 2 && !stateChange) // Enter the CURE State
  {
    lcd.setCursor(30,0) ; // Write the state name to the top right corner of the LCD
    lcd.print(state_n[state]) ; 

    // Switch on the oven input relay
    digitalWrite(DRV[1],HIGH) ; // check whether the ULQ is active high or low
    digitalWrite(DRV[2],LOW) ;
    digitalWrite(DRV[3],LOW) ;
    digitalWrite(DRV[4],LOW) ;

    int Time_start = millis() ;
    int ts = 1000 ;
    int error_I = 0 ;
    float Temp_last = 0 ;
    int avg_c = 0 ;

    control_HTR1_Timer.begin(driveHTR1,16666) ;
  
    while(ILOK && CURE && !ERR)
    {
      
      // log the data - need a fuction

      data[0] = now() ; // Returns the time in seconds since 1970...

      // Calculate the rampline value

      
      Temp_last = Temp_avg ; // should be averaging the last and next temperatures over 10 seconds to avoid noise
      Temp_avg = 0 ;

      
      for(int d = 0;d<data_width;1)
      {
        if(AIN_u[d])
        {
          if(d+1<13) // Read in Thermistor Data
          {
            data[d+1] = analogRead(AIN[d]) ; // add temp conversion, polynomical NTC fit 
            if(data[d+1]<10)
            {
              error[error_count] = strcat(strcat("TH",char(d+1))," BRK") ; // Thermistor disconnected, move to oven ERROR state
              ERR = true ;
            }
            if(data[d+1]>200)
            {
              error[error_count] = strcat(strcat("TH",char(d+1))," OVT") ; // Overtemperature on thermistor, move to oven ERROR state
              ERR = true ;
            }
            Temp_avg += data[d+1] ;
            avg_c += 1;
          }
          
          if(12<d+1 && d+1<15) // Read in UA2 Data
          {
            data[d+1] = analogRead(AIN[d]) ; // add generic conversion to volts based on calibration
          } 
          

          if(d+1==15)
          {
            data[d+1] = analogRead(AIN[d]) ; // Read in the vacuum sensor pressure
          }

          if(d+1==16)
          {
            data[d+1] = analogRead(AIN[d]) ; // Read in the differential sensor pressure
          }
          
        }
      }
    
    Temp_avg = Temp_avg/avg_c ; // Need to decide on average vs. particular temp for control

    if(control_HTR1<59||control_HTR1<0)
    {
      error_I += (Temp_set-Temp_avg)*ts ;
    } 
    control_HTR1 = Kp*(Temp_set - Temp_avg)+Kd/ts*(Temp_last-Temp_avg)+Ki*error_I ;

    // at program start, we want to pin the heating controller 120 C target - 20 C
    // 50*KP > map to 0-59

     if(control_HTR1>59)
     {
      control_HTR1=59 ;
     }
     else if(control_HTR1<0)
     {
      control_HTR1=0 ;
     }

      // write the data to the webpage

      // client.println("

      delay(980) ; // Delay to avoid logging an obscene amount of data

      if(millis()>(Time_heat+Time_hold-Time_start))
      {
        CURE = false ;
      }
  }

    control_HTR1_Timer.end() ;
    stateChange = true ;

    if(!ERR && ILOK){state = 3 ;} // Enter the cool state
    else{state = 4;}

  }

  if(state == 3 && !stateChange) // Enter the COOL State
  {
    lcd.setCursor(30,0) ; // Write the state name to the top right corner of the LCD
    lcd.print(state_n[state]) ; 

    while(COOL && !ERR)
    {
    // log the data
    // write the data to the internet
    }

    stateChange = true ;
    
    if(!ERR){state = 1 ;} // Enter the startup state
    else{state = 4;}
  }

  if(state = 4 && !stateChange) // Enter the ERROR State
  // TODO for error state - make sure that all the prints happen correctly
  {
    for(int k = 0;k<DRV_c;1)
    {
      digitalWrite(DRV[k],LOW) ; // Shut down all the oven outputs
    }
    
    
    lcd.setCursor(30,0) ; // Write the state name to the top right corner of the LCD
    lcd.print(state_n[state]) ; 

    lcd.setCursor(3,0) ;
    lcd.print("Error State") ;
    lcd.setCursor(2,1) ;
    lcd.print(error[0]) ;
    lcd.setCursor(3,2) ;
    lcd.print("SLT and IL Open to reset") ;

    while(ERR && !SLT && ILOK)
    {

      for(int e=0;e<error_c;1)
      {
        if(error[e]!="          ")
        {
          lcd.setCursor(0,3) ;
          lcd.print(clearline) ;
          lcd.setCursor(1,3) ;
          lcd.print("Error: ") ;
          lcd.print(e) ;
          lcd.setCursor(10,3) ;
          lcd.print(error[e]) ;
          delay(1000) ;
        }
      }
    }

   stateChange = true ;
   state = 1 ; // Enter the startup state

    
  }

  delay(50) ;
  stateChange = false ;
  lcd.clear() ;

}

void pushOUT()
{OUT = true;}
void pushUWP()
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
  error[error_count] = "IL BRK" ; // record the error
}

void driveHTR1()
{
  
  cycle += 1 ;

  if(control_HTR1<cycle*control_HTR1/60)
  {
    control_HTR1_count += 1 ;
    digitalWrite(DRV[3],HIGH) ; // Make sure to check the ULQ2004A behaviour
  }
  else
  {
    digitalWrite(DRV[3],LOW) ; // Make sure to check the ULQ2004A behaviour
  }
 
}

// https://learn.adafruit.com/sous-vide-powered-by-arduino-the-sous-viduino

// write all parameters to EEPROM
