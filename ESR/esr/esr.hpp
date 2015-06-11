#ifndef __ESR_HPP__
#define __ESR_HPP__

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
//#include <boost/property_tree/ptree.hpp>
#include <mat_serialize.hpp>

// different kinds of seeds, one for debug, one for final training
//#define RNG_SEED	(0)
#define RNG_SEED	(cv::getTickCount())

namespace esr {

// face rectangle
class rect {
	public:
		double x;
		double y;
		double width;
		double height;
		double center_x;
		double center_y;
		rect(double x, double y, double w, double h) {
			this->x = x;
			this->y = y;
			this->width = w;
			this->height = h;
			// automatically calculate the center
			this->center_x = x + w / 2.0;
			this->center_y = y + h / 2.0;
			//cout << "center x: " << this->center_x << endl;
			//cout << "center y: " << this->center_y << endl;
		}
};

// point struct which construct the face shape
class point {
	public:
		double x;
		double y;
		point() {x = 0; y = 0;}
		point(double x, double y) {
			this->x = x;
			this->y = y;
		}
};

#include <fern.hpp>
#include <regressor.hpp>
#include <alignment.hpp>

}

#endif // __ESR_HPP__
