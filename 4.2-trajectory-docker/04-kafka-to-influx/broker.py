##############################
# kafka to influx connector  #
# Based on python            #
##############################

from influxdb import InfluxDBClient
from kafka import KafkaConsumer
import time
import json

kafka_consumer = KafkaConsumer( 
    'opencv',
     bootstrap_servers=['localhost:9092'],
     enable_auto_commit=True)


influx_client = InfluxDBClient(host='localhost', port=8086)

influx_client.create_database('opencv')

influx_client.switch_database('opencv')

fl = influx_client.query('Delete FROM opencv_m WHERE time > 0')


def main():
    for message in kafka_consumer:
            message = message.value.decode("utf-8") 
            msg_json = json.loads(message)
            json_body = [
            {
            "coordinates": "opencv_m",
            "fields":{
                "x": msg_json['x'],
                "y": msg_json['y'],
                }
            }
            ]

            print(json_body)
            flag = influx_client.write_points(json_body)
            print(flag)


if __name__ == '__main__':
    while (True):
        main()
        time.sleep(0.05)