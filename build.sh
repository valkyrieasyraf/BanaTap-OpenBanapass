#!/bin/bash
# Compile and copy to the dist directory

set -e

# Clean and rebuild
rm -rf build
meson setup build
ninja -C build

# Create output directory
mkdir -p dist/w6w/3.05.03
mkdir -p dist/w5p/3.05.00

# Copy DLL
cp build/bngrw_w6w_30503.dll dist/w6w/3.05.03/bngrw.dll
cp build/bngrw_w5p_30500.dll dist/w5p/3.05.00/bngrw.dll

echo "Build complete!"
echo "  dist/w6w/3.05.03/bngrw.dll"
echo "  dist/w5p/3.05.00/bngrw.dll"