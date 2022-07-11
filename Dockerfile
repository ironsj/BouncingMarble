FROM ironsj/ubuntu-with-curses

WORKDIR /app

COPY . .

RUN gcc marblec.c -o marble -lcurses

CMD ["./marble", "7", "5", "-2", "-2"]