#!/bin/bash

set -e # fail on error

echo "SETUP"
SCRIPT_PATH=$(readlink -f "$0")
BASE_DIR=$(dirname "$SCRIPT_PATH")
BUILD_DIR="$BASE_DIR/build"


echo "script path $SCRIPT_PATH"
echo "base dir:  $BASE_DIR"
echo "BUILD DIR $BUILD_DIR"


echo "BUILDING"
if [ ! -d "$BUILD_DIR" ]; then
    echo "Build dir doesn't exist, creating one now"
    mkdir -p "$BUILD_DIR"
fi

cd "$BUILD_DIR"

echo "Home vcpkg installation"
export CMAKE_TOOLCHAIN_FILE="$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake"
export DEVKIT_LOCATION="$HOME/devkitBase"

## build isntructions
cmake -G "Unix Makefiles" -DDEVKIT_LOCATION="$HOME/devkitBase" "$BASE_DIR"
make -j$(nproc)

## run tests
# Move to the project's base directory to avoid issues with relative paths in the tests
cd $BASE_DIR

echo
echo ------------------
echo    Unit Tests
echo ------------------
echo Unit tests PKG folder
TEST_EXEC="$BUILD_DIR/pkg/retargeting/RetargetingTests"
"$TEST_EXEC"


echo
echo ------------------
echo   Build Success
echo ------------------
