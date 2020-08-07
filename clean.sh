#!/bin/bash

# http://redsymbol.net/articles/unofficial-bash-strict-mode/
set -euo pipefail
IFS=$'\n\t'

if [ -z "${VITASDK+x}" ]; then
    export VITASDK=/usr/local/vitasdk
    export PATH=$VITASDK/bin:$PATH
fi

make -C libvita2d clean