#!/bin/bash

cmake -B build -DCMAKE_PREFIX_PATH=/root/install/lib/cmake/SilKit
cmake --build build
