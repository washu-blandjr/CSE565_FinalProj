#!/bin/bash
# g++ -o test_findpeaks test_findpeaks.cpp ../../findpeaks.cpp -std=c++17 -Wall -Wextra -O2

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$( cd "$SCRIPT_DIR/../.." && pwd )"

# Compile using paths relative to the project root
g++ -o "$SCRIPT_DIR/test_findpeaks"     \
       "$SCRIPT_DIR/test_findpeaks.cpp" \
       "$PROJECT_ROOT/findpeaks.cpp"    ;

./test_findpeaks > output.txt
if diff output.txt golden.txt > /dev/null; then
    echo "!Output matches golden file!"
else
    echo "XXX Output differs from golden file XXX"
    diff output.txt golden
fi

