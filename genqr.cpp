#include "binaryDistortion.hpp"
using namespace std;
using namespace cv;

int main ( int argc, char** argv )
{
	stringstream base_id;
	base_id << argv[1];
    // Mat qr_code = generate_qr_image ( argv[1] );
	int stride  = atoi ( argv[2] );
	
	// Mat random_pic = generate_random_binary (200, 200 );
	Mat random_pic ( 200, 200, CV_8UC1, Scalar(255) );
    cout << "made the thing" << endl;

	// Resize image;
	resize ( random_pic, random_pic, Size(500,500), INTER_NEAREST);
	// namedWindow ( "Raw image", 	WINDOW_NORMAL );
	// imshow ( "Raw image", qr_code );


	// Splice QR code into image
	plant_qr_code ( random_pic, random_pic, base_id.str(), stride );
	cout << "spliced the thing" << endl;

	namedWindow( "before fucking with it", WINDOW_NORMAL );
	imshow ( "before fucking with it", random_pic );
	waitKey(0);

	// ark length test
	vector < float > y;
	y.push_back (1); y.push_back ( -4 ); y.push_back ( 0 );y.push_back ( 0 ); y.push_back ( 0 ); y.push_back ( 0 );
	Mat placeholder;
	warp_image ( random_pic, placeholder, y );

	namedWindow ( "after fucking with it", WINDOW_NORMAL );
	imshow ( "after fucking with it", placeholder );
	waitKey(0);


    return 0;
}