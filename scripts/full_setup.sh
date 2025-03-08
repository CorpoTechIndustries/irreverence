set -e

if [ -d build ]; then
	rm -r build
fi

mkdir build -p
cd build
cmake ..
