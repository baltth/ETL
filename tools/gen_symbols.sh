#!/usr/bin/env bash

set -u

: ${CLANG_VERSION_POSTFIX:=}

SCRIPT_DIR=$(dirname $(realpath ${0}))
SOURCE_DIR=$(realpath ${SCRIPT_DIR}/..)

function help() {
    echo "Usage: ${0} BUILD_DIR [SYMBOLS_DIR]" 1>&2
    echo "  - BUILD_DIR: build directory containing the compile_commands.json file" 1>&2
    echo "  - SYMBOLS_DIR: Optional output directory" 1>&2
}

if [[ $# -ne 1 ]] && [[ $# -ne 2 ]]; then
    echo "Invalid number of parameters" 1>&2; help; exit -1;
fi

BUILD_DIR=${1}
if [[ $# -eq 2 ]]; then
    OUT_DIR=${2}
    mkdir -p ${OUT_DIR}
    OUT_DIR=$(realpath ${OUT_DIR})
else
    OUT_DIR=$(mktemp -d)
fi

FAS_EXEC="find-all-symbols${CLANG_VERSION_POSTFIX}"
which ${FAS_EXEC} > /dev/null
if [[ $? -ne 0 ]]; then
    echo "Unable to find ${FAS_EXEC}. Please install the tool or" 1>&2
    echo "specify the proper version with the CLANG_VERSION_POSTFIX variable." 1>&2
    echo "E.g. 'CLANG_VERSION_POSTFIX=-15' selects ind-all-symbols-15" 1>&2
    exit -1
fi

echo "Analyzing ..."

FILES=$(find ${SOURCE_DIR} -type f \( \
    -name '*.cpp' -or \
    -name '*.h' -or \
    -name '*.hpp' \) \
    -not -name 'info.cpp' \
    -not -path '*/build-*' \
    -not -path '*/tests/catch2*')

${FAS_EXEC} -p ${BUILD_DIR} ${FILES} --output-dir=${OUT_DIR}

echo ""
echo "Please find the results in ${OUT_DIR}"
