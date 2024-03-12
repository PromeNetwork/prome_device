#!/bin/bash

echo "compiling bulletproof ..."

TOP_BUILD_PATH=$1
BP_BUILD_DIR=$TOP_BUILD_PATH/src/bplib
BP_SRC_DIR=$2
LIB_NAME=$3
SW_VERSION=$4

OUTFILE=${LIB_NAME}.so

if [ ! -d $BP_BUILD_DIR ]; then
	mkdir -p $BP_BUILD_DIR
fi

if [ ! -d $BP_INC_DIR ]; then
	mkdir -p $BP_INC_DIR
fi

cd $BP_BUILD_DIR

go build -buildmode=c-shared -o ${OUTFILE} $BP_SRC_DIR/*.go

mv ${OUTFILE} ${OUTFILE}.${SW_VERSION}
ln -s ${OUTFILE}.${SW_VERSION} ${OUTFILE}.1 
ln -s ${OUTFILE}.1 ${OUTFILE} 
cp ${LIB_NAME}.h $5
cp -r -d $BP_BUILD_DIR/${OUTFILE} $TOP_BUILD_PATH/lib
cp -r -d $BP_BUILD_DIR/${OUTFILE}.1 $TOP_BUILD_PATH/lib
cp -r -d $BP_BUILD_DIR/${OUTFILE}.${SW_VERSION} $TOP_BUILD_PATH/lib


echo "compile bulletproof ok."