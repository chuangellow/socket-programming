FROM ubuntu:latest

LABEL maintainer="r11944040@ntu.edu.tw"

RUN apt-get update

RUN apt-get install -y build-essential

WORKDIR /usr/src/app

COPY . .

RUN gcc -o server server.c
RUN gcc -o client client.c

EXPOSE 8080

CMD ["./server"]
