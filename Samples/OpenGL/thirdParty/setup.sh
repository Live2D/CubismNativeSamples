#!/usr/bin/env sh

set -ue

GLEW_VERSION=2.1.0
GLFW_VERSION=3.3

SETUP_DIR=$(cd $(dirname $0) && pwd)

cd $SETUP_DIR

##############
# Setup GLEW #
##############

cd $SETUP_DIR

# Download and extract the archive.
echo - Setup GLEW
echo Downloading...
curl -fsSL -o glew.zip https://github.com/nigels-com/glew/releases/download/glew-$GLEW_VERSION/glew-$GLEW_VERSION.zip
echo Extracting...
unzip -oq glew.zip
rm glew.zip

##############
# Setup GLFW #
##############

cd $SETUP_DIR

# Download and extract the archive.
echo - Setup GLFW
echo Downloading...
curl -fsSL -o glfw.zip https://github.com/glfw/glfw/releases/download/$GLFW_VERSION/glfw-$GLFW_VERSION.zip
echo Extracting...
unzip -oq glfw.zip
rm glfw.zip
