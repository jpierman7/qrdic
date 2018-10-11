# Repository for Generating and Reading DIC images using qr codes.

## Required Libraries:

[opencv](https://github.com/opencv/opencv)

[zbar](https://github.com/ZBar/ZBar)

[qrcodegen](https://www.nayuki.io/page/qr-code-generator-library)


## To build executable:
```
mkdir build
cd build
cmake ..
make
```

## Generating qr image

Command line input:
```
./genqr <image base name> <code stride length> <polynomial function input>
```

Output QR format:
```
"<image base name>-<row> | <column>"
```


## Decoding qr image
Command line input:
```
./decode <image filename>
```
Right now the decoder is NOT ROBUST. It won't read qr objects that are rotated or deformed more than a miniscule amount. I'm working on it.
## Things to quantify
- QR code robustness to warping
- Density & distribution of qr codes vs localization ability and accuracy
- Target accuracy
- QR robustness to noise & max likely resolution from current pattern application techniques
