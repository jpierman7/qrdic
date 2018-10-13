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
	}
	
	// Mat random_pic = generate_random_binary (200, 200 );
	Mat random_pic ( 200, 200, CV_8UC1, Scalar(255) );
    cout << "made the thing" << endl;

	// Splice QR code into image
	plant_qr_code ( random_pic, random_pic, base_id.str(), stride );
	generate_qr_image ( random_pic, base_id.str() );

	// Resize image;
	resize ( random_pic, random_pic, Size(200,200), 0, 0, INTER_NEAREST);
	cout << "spliced the thing" << endl;

	// Save untampered image
	imwrite ( base_id.str() + "unmodified.jpg", random_pic );

	namedWindow( "before fucking with it", WINDOW_AUTOSIZE );
	imshow ( "before fucking with it", random_pic );
	waitKey(0);

	Mat placeholder;
	warp_image ( random_pic, placeholder, polynomial_function );


	// Save warped image
	imwrite ( base_id.str() + "warped.jpg", placeholder );


	namedWindow ( "after fucking with it", WINDOW_AUTOSIZE );
	imshow ( "after fucking with it", placeholder );
	waitKey(0);

	// write parameter text file
	write_parameter_file ( base_id.str() + ".txt", polynomial_function );

    return 0;
}