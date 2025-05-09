#!/usr/bin/env bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
TOP_DIR=$(realpath "$SCRIPT_DIR/..")

IDF_PATH="$TOP_DIR"/esp-idf
C_CORE_PATH="$TOP_DIR"/components/c-core

echo "Cleaning..."
rm -rf build managed_components "$IDF_PATH" "$C_CORE_PATH"
rm -f sdkconfig
echo "Done."
