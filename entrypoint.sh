#!/bin/bash
# RandBit
cd /Falcon
make all -j$(nproc)

# Execute any additional command you want after the build, for example, running a bash shell
exec "$@"
