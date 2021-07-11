##############################
# mqtt to kafka connector    #
# Based on python            #
##############################

import paho.mqtt.client as mqtt
from pykafka import KafkaClient

# Settings

mqtt_broker = "localhost"
mqtt_client = mqtt.Client("detector")

kafka_client = KafkaClient(hosts="localhost:9092")
kafka_topic = kafka_client.topics['opencv']


kafka_producer = kafka_topic.get_sync_producer()


def on_message(client, userdata, message):
    msg_payload = message.payload.decode("utf-8") 
    kafka_producer.produce(msg_payload.encode('ascii'))

def main():
    mqtt_client.connect(mqtt_broker)
    mqtt_client.subscribe('opencv')
    mqtt_client.on_message = on_message
    mqtt_client.loop_forever()


if __name__ == '__main__':
    main()
