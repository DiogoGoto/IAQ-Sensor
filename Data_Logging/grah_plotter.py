import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import os 

try:
    os.mkdir("AIQ_Research")
except:
    pass

os.chdir("AIQ_Research")
UNITS = ["Samples", "Temperature (C)", "Humidity (%)", "Pressure (hPa)", "CO2 (PPM)", "Gas Resistivity (KOhms)", "PM 1.0 (ug/m3)", "PM 2.5 (ug/m3)", "PM 10 (ug/m3)"]

raw = np.array(pd.read_csv("Sensor_data.csv"))

data = {
        "Time": 0,
        "Temp": 0,
        "RH": 0,
        "Pres": 0,
        "CO2": 0,
        "Gas": 0,
        "PM1": 0,
        "PM25": 0, 
        "PM10" :0
}

for i, parameter in enumerate(data):
    data[parameter] = raw[::,i]


try:
    os.mkdir("Graphs")
except:
    pass

fig = plt.figure(figsize=(12,6))
for i, parameter in enumerate(data):
    if i == 0:
        continue
    plt.plot(range(len(data[parameter])),data[parameter])
    plt.xlabel("Samples")
    plt.ylabel(UNITS[i])
    plt.text(0,data[parameter][0],data["Time"][0],{"size": 8})
    plt.text(len(data[parameter])  ,data[parameter][-1],data["Time"][-1],{"size": 8})
    plt.title(f"{UNITS[i]}")
    plt.grid(True)
    plt.savefig(f"Graphs//{parameter}.png")
    plt.cla()