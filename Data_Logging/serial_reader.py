import serial
import csv
from datetime import datetime
import os
import time


try:
    os.mkdir("Data_Logging")
except:
    pass

os.chdir("Data_Logging")


FIELDNAMES = ["Time", "Temp", "RH", "Pres", "CO2", "VOC", "PM1", "PM25", "PM10"]

row = {
    "Time": 0,
    "Temp": 0,
    "RH": 0,
    "Pres": 0,
    "CO2": 0,
    "VOC": 0,
    "PM1": 0,
    "PM25": 0, 
    "PM10" :0
}

try:
    with open("Sensor_data.csv", "r") as file:
        reader = csv.DictReader(file, fieldnames=FIELDNAMES)
except:
    with open("Sensor_data.csv", "w") as file:
        writer = csv.DictWriter(file, fieldnames=FIELDNAMES)
        writer.writeheader()


SENSOR = serial.Serial("COM11", 115200) 
SENSOR.timeout = 3

data = ''
while(1):
    
    raw = str(SENSOR.readline())   
    data = raw[2:-5].split(",")

    for i, parameter in enumerate(data):
        row[FIELDNAMES[i]] = data[i]

    row["Time"] = datetime.now().strftime("%Y_%m_%d - %I:%M:%S %p")

    with open("Sensor_data.csv", "a") as file:
        writer = csv.DictWriter(file, fieldnames=FIELDNAMES)
        writer.writerow(row)
    

SENSOR.close()



