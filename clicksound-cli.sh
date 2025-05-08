#!/bin/bash

if [ -L $0 ] ; then
    CORE_DIR=$(dirname $(readlink -f $0)) ;
else
    CORE_DIR=$(dirname $0) ;
fi

source ${CORE_DIR}/extras/log.lib
source ${CORE_DIR}/extras/color.lib

CURRENT_VERSION="V1.0.0"

help() {
    log.setline "Clicksound"
    echo -e "\nVersion : ${CURRENT_VERSION} \nhttps://github.com/temelyus/clicksound"
    echo -e "\nCommands:"
    log.sub "list                  # Listing pre installed sounds"
    log.sub "start <sound-file>    # Start/Switch sound"
    log.sub "stop                  # Stop system"
    log.sub "help                  # Show help and Credits"
    log.sub "update                # Check updates"
    log.endline
}
list() {
    ls ${CORE_DIR}/sounds/
}
start() {
    shift
    if [[ ! -f ${CORE_DIR}/sounds/${1} ]]; then
        log.error "File ${1} not found in path."
        log.sub "${CORE_DIR}/${1}"
        return 0
    fi
    kill $(ps -aux | grep clicksound | head -n1 | awk '{print $2}') &> /dev/null
    nohup ${CORE_DIR}/clicksound ${CORE_DIR}/sounds/${1} &> /dev/null &
    log.info "Theme: ${1}"
    return 0
}
stop() {
    (kill $(ps -aux | grep clicksound | head -n1 | awk '{print $2}') && log.done "Stoppped") || log.error "Something went wrong!"
}
update() {
    REPO_OWNER="temelyus"
    REPO_NAME="clicksound"

    # Validate dependencies
    if ! command -v curl >/dev/null 2>&1; then
    echo "Error: curl is required but not installed." >&2
    exit 1
    fi

    # Fetch the latest release tag from GitHub
    LATEST_VERSION=$(curl --silent "https://api.github.com/repos/${REPO_OWNER}/${REPO_NAME}/releases/latest" | 
                    grep '"tag_name":' | 
                    sed -E 's/.*"([^"]+)".*/\1/')

 
    if [ "$CURRENT_VERSION" != "$LATEST_VERSION" ]; then
        log.hint "🔔 New version available: $LATEST_VERSION."

    else
        log.hint "✅ You are up-to-date."
    fi

}
[[ -z $1 || $1 =~ (--h|-h) ]] && help && exit 0;
($1 ${@} && exit 0) || (help && exit 1);




