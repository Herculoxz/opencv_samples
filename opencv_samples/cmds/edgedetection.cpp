#include "opencv4/opencv2/highgui.hpp"  // Always include highgui.hpp
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"  // Include for IMREAD_COLOR
#include <iostream>

#include <unistd.h>

#ifdef __EMBOX__
#include <cv_embox_imshowfb.hpp>
#endif


using namespace cv;
using namespace std;

Mat src, src_gray;
Mat dst, detected_edges;

int lowThreshold = 0;
const int max_lowthreshold = 100;
const int canny_ratio = 3;  // Renamed to avoid conflict
const int kernel_size = 3;
const char* window_name = "Edge Map";

static void help(const char* programName)
{
    cout <<
    "A program utilizing Laplacian of Gaussian (LoG), Sobel operators, and Hough transform "
    "to identify circles in a set of images (img1-6.jpg)\n"
    "Enhances edge detection by computing the gradient magnitude and direction.\n"
    "Call:\n"
    "./" << programName << " [file_name (optional)]\n"
    "Using OpenCV version " << CV_VERSION << "\n" << endl;
}

static void CannyThreshold(int, void*)
{
    blur(src_gray, detected_edges, Size(3, 3)); // conversion to gray scale and then applying gaussian blur.
    Canny(detected_edges, detected_edges, lowThreshold, lowThreshold * canny_ratio, kernel_size); // applying all the steps of canny edge detection
    dst = Scalar::all(0); // post processing
    src.copyTo(dst, detected_edges); //post processing

#ifndef __EMBOX__
    imshow(window_name, dst);
#else
    imshowfb(dst, 0);
    sleep(1);
#endif
}

int main(int argc, char** argv)
{
    CommandLineParser parser(argc, argv, "{@input | lotus.jpeg | input image}"); // to define the cli parser command for this app.
    src = imread(samples::findFile(parser.get<String>("@input")), IMREAD_COLOR); // get the path of @input and read the image in rgb colors

    if (src.empty()) // error management
    {
        std::cout << "Could not open or find the image!\n" << std::endl;
        std::cout << "Usage: " << argv[0] << " <Input image>" << std::endl;
        return -1;
    }

    dst.create(src.size(), src.type());
    cvtColor(src, src_gray, COLOR_BGR2GRAY); // decorating the window
    namedWindow(window_name, WINDOW_AUTOSIZE); //same smae
    createTrackbar("Min Threshold:", window_name, &lowThreshold, max_lowthreshold, CannyThreshold); //same same

    double t = (double)getTickCount();
    CannyThreshold(0, 0);
    t = ((double)getTickCount() - t) / getTickFrequency();
    printf("detection time of edges = %g ms\n", t * 1000);

#ifndef __EMBOX__
    imshow(window_name, dst);
#else
    imshowfb(dst, 0);
    sleep(1);
#endif

    char key = (char)waitKey();
    if (key == 27 || key == 'q' || key == 'Q') // 'ESC'
    {
        return 0;
    }

    return 0;
}