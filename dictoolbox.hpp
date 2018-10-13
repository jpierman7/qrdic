#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>

#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <math.h>

#include "BitBuffer.hpp"
#include "QrCode.hpp"

#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/video/tracking.hpp"

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;


void match_features ( const Mat img_1, const Mat img_2, const int minFeatures )
{   
  Ptr<SURF> detector = SURF::create();
  detector->setHessianThreshold(minFeatures);
  std::vector<KeyPoint> keypoints_1, keypoints_2;
  Mat descriptors_1, descriptors_2;
  detector->detectAndCompute( img_1, Mat(), keypoints_1, descriptors_1 );
  detector->detectAndCompute( img_2, Mat(), keypoints_2, descriptors_2 );
  //-- Step 2: Matching descriptor vectors using FLANN matcher
  FlannBasedMatcher matcher;
  std::vector< DMatch > matches;
  matcher.match( descriptors_1, descriptors_2, matches );

  double max_dist = 0; double min_dist = 100;
  //-- Quick calculation of max and min distances between keypoints
  for( int i = 0; i < descriptors_1.rows; i++ )
  { double dist = matches[i].distance;
    if( dist < min_dist ) min_dist = dist;
    if( dist > max_dist ) max_dist = dist;
  }
  printf("-- Max dist : %f \n", max_dist );
  printf("-- Min dist : %f \n", min_dist );

  //-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist,
  //-- or a small arbitary value ( 0.02 ) in the event that min_dist is very
  //-- small)
  //-- PS.- radiusMatch can also be used here.
  std::vector< DMatch > good_matches;
  for( int i = 0; i < descriptors_1.rows; i++ )
  { if( matches[i].distance <= max(2*min_dist, 0.02) )
    { good_matches.push_back( matches[i]); }
  }
  //-- Draw only "good" matches
  Mat img_matches;
  drawMatches( img_1, keypoints_1, img_2, keypoints_2,
               good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
               vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
  //-- Show detected matches
  imshow( "Good Matches", img_matches );
  for( int i = 0; i < (int)good_matches.size(); i++ )
  { printf( "-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx ); }
  waitKey(0);


}

void optical_flow ( Mat &img_1, const Mat img_2 )
{
    Mat gray1, gray2;
    cvtColor ( img_1, gray1, CV_BGR2GRAY) ;
    cvtColor ( img_2, gray2, CV_BGR2GRAY );
    vector<uchar> status;
    vector<float> err;
    const int MAX_COUNT = 500;
    vector<Point2f> points[2];
    goodFeaturesToTrack(gray1, points[0], MAX_COUNT, 0.01, 10, Mat(), 3, 3, 0, 0.04);
    calcOpticalFlowPyrLK ( gray1, gray2, points[0], points[1], status, err );
    for ( int i = 0; i < points[0].size(); i++ )
    {
        arrowedLine(img_1,  points[0][i], points[1][i], Scalar(255,0,0), 2 );
    }

}

void dic ( const Mat im1, const Mat im2 )
{
    // Do DIC on im1 and im2
}

