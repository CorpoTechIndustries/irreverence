set -e

cd build
cmake --build . -j8

cd ../game
./bin/irreverence
