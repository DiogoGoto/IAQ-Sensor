from flask import Flask, request, jsonify
from boto3.dynamodb.conditions import Attr
from flask import Flask, request, jsonify
import boto3
from boto3.dynamodb.conditions import Key, Attr
import datetime
from flask_cors import CORS  # Import the CORS extension
import pandas as pd

from cosmosdb import *

app = Flask(__name__)
CORS(app)  # Add this line to enable CORS for your app

@app.route("/get_data_now", methods=["POST"])
async def get_data_now():
    data = request.json
    print(data)
    #start_time = data["start_time"]
    #end_time = data["end_time"]
    sensor_id = int(data["sensor_id"])

    query = f'SELECT top 1 c.Body FROM c where c.Body.id = {sensor_id} order by c.Body.datetime DESC'
    #print(query)
    items = await cosmos_query(query)
    # with open('test2.txt','w') as tf:
    #     for item in items:
    #         tf.write(f"{str(item)}\n")
    for item in items:
       return jsonify(item['Body'])

@app.route("/get_data_timerange", methods=["POST"])
async def get_data_timerange():
    data = request.json
    print(data)
    start_time = data["start_time"]
    end_time = data["end_time"]
    sensor_id = int(data["sensor_id"])

    #query = 'SELECT top 1 * FROM c order by c.Body.datetime DESC'
    query = f'SELECT VALUE root FROM (SELECT AVG(c.Body.CO2) FROM c where c.Body.datetime >= "{start_time}" and c.Body.datetime <= "{end_time}" and c.Body.id={sensor_id}) as root'
    print(query)
    items = await cosmos_query(query)

    for item in items:
       return jsonify(item['$1'])

@app.route("/get_data_download", methods=["POST"])
async def get_data_download():
    data = request.json
    print(data)
    start_time = data["start_time"]
    end_time = data["end_time"]
    sensor_id = int(data["sensor_id"])

    #query = 'SELECT top 1 * FROM c order by c.Body.datetime DESC'
    #SELECT VALUE root FROM (SELECT 
    query = f'SELECT c.Body FROM c where c.Body.datetime >= "{start_time}" and c.Body.datetime <= "{end_time}" and c.Body.id={sensor_id}'
    print(query)
    items = await cosmos_query(query)

    # fname = f'{start_time}_{end_time}_{sensor_id}.txt'
    # fname = fname.replace(":","-")
    # print(fname)
    # with open(fname,'w') as tf:
    #     for item in items:
    #         tf.write(f"{str(item)}\n")

    return items
    #return jsonify(sensor_id)

if __name__ == "__main__":
    app.run(debug=True)    