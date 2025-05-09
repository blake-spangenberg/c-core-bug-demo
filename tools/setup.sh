#!/usr/bin/env bash
set -e

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
TOP_DIR=$(realpath "$SCRIPT_DIR/..")

IDF_PATH="$TOP_DIR"/esp-idf
C_CORE_PATH="$TOP_DIR"/components/c-core

C_CORE_REMOTE=https://github.com/blake-spangenberg/c-core.git

ESP_IDF_VER=v5.4.1
C_CORE_VER=fix-esp-idf-compilation

echo "Updating packages..."
sudo apt-get -qq -y -u update
sudo apt-get -qq -y install git wget flex bison gperf python3 python3-pip python3-venv cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0

echo "Cleaning..."
rm -rf build managed_components
rm -f sdkconfig

if [ ! -d "$IDF_PATH" ]; then
    echo "Cloning ESP-IDF..."
    cd "$(dirname $IDF_PATH)"
    git clone https://github.com/espressif/esp-idf.git "$(basename $IDF_PATH)"
fi

echo "Checking out $ESP_IDF_VER..."
cd "$IDF_PATH"
git fetch --quiet --tags -f
git checkout --quiet -f "$ESP_IDF_VER"
git reset --quiet --hard HEAD
git clean --quiet -fdx
git submodule update --force --quiet --progress --init --recursive
git submodule foreach --quiet --recursive git clean --quiet -fdx
git submodule foreach --quiet --recursive git reset --quiet --hard

echo "Installing ESP-IDF..."
./install.sh esp32

if [ ! -d "$C_CORE_PATH" ]; then
    echo "Cloning c-core..."
    cd "$(dirname $C_CORE_PATH)"
    git clone $C_CORE_REMOTE "$(basename $C_CORE_PATH)"
fi

cd "$C_CORE_PATH"
git fetch --quiet --tags -f
git checkout --quiet -f "$C_CORE_VER"
git reset --quiet --hard HEAD
git clean --quiet -fdx
git submodule update --force --quiet --progress --init --recursive
git submodule foreach --quiet --recursive git clean --quiet -fdx
git submodule foreach --quiet --recursive git reset --quiet --hard

echo ""
echo "Setup Done"
