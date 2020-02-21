import serial
#import numpy
import matplotlib.pyplot as plt
from drawnow import *
import json
from tkinter import *
from tkinter.ttk import *

inputList=[] #time array
setpointList=[] # valori array
integralPartList=[]
outputList=[]
connesso=False

while(not connesso):
    try:
        arduinoData = serial.Serial('/dev/ttyACM0',230400)
        connesso=True
    except:
        print("Arduino non connesso")
        input("collega arduino e premi invio")
        connesso=False
 
    
        
        
plt.ion() #interactive mode
#plt.show()

cnt=0


def makeFig():
    plt.figure(1)
    plt.ylim(0,45)
    stringK = "KP :"+ str(loaded_json["kp"]) + " KI :"+str(loaded_json["ki"])+ " KD :"+ str(loaded_json["kd"])
    plt.title("Poisition                 "+stringK+"                   Output")
    plt.grid(True)
    plt.plot(setpointList,'r',label='setpoint')
    plt.plot(inputList,'b',label='input')
    plt.legend(loc='lower left')
    
    plt2=plt.twinx()
    plt.ylim(-50,50)
    #plt2.grid(True)
    plt2.plot(integralPartList,'g',label='Integral')
    plt2.plot(outputList,'y',label='Output')
    plt2.legend(loc='lower right')
    
    #plt2=plt.twinx()
    #plt2.plot(outputList,'y',label='Output')
    #plt2.set_ylabel("OutPut")
    #plt2.legend(loc='upper right')





try:   
    for i in range(10):
        while(arduinoData.inWaiting()==0):
            pass
        arduinoString = (arduinoData.readline()).decode()
        print(arduinoString)
except:
    print("problem in read from serial")
    pass




while True: 
    try:
        while(arduinoData.inWaiting()==0):
            pass
        arduinoString = (arduinoData.readline()).decode()
        
        print(arduinoString)
    except:
        print("problem in read from serial")
        pass
    try:
        loaded_json = json.loads(arduinoString)

        
        inputList.append(loaded_json["Input"])
        setpointList.append(loaded_json["Setpoint"])
        integralPartList.append((loaded_json["IntegralPart"])*0.135)
        outputList.append(loaded_json["Output"]*0.135)
        
    except:
        print("parsing problem")
        pass
    try:
        drawnow(makeFig)
        #print(stringK)

        #ro = Tk()
        #w = Label(ro, text="Hello world")
        #w.pack()
        #ro.mainloop()
        
        
        plt.pause(.0001)
        cnt=cnt+1
        if(cnt>70):  #15 secondi di storia
            inputList.pop(0)
            setpointList.pop(0)
            integralPartList.pop(0)
            outputList.pop(0)
    except:
        print("draw+list problem")
        pass
    #http://www.toptechboy.com/using-python-with-arduino-lessons/
