#!/usr/bin/env bash

set -euo pipefail

function help() {
    echo "Usage: $0 MODE BUILD_DIR" 1>&2
    echo "  - MODE: -[l|b]" 1>&2
    echo "    - l: line coverage" 1>&2
    echo "    - b: branch coverage" 1>&2
    echo "  - BUILD_DIR: build directory containing gcda files" 1>&2
}

if [[ $# -ne 2 ]]; then
    echo "Invalid number of parameters" 1>&2; help; exit -1;
fi

case $1 in
-l) MODE="" ;;
-b) MODE="-b" ;;
*) echo "Invalid argument" 1>&2; help; exit -1;;
esac

BUILD_DIR=$2

BASEDIR=$(dirname "$0")
$BASEDIR/coverage_report.sh $MODE $BUILD_DIR | tail -n 2 | head -n 1 | sed 's/TOTAL .*\s\([0-9]\+\)%/\1/g'
