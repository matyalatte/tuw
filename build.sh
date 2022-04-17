cmake -S . -B build
cd build
cmake -DCMAKE_BUILD_TYPE=Release
cmake --build .
cd ..
mv build/SimpleCommandRunner ./
rm -rf build