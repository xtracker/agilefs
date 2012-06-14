#!/bin/bash

find . -name "*.h" -o -name "*.c" -o -name "*.cpp" > cscope.files
cscope -bq
