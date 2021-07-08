##############################
# Dockerfile to run opencv   #
# Based on Ubuntu & gcc      #
##############################

FROM gcc:latest as build

RUN apt update && \
      apt install -y cmake g++ git && \
      apt install -y libopencv-dev

ADD ./src /app/src

ADD ./examples /app/examples

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