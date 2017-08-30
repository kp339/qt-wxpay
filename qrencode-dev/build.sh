#!/bin/sh

HOST=arm-none-linux-gnueabi
CROSS_COMPILE=arm-none-linux-gnueabi-
PREFIX=${PWD}/install

if [ ! -d install ]; then
    mkdir install
fi

sync

ZLIB_PKG=`ls *zlib* | grep tar`
if [ $ZLIB_PKG == '' ]; then
    echo "qrencode packets no found"
    exit
fi

PNG_PKG=`ls *png* | grep tar`
if [ $PNG_PKG == '' ]; then
    echo "libpng packets no found"
    exit
fi

QRCODE_PKG=`ls *qrencode* | grep tar`
if [ $QRCODE_PKG == '' ]; then
    echo "qrencode packets no found"
    exit
fi

ZLIB_PKG_PATH=`echo $ZLIB_PKG | awk -F '.tar' '{print $1}'`
PNG_PKG_PATH=`echo $PNG_PKG | awk -F '.tar' '{print $1}'`
QRCODE_PKG_PATH=`echo $QRCODE_PKG | awk -F '.tar' '{print $1}'`

if [ -d $ZLIB_PKG_PATH ]; then
    rm $ZLIB_PKG_PATH -r
fi


if [ -d $PNG_PKG_PATH ]; then
    rm $PNG_PKG_PATH -r
fi

if [ -d $QRCODE_PKG_PATH ]; then
    rm $QRCODE_PKG_PATH -r
fi

sync
tar xf $ZLIB_PKG
tar xf $PNG_PKG
tar xf $QRCODE_PKG

sync
sleep 1

echo ""
echo ">>>>>>>>>>>>>>>> Start compile zlib"
cd $ZLIB_PKG_PATH
CC=${CROSS_COMPILE}gcc ./configure --prefix=${PWD}/../install/
sync
sleep 1
make
sync
sleep 1
make install
cd ..
if [ -f install/lib/libz.so ]; then
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>"
    echo "zlib compile success"
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>"
else
    echo "zlib compile failed"
    exit
fi
sync
sleep 1

echo ""
echo ">>>>>>>>>>>>>>>> Start compile libpng"
cd $PNG_PKG_PATH
cp ../install/include/*.h ./
./configure --prefix=${PWD}/../install/ --host=${HOST} CC=${CROSS_COMPILE}gcc CFLAGS=-I${PWD}/../install/include LDFLAGS=-L${PWD}/../install/lib
sync
sleep 1
make
sync
sleep 1
make install
cd ..
if [ -f install/lib/libpng.so ]; then
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>"
    echo "libpng compile success"
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>"
else
    echo "libpng compile failed"
    exit
fi
sleep 1
sync

echo ""
echo ">>>>>>>>>>>>>>>> Start compile qrencode"
cd $QRCODE_PKG_PATH
./configure --host=${HOST} --prefix=${PWD}/../install/ CC=${CROSS_COMPILE}gcc png_CFLAGS=-I${PWD}/../install/include png_LIBS=-L${PWD}/../install/lib LDFLAGS="-L${PWD}/../install/lib -lpng16 -lz" CFLAGS=-I${PWD}/../install/include
sync
sleep 1
make
sync
sleep 1
make install
cd ..
if [ -f install/lib/libqrencode.so ]; then
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>"
    echo "qrencode compile success"
    echo ">>>>>>>>>>>>>>>>>>>>>>>>>"
else
    echo "qrencode compile failed"
    exit
fi

mv install ./qrcode-dev
tar cf qrcode-dev.tar.gz qrcode-dev
mv qrcode-dev install
sync
echo "All have done, copy the output file qrcode-dev.tar.gz to your board."
