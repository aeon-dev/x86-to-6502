FROM ubuntu:18.04

MAINTAINER Robin Degen version: 1.0

ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && apt-get install -y tzdata && ln -fs /usr/share/zoneinfo/Europe/Amsterdam /etc/localtime && dpkg-reconfigure --frontend noninteractive tzdata

RUN apt-get install -y clang cmake git

ENV CC clang
ENV CXX clang++

COPY . /x86-to-6502

RUN cd /x86-to-6502 && ./bootstrap.sh
RUN cd /x86-to-6502/build && make -j2
RUN cd /x86-to-6502/build && ctest