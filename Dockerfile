FROM ubuntu:22.04

RUN apt install build-essential clang libunwind-dev
COPY . /work
WORKDIR /work
RUN make
