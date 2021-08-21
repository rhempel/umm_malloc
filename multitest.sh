#!/bin/bash

# Iterate through the files in test/options and pass them to ceedling

for filename in test/options/*; do
    [ -e "$filename" ] || continue
    name=$(basename $filename)
    printf "# ---------------------------------------------\n"
    printf "# ceedling options:%s clean test\n" ${name%.yml}
    printf "# ---------------------------------------------\n"
    ceedling options:${name%.yml} clean test
done
