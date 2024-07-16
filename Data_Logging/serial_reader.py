import serial
import csv
from datetime import datetime
import os
import time
from math import isclose


try:
    os.mkdir("Data_Logging")
except:
    pass

os.chdir("Data_Logging")


FIELDNAMES = ["Time", "Temp", "RH", "Pres", "CO2", "VOC", "PM1", "PM25", "PM10"]
FIELDNAMES_C = ["Time", "Temp", "RH", "Pres", "CO2", "VOC", "PM1", "PM25", "PM10"]

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

row_C = {
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

try:
    with open("Communication_data.csv", "r") as file:
        reader = csv.DictReader(file, fieldnames=FIELDNAMES_C)
except:
    with open("Communication_data.csv", "w") as file:
        writer = csv.DictWriter(file, fieldnames=FIELDNAMES_C)
        writer.writeheader()


SENSOR = serial.Serial("COM14", 115200) 
COMMUNICATION = serial.Serial("COM11", 115200)
SENSOR.timeout = 3
COMMUNICATION.timeout = 3

data_S = ''
data_C = ''
FLAG_S = False
FLAG_C = False

while(1):
    #T,RH,P,CO2,VOCs,PM1.0,PM2.5,PM10
    raw_S = str(SENSOR.readline())
    raw_C = COMMUNICATION.readlines()
    if len(raw_C) > 6:
        try:
            data_C = raw_C[0].decode("utf-8").split(",")[:-1]
            data_C = [i[i.find('=')+1:] for i in data_C]

            for i, parameter in enumerate(data_C):
                row_C[FIELDNAMES_C[i+1]] = data_C[i]
            row_C["Time"] = datetime.now().strftime("%Y_%m_%d - %I:%M:%S %p")
            
            with open("Communication_data.csv", "a") as file:
                writer = csv.DictWriter(file, fieldnames=FIELDNAMES_C)
                writer.writerow(row_C)
            FLAG_C = True
        except:
            print(raw_C)


    if ',' in raw_S:
        try:
            data_S = raw_S[2:-1].split(",")
            for i, parameter in enumerate(data_S):
                row[FIELDNAMES[i]] = data_S[i]

            row["Time"] = datetime.now().strftime("%Y_%m_%d - %I:%M:%S %p")

            with open("Sensor_data.csv", "a") as file:
                writer = csv.DictWriter(file, fieldnames=FIELDNAMES)
                writer.writerow(row)
            FLAG_S = True
        except:
            print(raw_S)

    if FLAG_S and FLAG_C:
        for parameter in FIELDNAMES_C[1:]:
            try:
                if not isclose(float(row[parameter]), float(row_C[parameter]), rel_tol=0.01):
                    print(f"Data Mismatch: {parameter}")
                    print(f"Sensor Data: {row[parameter]}")
                    print(f"Communication Data: {row_C[parameter]}")
                    print("\n")
                    with open("Mismatch.csv", "a") as file:
                        writer = csv.writer(file)
                        writer.writerow([parameter, row[parameter], row_C[parameter], datetime.now().strftime("%Y_%m_%d - %I:%M:%S %p")])
                #print("Data Logged")
            except:
                print(f'Error in {parameter}: Sensor = {row[parameter]=}, Communication =  {row_C[parameter]=}')
        FLAG_S = False
        FLAG_C = False
    

SENSOR.close()



