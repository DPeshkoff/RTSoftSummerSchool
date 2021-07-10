##############################
# Dockerfile to run mqtt     #
# Based on ununtu            #
##############################

FROM ubuntu:20.04

RUN apt-get update && \
    apt-get install software-properties-common -y && \
    apt-add-repository ppa:mosquitto-dev/mosquitto-ppa && \
    apt-get update && \
    apt-get install libmosquitto-dev mosquitto mosquitto-clients -y

EXPOSE 1883 9001
   
RUN mosquitto -d
    
#CMD mosquitto_sub -h localhost -t opencv

LABEL \
      name="MQTT Broker" \
      description="Launch MQTT Broker" \
      version="1.0" \
      maintainer="dm...kov@gmail.com"