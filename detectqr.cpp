#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include<math.h>
#include<zbar.h>

#include "opencv2/opencv.hpp"
#include "binaryDistortion.hpp"
#include "dictoolbox.hpp"

using namespace std;
using namespace zbar;
using namespace cv;

typedef struct
{
  string type;
  string data;
  vector <Point> location;
  int corner [2];
} decodedObject;



// Find and decode barcodes and QR codes
void decode(const Mat im, vector<decodedObject>&decodedObjects)
{
   
  // Create zbar scanner
  ImageScanner scanner;
 
  // Configure scanner
  scanner.set_config(ZBAR_QRCODE, ZBAR_CFG_ENABLE, 1);
   
  // Convert image to grayscale
  Mat imGray;
  cvtColor(im, imGray, CV_BGR2GRAY);
 
  // Wrap image data in a zbar image
  Image image(im.cols, im.rows, "Y800", imGray.data, im.cols * im.rows);
 
  // Scan the image for barcodes and QRCodes
  int n = scanner.scan(image);
  

  if ( n == -1 )
  {
    cerr << "Error scanning!" << endl;
    exit ( EXIT_FAILURE );
  }
  else if ( n == 0 )
  {
    cerr << "No symbols found." << endl;
    exit ( EXIT_FAILURE );
  }
  else
  {
    cout << "finished scanning" << endl;
  }
  
  // Print results
  unsigned long datalength = image.get_data_length();
  // cout << " finding results from image of size: " << datalength << endl;
  for(Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
  {
    decodedObject obj;
     
    obj.type = symbol->get_type_name();
    obj.data = symbol->get_data();
     
    // // Print type and data
    // cout << "Type : " << obj.type << endl;
    // cout << "Data : " << obj.data << endl << endl;
     
    // Obtain location
    obj.corner[0] = symbol->get_location_x(0); obj.corner[1] = symbol->get_location_y(0);
    // cout << obj.corner[0] << " " << obj.corner[1] << endl;

     
    decodedObjects.push_back(obj);
  }
  cout << "number of objects found: " << decodedObjects.size() << endl;
}

void split_image ( Mat &image, const vector<decodedObject> codes )
{
  int height(22), width(22);
  // Point2d p1, p2;
  for ( int i = 0; i < codes.size(); i++ )
  {
    // p1 = Point2d ( codes[i].corner[0], codes[i].corner[1] );
    Rect rect ( codes[i].corner[0], codes[i].corner[1], height, width);
    rectangle ( image, rect, Scalar(0,0,255) );
  }
  imshow("split by qr", image);
}

int main ( int argc, char** argv )
{

  // Load files into memory as Mat image objects
  string filename1="", filename2="";
  filename1 += argv[1]; filename2+=argv[2];
  Mat im1 = imread ( filename1 ), im2 = imread ( filename2 ) ;
  

  // Decode available qr info
  vector<decodedObject> qrinfo1, qrinfo2 ;
  cout << "decoding image 1" << endl;
  decode ( im1, qrinfo1);
  split_image ( im1, qrinfo1 );

  // Determine Global displacement, segment image by individual qr code


// For each qr code match:
  // Pad images to be same dimensions
  Size s1 = im1.size();
  Size s2 = im2.size();
  
  int height (s2.height), width (s2.width);
  if (s1.height > s2.height)
  {
    height = s1.height;
  }
  if (s1.width > s2.width)
  {
    width = s1.width;
  }
  
  Mat temp1, temp2;
  copyMakeBorder ( im1, temp1, 0, height-s1.height, 0, width-s1.width, BORDER_CONSTANT, Scalar(255,255,255) );
  copyMakeBorder ( im2, temp2, 0, height-s2.height, 0, width-s2.width, BORDER_CONSTANT, Scalar(255,255,255) );

  // match_features ( temp1, temp2, 400 );


  // Determine local displacement
  Mat overlay;
  optical_flow ( temp1, temp2, overlay );
  cout << overlay.size << endl;
  imshow( "optical flow", overlay );
  // imshow("mutilated", temp2 );
  waitKey(0);


/* 
  At this point in time, there is no:
    -Global displacement 
    -Separation of individual qr codes
    -Robustness to noise/rotation
    -Validation method / quantification of results
*/


  return 0;

}