#include "binaryDistortion.hpp"
using namespace std;
using namespace cv;

int main ( int argc, char** argv )
{
	stringstream base_id;
	base_id << argv[1];

	int stride  = atoi ( argv[2] );


	vector < float > polynomial_function;
	for ( int i = 3; i < argc; i++ )
	{
		polynomial_function.push_back ( atof ( argv[i] ) );
		cout << polynomial_function[i-2] << " ";
	}
	cout << endl;

	
	// Mat random_pic = generate_random_binary (200, 200 );
	Mat random_pic ( 200, 200, CV_8UC1, Scalar(255) );
    cout << "made the thing" << endl;

	// Resize image;
	resize ( random_pic, random_pic, Size(500,500), INTER_NEAREST);


	// Splice QR code into image
	plant_qr_code ( random_pic, random_pic, base_id.str(), stride );
	cout << "spliced the thing" << endl;

	// Save untampered image
	imwrite ( base_id.str() + "unmodified.png", random_pic );

	namedWindow( "before fucking with it", WINDOW_NORMAL );
	imshow ( "before fucking with it", random_pic );
	waitKey(0);

	Mat placeholder;
	warp_image ( random_pic, placeholder, polynomial_function );


	// Save warped image
	imwrite ( base_id.str() + "warped.png", placeholder );


	namedWindow ( "after fucking with it", WINDOW_NORMAL );
	imshow ( "after fucking with it", placeholder );
	waitKey(0);


    return 0;
}