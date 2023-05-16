import tkinter as tk

import csv
import time

global bool_RUN
bool_RUN = False
global ILBRK
bool_ILBRK = False

global bool_OUT
bool_OUT = False

global bool_UPW
bool_UPW = False

global bool_DWN
bool_DWN = False

global bool_SLT
bool_SLT = False




def Serialprintln(s):  # Function to print to a tkinter window representing the serial port
    pass  # Print to the serial port tkinter window



class lcd():

    def __init__(self, x, y):  # Initialize the lcd
        self.x = x
        self.y = y
        self.lcdstr = y * (x * '_' + '\n')
        self.lcdclr = self.lcdstr  # set up the empty screen variable
        self.crs = [0, 0]  # Cursor
        
#    def init():
        pass
 #   def begin():
        pass
    def clear(self):
        l_lcd.config(text=self.lcdclr)
        # Clears the display
    def home():
        pass
    def noDisplay():
        pass
    def display():
        pass
    def noBlink():
        pass
    def blink():
        pass
    def noCursor():
        pass
    def cursor():
        pass
    def scrollDisplayLeft():
        pass
    def scrollDisplayRight():
        pass
    def leftToRight():
        pass
    def rightToLeft():
        pass
    def shiftIncrement():
        pass
    def shiftDecrement():
        pass
    def noBacklight():
        pass
    def backlight():
        pass
    def autoscroll():
        pass
    def noAutoscroll():
        pass
    def createChar():
        pass
    def setCursor(self, x, y):  # Uses 0-index (i.e. 0 to y - 1 lines, 0 to x - 1 characters)
        self.crs[0] = x
        self.crs[1] = y
    def print(self, string):
        crs_1 = self.crs[0] + self.x * self.crs[1] + self.crs[1]  # Figure out how far into the string the cursor is
        self.lcdstr = self.lcdstr[:crs_1] + string + self.lcdstr[crs_1 + len(string):]  # need to define the line wrapping / overflow behaviour

        l_lcd.config(text=self.lcdstr)
        self.crs[0] += len(string) - 1

        # Hmmmmm according to a forum post the line wrapping behaviour is 0-2-1-3

        
    def blink_on():
        pass
    def blink_off():
        pass
    def cursor_on():
        pass
    def cursor_off():
        pass
    def setBacklight():
        pass
    def load_custom_character():
        pass
    def printstr():
        pass


def OUT():
    print('Back')

    global bool_OUT

    bool_OUT = True


def UPW():
    print('Upwards')  # Need to find a clever way to represent the interrupts
    global bool_UPW

    bool_UPW = True


def DWN():
    print('Downwards')

    global bool_DWN

    bool_DWN


def SLT():
    print('Select')

    global bool_SLT

    bool_SLT = True


def RUN():
    print('Run')

    global bool_RUN
    
    bool_RUN = True

def ILOK():
    print('ILOK')
    bool_ILOK = False




# Set up Curing Oven Display
LCD = lcd(20, 4)  # Initialize the LCD class


root=tk.Tk()
root.title('Power Ranchers Curing Oven')
root.geometry("800x600")

l_lcd = tk.Label(root, text=LCD.lcdstr, relief='raised', height=4, width=20, justify=tk.LEFT, anchor='nw', font='Consolas')
l_lcd.place(x=100,y=100)

l_LED1 = tk.Label(root, text='', relief='raised', height=1, width=1, bg='gray')
l_LED1.place(x=350, y=100)
l_LED2 = tk.Label(root, text='', relief='raised', height=1, width=1, bg='gray')
l_LED2.place(x=350, y=133)
l_LED3 = tk.Label(root, text='', relief='raised', height=1, width=1, bg='gray')
l_LED3.place(x=350, y=167)

l_blower = tk.Label(root, text='Blower', relief='raised', height=4, width=10, fg='blue')
l_blower.place(x=0, y=400)
l_heater = tk.Label(root, text='Heater', relief='raised', height=4, width=10, fg='red')
l_heater.place(x=0, y=500)
l_aux = tk.Label(root, text='Auxiliary', relief='raised', height=4, width=10, fg='green')
l_aux.place(x=0, y=300)

