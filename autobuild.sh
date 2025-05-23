#!/bin/bash

set -e

if [ "$(id -u)" -ne 0 ]; then
    echo "Please run the script as sudo or root"
    exit 1
fi

# Detect OS
if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS_ID=$ID
else
    echo "Unsupported system: cannot detect OS"
    exit 1
fi

echo "Detected OS: $OS_ID"

if [[ "$OS_ID" == "ubuntu" || "$OS_ID" == "debian" ]]; then
    apt update -y
    apt install -y libasio-dev libpqxx-dev g++ cmake curl uuid-dev make

    # Install Crow if not installed
    if dpkg -l | grep -q "crow"; then
        echo "Crow is already installed."
    else
        echo "Installing Crow..."
        curl -O -L https://github.com/CrowCpp/Crow/releases/download/v1.2.1.2/Crow-1.2.1-Linux.deb || {
            echo "Download Crow failed. Exiting."
            exit 1
        }
        apt install ./Crow-1.2.1-Linux.deb -y
        rm ./Crow-1.2.1-Linux.deb
    fi

elif [[ "$OS_ID" == "alinux" || "$OS_ID" == "alibaba" || "$OS_ID" == "centos" || "$OS_ID" == "rhel" ]]; then
    yum update -y
    yum install -y epel-release
    yum install -y asio-devel libpqxx-devel g++ cmake curl libuuid-devel make

    # No prebuilt Crow package, recommend using source
    if [ ! -d "./crow" ]; then
        echo "Installing Crow from source..."
        git clone https://github.com/CrowCpp/Crow.git
        mkdir -p Crow/build && cd Crow/build
        cmake ..
        make -j
        make install
        cd ../../
    else
        echo "Crow source already cloned."
    fi
else
    echo "Unsupported OS: $OS_ID"
    exit 1
fi

# Build your project
if [ -d "./build" ]; then
    rm -r ./build
fi  

cmake -B build && cmake --build build

echo "Build successful."
