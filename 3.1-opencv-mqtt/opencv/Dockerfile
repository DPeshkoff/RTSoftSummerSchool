##############################
# Dockerfile to run opencv   #
# Based on Ubuntu & gcc      #
##############################

FROM gcc:latest as build

RUN   apt-get update && \
      apt-get install cmake g++ git -y && \
      apt-get install libopencv-dev -y && \
      apt-get install libmosquitto-dev -y 

ADD ./src /app/src

ADD ./examples /app/examples

ADD ./dependencies /app/dependencies

WORKDIR /app/build

RUN cmake ../src && \
    cmake --build .

FROM ubuntu:20.04

RUN groupadd -r common && useradd -r -g common common
USER common

WORKDIR /app

COPY --from=build /app/build/hw .

ENTRYPOINT ["./hw", "./examples/01-rtsoft-example.mov"]

LABEL \
      name="Opencv-Contrast" \
      description="Launch Opencv-contrast" \
      version="1.0" \
      maintainer="dm...kov@gmail.com"