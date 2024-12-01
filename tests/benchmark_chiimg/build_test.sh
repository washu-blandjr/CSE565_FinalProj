#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$( cd "$SCRIPT_DIR/../.." && pwd )"

# Compile using paths relative to the project root
g++ -o "$SCRIPT_DIR/test_chiimg"     \
       "$SCRIPT_DIR/test_chiimg.cpp" \
       "$PROJECT_ROOT/chiimg.cpp"    \
       "$PROJECT_ROOT/conv2.cpp"     \
       `pkg-config --cflags --libs opencv4`;

echo "FINISHED COMPILING!"

start_time=$(date +%s)
./test_chiimg #> output.txt
end_time=$(date +%s)

# if diff output.txt golden.txt > /dev/null; then
#     echo "!Output matches golden file!"
# else
#     echo "XXX Output differs from golden file XXX"
#     diff output.txt golden
# fi

echo "!! FINISHED RUNNING THE TEST !!"
difference=$((end_time - start_time))
echo "Time difference: $difference seconds"
