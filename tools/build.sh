#!/usr/bin/env bash
set -e
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
TOP_DIR=$(realpath "$SCRIPT_DIR/..")

mkdir -p "$TOP_DIR"/build
source "$TOP_DIR"/esp-idf/export.sh && idf.py --ccache build
