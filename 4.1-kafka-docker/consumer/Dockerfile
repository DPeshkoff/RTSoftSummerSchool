##############################
# Dockerfile to run consumer #
# Based on Ubuntu            #
##############################

FROM ubuntu:20.04

RUN   apt update && \
      apt-get install curl -y && \
      mkdir ~/Downloads && \
      curl "https://downloads.apache.org/kafka/2.6.2/kafka_2.13-2.6.2.tgz" -o ~/Downloads/kafka.tgz && \
      mkdir ~/kafka && cd ~/kafka && \
      tar -xvzf ~/Downloads/kafka.tgz --strip 1

COPY ./kafka-config/server.properties ~/kafka/config

COPY ./kafka-config/zookeeper.service ~/etc/systemd/system/

COPY ./kafka-config/kafka.service ~/etc/systemd/system/

RUN   mv ~/kafka /kafka

LABEL \
      name="Kafka Consumer" \
      description="Launch Kafka Consumer" \
      version="1.0" \
      maintainer="dm...kov@gmail.com"