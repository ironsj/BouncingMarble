FROM ubuntu:latest

WORKDIR /app

COPY . .

RUN apt-get update && apt -y install build-essential && apt-get install libncurses-dev -y

ENV TERM=xterm

RUN gcc marblec.c -o marble -lcurses

CMD ["./marble", "4", "4", "-1", "2"]