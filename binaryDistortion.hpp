#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>

#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include<math.h>

#include "BitBuffer.hpp"
#include "QrCode.hpp"
#include "opencv2/opencv.hpp"


using namespace std;
using namespace cv;
using std::uint8_t;
using qrcodegen::QrCode;
using qrcodegen::QrSegment;

void write_parameter_file ( const string filestring, const vector<float> coefficients )
{
    ofstream outfile ( filestring );
    outfile << "Polynomial modulation function:\n";
    for ( int i = 0; i < coefficients.size()-1; i++ )
    {
        outfile << coefficients[i] << "x^" << coefficients.size()-1-i << " + ";
    }
    outfile << coefficients.back() << endl;
}


void convert_to_mat ( Mat &qr_image, const QrCode qr )
{
	// cout << "size: " << qr.getSize() << endl;

    Mat temp_img ( qr.getSize(), qr.getSize(), CV_8UC1, Scalar(0) );
    temp_img.copyTo ( qr_image );

    int border = 0;
	for (int y = -border; y < qr.getSize() + border; y++) {
		for (int x = -border; x < qr.getSize() + border; x++) {
			qr_image.at<u_int8_t>(y,x) = u_int8_t(255-255*qr.getModule(x, y));
		}
	}
	// return qr_image;
}

template < typename datatype >
void swap ( datatype &value, const datatype high = 255, const datatype low = 0 )
{
    if ( value == low )
    {   value = high;   }
    else
    {    value = low;   }
}

void logic_kernel ( Mat &image, const Mat kernel, const int y, const int x )
{
    Size kernel_size = kernel.size();
    u_int8_t mult;

    for ( int i = 0; i < kernel_size.height; i++ )
    {
        for ( int j = 0; j < kernel_size.width; j++ )
        {
            mult = kernel.at<u_int8_t>(i,j) / 255;
            if ( mult == 0 )
            {   swap ( image.at<u_int8_t>(x+i, y+j) );  }
        }
    }
}


void apply_kernel ( Mat &image, const Mat kernel, const int y, const int x )
{
    Size kernel_size = kernel.size();
    u_int8_t mult;

    for ( int i = 0; i < kernel_size.height; i++ )
    {
        for ( int j = 0; j < kernel_size.width; j++ )
        {
            image.at<u_int8_t>(x+i, y+j) = kernel.at<u_int8_t>(i,j);
        }
    }
}

void pad ( const Mat input, Mat & output, const int true_kern )
{
    int row_remainder = input.size().height % true_kern;
    int col_remainder = input.size().width  % true_kern;

    copyMakeBorder ( input, output, 0, row_remainder, 0, col_remainder, BORDER_CONSTANT, Scalar(255) );
}

void generate_qr_image ( Mat &image, string input_string ) //const char* text ) 
{
	int n = input_string.length();  
      
    // convert from string to character array
    char text[n+1];  
    strcpy(text, input_string.c_str());
    // cout << "converting " << text << " to qr code." << endl;
	const QrCode::Ecc errCorLvl = QrCode::Ecc::MEDIUM;  // Error correction level

	// Make and print the QR Code symbol
	const QrCode qr = QrCode::encodeText(text, errCorLvl);
	convert_to_mat ( image, qr );

	// return image;
}



void applySplicing ( const Mat input_image, Mat &output_image, const Mat qr_image, const int stride = 0)
{
    
    input_image.copyTo ( output_image );
    Size kernel     =   qr_image.size();
    Size image_size =   input_image.size();

    int row_step     =   stride + kernel.height;

    cout << "adding padding" << endl;
    if ( image_size.height % row_step !=0 || image_size.width % row_step != 0)
    {
        pad ( output_image, output_image, row_step );
    }

    Size new_size = output_image.size();

    cout << "Iterating logic kernel" << endl;
    for ( int i = 0; i <= new_size.height-row_step; i += row_step)
    {
        for ( int j = 0; j <= new_size.width-row_step; j += row_step)
        {
            logic_kernel ( output_image, qr_image, i, j );
        }
    }
}


