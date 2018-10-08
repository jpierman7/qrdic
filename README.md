# Repository for Generating and Reading DIC images using qr codes.

## Required Libraries:
opencv
zbar
qrcodegen (https://www.nayuki.io/page/qr-code-generator-library)


## To build executable:
'''
mkdir build
cd build
cmake ..
make
'''

## Generating qr image

Command line input:
'''
./genqr <qr code base name> <code stride length>
'''

Output QR format:

"<qr code base name> <row> | <column>"
