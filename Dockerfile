FROM ubuntu:18.04
LABEL auth="Baratelia_Timur_221_352"
ENV TZ=Europe/Moscow  
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN apt-get update && \
    apt-get install -y qt5-default qtbase5-dev qt5-qmake build-essential  
WORKDIR /Baratelia_Timur_221_352
COPY . .
RUN qmake MyServer.pro&&make
CMD ["./MyServer","param"]