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


using namespace std;
using namespace zbar;
using namespace cv;

typedef struct
{
  string type;
  string data;
  vector <Point> location;
} decodedObject;



// Find and decode barcodes and QR codes
void decode(Mat &im, vector<decodedObject>&decodedObjects)
{
   
  // Create zbar scanner
  ImageScanner scanner;
 
  // Configure scanner
  scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
   
  // Convert image to grayscale
  Mat imGray;
  cvtColor(im, imGray,CV_BGR2GRAY);
 
  // Wrap image data in a zbar image
  Image image(im.cols, im.rows, "Y800", (uchar *)imGray.data, im.cols * im.rows);
 
  // Scan the image for barcodes and QRCodes
  int n = scanner.scan(image);
   
  // Print results
  for(Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
  {
    decodedObject obj;
     
    obj.type = symbol->get_type_name();
    obj.data = symbol->get_data();
     
    // Print type and data
    cout << "Type : " << obj.type << endl;
    cout << "Data : " << obj.data << endl << endl;
     
    // Obtain location
    for(int i = 0; i< symbol->get_location_size(); i++)
    {
      obj.location.push_back(Point(symbol->get_location_x(i),symbol->get_location_y(i)));
    }
     
    decodedObjects.push_back(obj);
  }
}