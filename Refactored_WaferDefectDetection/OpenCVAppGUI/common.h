#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <map>

#define OPENCV_481

#include <opencv2/opencv.hpp>
#include <opencv2/core/ocl.hpp>
#ifdef OPENCV_481
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc/types_c.h>
#endif // OPENCV_470


#ifdef _DEBUG
#pragma comment(lib,"opencv_world481d.lib")
#else	//RELEASE
#pragma comment(lib,"opencv_world481.lib")
#endif
using namespace std;
using namespace cv;