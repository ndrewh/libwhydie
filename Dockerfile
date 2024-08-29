FROM ubuntu:22.04

RUN apt update && apt install -y build-essential clang libunwind-dev
COPY . /work
WORKDIR /work
RUN make
