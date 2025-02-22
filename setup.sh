# Relies on GTest
# sudo apt install google-mock libgmock-dev libgtest-dev
#
#
rm -rf build

mkdir build && \
cd build

cmake -GNinja \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_INSTALL_PREFIX=bin \
	..

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .
cd ..
ln -s build/compile_commands.json

# Build targets from top project directory.
#cmake --build build

# Run CTest tests from top project directory.
#cmake --build build --target test
