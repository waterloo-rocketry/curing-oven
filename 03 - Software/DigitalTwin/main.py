import tkinter as tk

import csv


def Serialprintln(s):  # Function to print to a tkinter window representing the serial port
    pass  # Print to the serial port tkinter window



class lcd():
    pass
    




def BACK():
    print('Back')


def UPW():
    print('Upwards')  # Need to find a clever way to represent the interrupts


def DWN():
    print('Downwards')


def SLT():
    print('Select')


def RUN():
    print('Run')

def ILBRK():
    print('ILBRK')



# Set up Curing Oven Display

root=tk.Tk()
root.title('Power Ranchers Curing Oven')
root.geometry("800x600")

l_lcd = tk.Label(root, text='Power Ranchers', relief='raised', height=4, width=20)
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


b_BACK = tk.Button(root, text="Back", command=BACK)
b_BACK.place(x=100, y=200)
b_UPW = tk.Button(root, text="UPW", command=UPW)
b_UPW.place(x=150, y=200) 
b_DWN = tk.Button(root, text="DWN", command=DWN)
b_DWN.place(x=200, y=200)
b_SLT = tk.Button(root, text="SLT", command=SLT)
b_SLT.place(x=250, y=200)
b_RUN = tk.Button(root, text="RUN", command=RUN)
b_RUN.place(x=300, y=200)

b_estop = tk.Button(root, text="E-Stop", command=ILBRK)
b_estop.place(x=415, y=115)

root.mainloop()

# Code import

# Sort text line by line

#   Variables - if the line begins with the keywords const int, int, etc

d_int = {}

d_floats = {}

d_bools = {}

d_str = {}

with open(r'C:\GrabCAD-ME481\04 - Oven Control Elements\03 - Software\DigitalTwin\OCE_Firmware.txt', newline='') as f_dt:

    for row in f_dt:
        
        if 'int' in row.split('=')[0] and 'Timer' not in row.lstrip() and '//' not in row and '=' in row and 'for' not in row:
            print('int')
            if '[]' in row:
                if row.split('=')[1].split(' ')[1].lstrip('{').rstrip('}').split(',')[0].isnumeric(): 
                    d_int[row.split('=')[0].split(' ')[-2]] = [int(a) for a in row.split('=')[1].split(' ')[1].lstrip('{').rstrip('}').split(',')]
                else:
                    d_int[row.split('=')[0].split(' ')[-2]] = row.split('=')[1].split(' ')[1].lstrip('{').rstrip('}').split(',')
            else:
                if row.split('=')[1].split(' ')[1].isnumeric():  # Only works for ints....
                    d_int[row.split('=')[0].split(' ')[-2]] = int(row.split('=')[1].split(' ')[1])
                else:
                    d_int[row.split('=')[0].split(' ')[-2]] = row.split('=')[1].split(' ')[1]