void plant_qr_code ( const Mat input_image, Mat &output_image, const string base_id, const int stride = 0)
{

    Mat qr_image;
    generate_qr_image ( qr_image,  base_id + "-0|0"  );


    input_image.copyTo ( output_image );

    Size kernel     =   qr_image.size();
    Size image_size =   input_image.size();

    int row_step     =   stride + kernel.height;
    
    cout << "adding padding" << endl;
    if ( image_size.height % row_step !=0 || image_size.width % row_step != 0)
    {
        pad ( output_image, output_image, row_step );
    }

    Size new_size = output_image.size();

    cout << "Iterating logic kernel" << endl;
    for ( int i = 5; i <= new_size.height-row_step; i += row_step)
    {
        for ( int j = 5; j <= new_size.width-row_step; j += row_step)
        {
            generate_qr_image ( qr_image, base_id + "-" + to_string(i) + "|" + to_string(j) );
            apply_kernel ( output_image, qr_image, i, j );
        }
    }
}


Mat generate_random_binary (const int row, const int col )
{
	Mat random_image ( row, col, CV_8UC1, Scalar(0) );
	cout << "made empty image" << endl;

	srand(time(NULL));
	for (int x = 0; x < row; x++) 
	{
		for (int y = 0; y < col; y++) 
		{
			random_image.at<u_int8_t>(x,y) = u_int8_t( 255*( rand() % 5 ) );
		}
	}

	return random_image;
}

float get_fx ( const vector<float> poly, const float x )
{
    int j = 0;
    float output;
    for ( int i = poly.size()-1; i>0; i-- )
    {   output += poly[j] * pow ( x, i );   j++;   }

    return output;
}

float polynomial_dydx ( const vector<float> coefficients, const float input )
{
    float output = 0;
    float n_power;
    float d_coefficient;

    int j = 0;
    for (int i =coefficients.size()-1; i>0; i--)
    {
        n_power = i-1;
        d_coefficient = coefficients[j]*i;
        output += d_coefficient * pow ( input, n_power );
        j++;
    }

    return output;
}


float length_integral ( const vector<float> fx,  float upper_bound, float lower_bound = 0, float dx = 0.01 )
{

    // Equation for arc length of an explicit function [ y = f(x) ]:
    //      L = integral ( ds ) where ds = sqrt ( 1 + (dy/dx)^2 ) dx
    float ds;
    float integral = pow ( 1+ pow(polynomial_dydx ( fx, lower_bound), 2), 0.5) + pow ( 1+ pow(polynomial_dydx ( fx, upper_bound), 2), 0.5);
    for ( float x = lower_bound+dx; x<upper_bound; x+=dx )
    {
        ds = pow ( 1+ pow(polynomial_dydx ( fx, x), 2), 0.5);
        integral += 2*ds;
    }
    integral *= (dx / 2);

    return integral;

}

void crop( Mat &image , const int rowstart, const int colstart, const int rowend, const int colend )
{
    cout << "cropping to " << colstart << " " << rowstart << " " << colend << " " << rowend << endl;
        Rect cropper = Rect(colstart,rowstart, colend-colstart, rowend-rowstart);
        image = image(cropper);
}

void warp_image ( const Mat input_image, Mat &output_image, const vector<float> fx )
{
    cout << "beginning the rewrite"<< endl;
    // Copy image and add padding
    int x_pad = 200;
    int y_pad = 200;
    copyMakeBorder ( input_image, output_image, y_pad, y_pad, 0, x_pad, BORDER_CONSTANT, Scalar(255) );
    output_image.setTo ( Scalar(255) );
    Size input_size = input_image.size();
    Size output_size = output_image.size();
    int warped_y;
    int previous_x;
    
    int warp_row;
    int max_rewrite_x (0), max_rewrite_y (0);
    int lowest_y = output_size.height;
    for ( int x = 0; x < input_size.width; x++ )
    {
        warped_y = int ( round ( get_fx ( fx, x ) ) );
        if ( warped_y + y_pad < lowest_y && warped_y+y_pad > 0)
        {
            lowest_y = warped_y + y_pad;
        }

        previous_x = int ( round ( length_integral ( fx, x ) ) ); 
        for (int j = 0; j < input_size.height; j++ )
        {
            warp_row = warped_y+j+y_pad;
            
            if (warp_row < output_size.height && warp_row >= 0 && previous_x < input_size.width && previous_x >= 0)
            {
                // cout << warp_row << " " << warped_y << endl;
                output_image.at<u_int8_t>( warp_row, x ) = input_image.at<u_int8_t>( j, previous_x );
                if ( warp_row > max_rewrite_y )
                {
                    max_rewrite_y = warp_row;
                }
                
                max_rewrite_x = x;
            }
        }


    }

    crop ( output_image, lowest_y, 0, max_rewrite_y, max_rewrite_x );
    
    cout << "done the rewrite" << endl;
    
}

