#!/bin/bash

##############################################################################
# Linux libobs plugin build function
##############################################################################
#
# This script file can be included in build scripts for Linux or run directly
#
##############################################################################

# Halt on errors
set -eE

package_obs_plugin() {
    status "Package OBS plugin ${PRODUCT_NAME}"
    trap "caught_error 'package_obs_plugin'" ERR

    ensure_dir "${CHECKOUT_DIR}"

    step "Package ${PRODUCT_NAME}..."

    cmake -S . -B ${BUILD_DIR} -DOBS_SOURCE_DIR="../obs-studio" -DOBS_PLUGIN_LINUX_PACK_GENERATOR="DEB" -G Ninja ${CMAKE_CCACHE_OPTIONS} ${QUIET:+-Wno-deprecated -Wno-dev --log-level=ERROR}
    
    cmake --build ${BUILD_DIR} -t package

    mv -f "${CHECKOUT_DIR}/${BUILD_DIR}/${ORIGINAL_FILE_NAME}.deb" "${CHECKOUT_DIR}/${BUILD_DIR}/${FILE_NAME}.deb"

    cmake -S . -B ${BUILD_DIR} -DOBS_SOURCE_DIR="../obs-studio" -DOBS_PLUGIN_LINUX_PACK_GENERATOR="TGZ" -G Ninja ${CMAKE_CCACHE_OPTIONS} ${QUIET:+-Wno-deprecated -Wno-dev --log-level=ERROR}
    
    cmake --build ${BUILD_DIR} -t package

    mv -f "${CHECKOUT_DIR}/${BUILD_DIR}/${ORIGINAL_FILE_NAME}.tar.gz" "${CHECKOUT_DIR}/${BUILD_DIR}/${FILE_NAME}.tar.gz"

}

package-plugin-standalone() {
    CHECKOUT_DIR="$(git rev-parse --show-toplevel)"
    if [ -f "${CHECKOUT_DIR}/CI/include/build_environment.sh" ]; then
        source "${CHECKOUT_DIR}/CI/include/build_environment.sh"
    fi
    PRODUCT_NAME="${PRODUCT_NAME:-obs-plugin}"
    source "${CHECKOUT_DIR}/CI/include/build_support.sh"
    source "${CHECKOUT_DIR}/CI/include/build_support_linux.sh"

    GIT_BRANCH=$(git rev-parse --abbrev-ref HEAD)
    GIT_HASH=$(git rev-parse --short HEAD)
    GIT_TAG=$(git describe --tags --always --dirty=-dev)
    GIT_VERSION=$(echo ${GIT_TAG} | grep -Eos '[0-9]+.[0-9]+.[0-9]+(-[a-z0-9]+)+$')
    ORIGINAL_FILE_NAME="${PRODUCT_NAME}-${GIT_VERSION}-Linux"
    FILE_NAME="${PRODUCT_NAME}-${GIT_TAG}-linux"

    package_obs_plugin
}

print_usage() {
    echo -e "Usage: ${0}\n" \
            "-h, --help                     : Print this help\n" \
            "-q, --quiet                    : Suppress most build process output\n" \
            "-v, --verbose                  : Enable more verbose build process output\n" \
            "--build-dir                    : Specify alternative build directory (default: build)\n"
}

package-plugin-main() {
    if [ ! -n "${_RUN_OBS_BUILD_SCRIPT}" ]; then
        while true; do
            case "${1}" in
                -h | --help ) print_usage; exit 0 ;;
                -q | --quiet ) export QUIET=TRUE; shift ;;
                -v | --verbose ) export VERBOSE=TRUE; shift ;;
                --build-dir ) BUILD_DIR="${2}"; shift 2 ;;
                -- ) shift; break ;;
                * ) break ;;
            esac
        done

        package-plugin-standalone
    fi
}

package-plugin-main $*
