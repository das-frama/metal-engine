#!/bin/bash

set echo on

mkdir -p build
mkdir -p build/shaders/metal

c_files=./src/build_macos.m
metal_shaders=./src/shaders/metal/*.metal

echo "files: " $c_files

assembly="engine"
compiler_flags="-g -std=c17 -ObjC -fobjc-arc -Wno-deprecated-declarations"
include_flags="-Isrc"
linker_flags="-framework QuartzCore -framework Cocoa -framework Metal -framework MetalKit"

echo "Компилируем шейдеры..."
for file in $metal_shaders; do
    filename=$(basename "$file" .metal)
    xcrun -sdk macosx metal -c "$file" -o "build/shaders/metal/$filename.air"
done
xcrun -sdk macosx metallib build/shaders/metal/*.air -o build/shaders/metal/default.metallib
xxd -i build/shaders/metal/default.metallib > src/shaders/metal/metallib_data.h


echo "Собираем $assembly..."

set echo off
clang $c_files $compiler_flags -o ./build/$assembly $include_flags $linker_flags
