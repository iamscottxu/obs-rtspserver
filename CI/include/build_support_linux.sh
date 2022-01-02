#!/bin/bash

##############################################################################
# Linux support functions
##############################################################################
#
# This script file can be included in build scripts for Linux.
#
##############################################################################

# Setup build environment

# CI_OBS_VERSION=$(cat "${CI_WORKFLOW}" | sed -En "s/[ ]+OBS_VERSION: '([0-9\.]+)'/\1/p")

if [ "${TERM-}" -a -z "${CI}" ]; then
    COLOR_RED=$(tput setaf 1)
    COLOR_GREEN=$(tput setaf 2)
    COLOR_BLUE=$(tput setaf 4)
    COLOR_ORANGE=$(tput setaf 3)
    COLOR_RESET=$(tput sgr0)
else
    #COLOR_RED=""
    #COLOR_GREEN=""
    #COLOR_BLUE=""
    #COLOR_ORANGE=""
    #COLOR_RESET=""
    COLOR_RED$(echo -e '\033[31m')
    COLOR_GREEN=$(echo -e '\033[32m')
    COLOR_BLUE=$(echo -e '\033[34m')
    COLOR_ORANGE=$(echo -e '\033[33m')
    COLOR_RESET=$(echo -e '\033[0m')
fi

if [ "${CI}" -o "${QUIET}" ]; then
    export CURLCMD="curl --silent --show-error --location -O"
else
    export CURLCMD="curl --progress-bar --location --continue-at - -O"
fi
