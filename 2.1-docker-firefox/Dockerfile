##############################
# Dockerfile to run Firefox  #
# Based on Ubuntu            #
##############################

FROM ubuntu:20.04
RUN apt update
RUN apt install firefox -y

RUN firefox -P -no-remote

LABEL \
      name="Firefox" \
      description="Launch Firefox via Docker" \
      version="1.0" \
      maintainer="dm...kov@gmail.com"