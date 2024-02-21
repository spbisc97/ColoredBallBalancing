import numpy as np
import matplotlib.pyplot as plt
from serial import Serial
import json

inputList = np.zeros(70)
setpointList = np.zeros(70)
integralPartList = np.zeros(70)
outputList = np.zeros(70)

connesso = False

while not connesso:
    try:
        arduinoData = Serial('COM5', 230400)
        connesso = True
    except Exception as e:
        print(f"Arduino non connesso: {e}")
        input("Collega Arduino e premi invio")
        connesso = False

fig, ax1 = plt.subplots()
ax2 = ax1.twinx()

ax1.set_ylim(0, 45)
ax2.set_ylim(-50, 50)

stringK = ""

try:
    for i in range(10):
        while arduinoData.inWaiting() == 0:
            pass
        arduinoString = arduinoData.readline().decode()
        print(arduinoString)
except Exception as e:
    print(f"Problem in read from serial: {e}")

while True:
    try:
        while arduinoData.inWaiting() == 0:
            pass
        arduinoString = arduinoData.readline().decode()
        print(arduinoString)

        loaded_json = json.loads(arduinoString)

        inputList = np.roll(inputList, -1)
        setpointList = np.roll(setpointList, -1)
        integralPartList = np.roll(integralPartList, -1)
        outputList = np.roll(outputList, -1)

        inputList[-1] = loaded_json["Input"]
        setpointList[-1] = loaded_json["Setpoint"]
        integralPartList[-1] = loaded_json["IntegralPart"] * 0.135
        outputList[-1] = loaded_json["Output"] * 0.135

    except Exception as e:
        print(f"Parsing problem: {e}")

    try:
        ax1.clear()
        ax2.clear()

        ax1.set_ylim(0, 45)
        ax2.set_ylim(-50, 50)

        stringK = f"KP: {loaded_json['kp']} KI: {loaded_json['ki']} KD: {loaded_json['kd']}"

        ax1.set_title(f"Poisition {stringK} Output")
        ax1.grid(True)
        ax1.plot(setpointList, 'r', label='setpoint')
        ax1.plot(inputList, 'b', label='input')
        ax1.legend(loc='lower left')

        ax2.plot(integralPartList, 'g', label='Integral')
        ax2.plot(outputList, 'y', label='Output')
        ax2.legend(loc='lower right')

        plt.pause(0.0001)

    except Exception as e:
        print(f"Draw+list problem: {e}")
