#!/bin/bash

##############################################################################
# Linux dependency management function
##############################################################################
#
# This script file can be included in build scripts for Linux or run directly
#
##############################################################################

# Halt on errors
set -eE

install_obs-studio() {
    if [ -n "${OBS_BRANCH}" ]; then
        CHECKOUT_REF="${OBS_BRANCH}"
    else
        #CHECKOUT_REF="tags/${OBS_VERSION:-${CI_OBS_VERSION}}"
        CHECKOUT_REF="tags/${OBS_VERSION}"
    fi

    ensure_dir "${OBS_BUILD_DIR}"

    if [ ! -d "${OBS_BUILD_DIR}/.git" ]; then
        git clone --recursive https://github.com/obsproject/obs-studio "$(pwd)"
        git fetch origin --tags
        git checkout ${CHECKOUT_REF} -b obs-plugin-build
    else
        if ! git show-ref --verify --quiet refs/heads/obs-plugin-build; then
            git checkout ${CHECKOUT_REF} -b obs-plugin-build
        else
            git checkout obs-plugin-build
        fi
    fi
}

install_build-deps() {
    shift
    status "Install OBS build dependencies"
    trap "caught_error 'install_build-deps'" ERR

    sudo apt-get install -y $@
}

install_obs-deps() {
    shift
    status "Install OBS dependencies"
    trap "caught_error 'install_obs-deps'" ERR

    #if [ -z "${DISABLE_PIPEWIRE}" ]; then
    #    sudo apt-get install -y $@ libpipewire-0.3-dev
    #else
    sudo apt-get install -y $@
    #fi
}

install_qt5-deps() {
    shift
    status "Install Qt5 dependencies"
    trap "caught_error 'install_qt5-deps'" ERR

    sudo apt-get install -y $@
}

install_qt6-deps() {
    shift
    status "Install Qt6 dependencies"
    trap "caught_error 'install_qt6-deps'" ERR

    _QT6_AVAILABLE="$(sudo apt-cache madison ${1})"
    if [ "${_QT6_AVAILABLE}" ]; then
        sudo apt-get install -y $@
    fi
}

install_dependencies() {
    status "Set up apt"
    trap "caught_error 'install_dependencies'" ERR

    BUILD_DEPS=(
        "build-deps cmake ninja-build pkg-config clang clang-format build-essential curl ccache"
        "obs-deps libavcodec-dev libavdevice-dev libavfilter-dev libavformat-dev libavutil-dev libswresample-dev \
         libswscale-dev libx264-dev libcurl4-openssl-dev libmbedtls-dev libgl1-mesa-dev libjansson-dev \
         libluajit-5.1-dev python3-dev libx11-dev libxcb-randr0-dev libxcb-shm0-dev libxcb-xinerama0-dev \
         libxcb-composite0-dev libxinerama-dev libxcb1-dev libx11-xcb-dev libxcb-xfixes0-dev swig libcmocka-dev \
         libpci-dev libxss-dev libglvnd-dev libgles2-mesa libgles2-mesa-dev libwayland-dev libxkbcommon-dev"
        "qt5-deps qtbase5-dev qtbase5-private-dev libqt5svg5-dev qtwayland5"
        "qt6-deps qt6-base-dev qt6-base-private-dev libqt6svg6-dev qt6-wayland"
    )

    sudo dpkg --add-architecture amd64
    sudo apt-get -qq update

    for DEPENDENCY in "${BUILD_DEPS[@]}"; do
        set -- ${DEPENDENCY}
        trap "caught_error ${DEPENDENCY}" ERR
        FUNC_NAME="install_${1}"
        ${FUNC_NAME} ${@}
    done
}

install-dependencies-standalone() {
    CHECKOUT_DIR="$(git rev-parse --show-toplevel)"
    if [ -f "${CHECKOUT_DIR}/CI/include/build_environment.sh" ]; then
        source "${CHECKOUT_DIR}/CI/include/build_environment.sh"
    fi
    PRODUCT_NAME="${PRODUCT_NAME:-obs-plugin}"
    DEPS_BUILD_DIR="${CHECKOUT_DIR}/../obs-build-dependencies"
    OBS_BUILD_DIR="${CHECKOUT_DIR}/../obs-studio"
    source "${CHECKOUT_DIR}/CI/include/build_support.sh"
    source "${CHECKOUT_DIR}/CI/include/build_support_linux.sh"

    status "Setting up plugin build dependencies"
    install_dependencies
}

print_usage() {
    echo -e "Usage: ${0}\n" \
            "-h, --help                     : Print this help\n" \
            "-q, --quiet                    : Suppress most build process output\n" \
            "-v, --verbose                  : Enable more verbose build process output\n"
}

install-dependencies-main() {
    if [ -z "${_RUN_OBS_BUILD_SCRIPT}" ]; then
        while true; do
            case "${1}" in
                -h | --help ) print_usage; exit 0 ;;
                -q | --quiet ) export QUIET=TRUE; shift ;;
                -v | --verbose ) export VERBOSE=TRUE; shift ;;
                -- ) shift; break ;;
                * ) break ;;
            esac
        done

        install-dependencies-standalone
    fi
}

install-dependencies-main $*