l_OAL = tk.Label(root, text='OAL', relief='raised', height=5, width=10, fg='red')
l_OAL.place(x=400, y=150)
# l_OAL.config(bg='red', fg='black')


b_BACK = tk.Button(root, text="OUT", command=OUT)
b_BACK.place(x=100, y=200)
b_UPW = tk.Button(root, text="UP", command=UPW)
b_UPW.place(x=150, y=200) 
b_DWN = tk.Button(root, text="DWN", command=DWN)
b_DWN.place(x=200, y=200)
b_SLT = tk.Button(root, text="SLT", command=SLT)
b_SLT.place(x=250, y=200)
b_RUN = tk.Button(root, text="RUN", command=RUN)
b_RUN.place(x=300, y=200)

b_estop = tk.Button(root, text="E-Stop", command=ILOK)
b_estop.place(x=415, y=115)

state = 'STARTUP'
stateChange = False

curs = 0

try:
    while True:  # Equivalent to the main Arduino loop
        
    
        if state == 'STARTUP' and not stateChange:

            curs_hrz = 0
            curs_row = 1

            print('Welcome to the Startup State')

            LCD.setCursor(0, 0)
            LCD.print('MAIN MENU____')
            LCD.print('CURE')
            LCD.setCursor(12, 1)
            LCD.print('SENSE')
            LCD.setCursor(12, 2)
            LCD.print('DATA')
            LCD.setCursor(12, 3)
            LCD.print('CTRL')

            LCD.setCursor(11, curs)
            LCD.print('>')

            if bool_UPW:
                if curs != 0:
                    curs -= 1
                else:
                    curs = 3
                print(curs)
                bool_UPW = False

            if bool_DWN:
                if curs < 3:
                    curs += 1
                else:
                    curs = 0
                bool_DWN = False

            while curs == 0 and not OUT and SLT:

                SLT = False

                SLT = True  # Used to re-enter the loop

            if bool_RUN:
                stateChange = True
                state = 'CURE'

        if state == 'CURE' and not stateChange:

            print('Welcome to the Cure State')

        if state == 'ERROR' and not stateChange:

            print('Welcome to the Error State')
        
        root.update_idletasks()
        root.update()
        time.sleep(1)
        stateChange = False
        LCD.clear()


        
except KeyboardInterrupt:
    print('closing OCE')
    quit()

# Code import

# Sort text line by line

#   Variables - if the line begins with the keywords const int, int, etc

##d_int = {}
##
##d_floats = {}
##
##d_bools = {}
##
##d_str = {}
##
##with open(r'C:\GrabCAD-ME481\04 - Oven Control Elements\03 - Software\DigitalTwin\OCE_Firmware.txt', newline='') as f_dt:
##
##    for row in f_dt:
##        
##        if 'int' in row.split('=')[0] and 'Timer' not in row.lstrip() and '//' not in row and '=' in row and 'for' not in row:
##            print('int')
##            if '[]' in row:
##                if row.split('=')[1].split(' ')[1].lstrip('{').rstrip('}').split(',')[0].isnumeric(): 
##                    d_int[row.split('=')[0].split(' ')[-2]] = [int(a) for a in row.split('=')[1].split(' ')[1].lstrip('{').rstrip('}').split(',')]
##                else:
##                    d_int[row.split('=')[0].split(' ')[-2]] = row.split('=')[1].split(' ')[1].lstrip('{').rstrip('}').split(',')
##            else:
##                if row.split('=')[1].split(' ')[1].isnumeric():  # Only works for ints....
##                    d_int[row.split('=')[0].split(' ')[-2]] = int(row.split('=')[1].split(' ')[1])
##                else:
##                    d_int[row.split('=')[0].split(' ')[-2]] = row.split('=')[1].split(' ')[1]
##
##
##
