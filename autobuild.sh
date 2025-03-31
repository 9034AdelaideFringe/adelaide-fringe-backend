#!/bin/bash

if [ "$(id -u)" -ne 0 ]; then
    echo "Please run the script as sudo or root"
    exit 1
fi

sudo apt upgrade

sudo apt install libasio-dev -y
sudo apt install libpqxx-dev -y

sudo apt install gcc -y
sudo apt install make -y
sudo apt install cmake -y

if dpkg -l | grep -q "crow"; then
    echo "crow installed"
else   
    echo "crow not installed"
    echo "starting to install..."
    curl -O -L https://github.com/CrowCpp/Crow/releases/download/v1.2.1.2/Crow-1.2.1-Linux.deb || {
        echo "download crow failed"
        echo "exiting"
        exit 1
    }
    apt install "./Crow-1.2.1-Linux.deb" -y
    rm ./Crow-1.2.1-Linux.deb
fi



if [ -d "./build" ]; then
    rm -r ./build
fi  

cmake -B build && cmake --build build


echo "build successful"