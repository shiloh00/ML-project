#include <iostream>
#include <esr.hpp>
#include <fstream>
#include <string>

using namespace std;
using namespace esr;

void print_usage(int argc, char **argv) {
	cout << "usage: " << argv[0] << "num rect_list shape_list model_path" << endl;
}

void print_comparison(vector<point> &real_shape, vector<point> &predicted_shape) {
	cout << "**************************************************************************" << endl;
	int len = real_shape.size();
	for(int i = 0; i < len; ++i) {
		cout << real_shape[i].x << "," << real_shape[i].y << "  " << 
			predicted_shape[i].x << "," << predicted_shape[i].y << "  " <<
			abs(real_shape[i].x - predicted_shape[i].x) << "," << abs(real_shape[i].y - predicted_shape[i].y) << endl;
	}
}

int main(int argc, char **argv) {
	if(argc != 5) {
		print_usage(argc, argv);
		return 0;
	}
	string rect_list_path(argv[2]),
	       shape_list_path(argv[3]),
	       model_path(argv[4]);
	int num = atoi(argv[1]),
	    num_landmark = 29,
	    //num_regressor = 10,
	    num_regressor = 10,
	    num_fern = 500,
	    //num_fern = 500,
	    num_pixel = 400,
	    //num_pixel = 400,
	    num_fern_pixel = 5,
	    num_augment = 50;
	cout << "Welcome to ESR train" << endl;
	alignment alignment;
	ifstream rect_file(rect_list_path), shape_file(shape_list_path);// valid_file("valid_list.txt");
	string dir_path("./dataset/trainingImages/");
	//string dir_path("./new_db/train/");
	for(int i = 0; i < num; ++i) {
		//string fn;
		//valid_file >> fn;
		//string image_name = "./dataset/trainingImages/";
		string image_name = dir_path;
		image_name = image_name + to_string(i+1) + ".jpg";
		//image_name = image_name + fn;
		vector<point> shape;
		shape.resize(num_landmark);
		double x, y, w, h;
		rect_file >> x >> y >> w >> h;
		//cout << x << "," << y << "," << w << "," << h << endl;
		for(int j = 0; j < num_landmark; ++j) {
			shape_file >> shape[j].x;
			//cout << shape[j].x << endl;
		}
		for(int j = 0; j < num_landmark; ++j) {
			shape_file >> shape[j].y;
		}
		for(int j = 0; j < num_landmark; ++j) {
			//cout << shape[j].x << "," << shape[j].y << endl;
		}
		//return 0;
		//rect_file >> x >> y >> w >> h;
		esr::rect r(x, y, w, h);
		cv::Mat_<uint8_t> temp = alignment::read_image(image_name);
		alignment.feed_sample(temp, r, shape);
	}
	cout << "Load done" << endl;

	alignment.train(num_regressor, num_fern, num_augment, num_pixel, num_fern_pixel);
	alignment.save_model(model_path);

	ifstream prect_file(rect_list_path), pshape_file(shape_list_path);// pvalid_file("valid_list.txt");
	for(int i = 0; i < 10; ++i) {
		//string fn;
		//pvalid_file >> fn;
		//string image_name = "./dataset/trainingImages/";
		string image_name = dir_path;
		image_name = image_name + to_string(i+1) + ".jpg";
		//image_name = image_name + fn;
		vector<point> shape;
		shape.resize(num_landmark);
		double x, y, w, h;
		prect_file >> x >> y >> w >> h;
		//cout << x << "," << y << "," << w << "," << h << endl;
		for(int j = 0; j < num_landmark; ++j) 
			pshape_file >> shape[j].x;
		for(int j = 0; j < num_landmark; ++j) 
			pshape_file >> shape[j].y;
		//rect_file >> x >> y >> w >> h;
		esr::rect r(x, y, w, h);
		cout << "Prediction " << i << endl;
		cout << "rect.x = " << r.x << ", y = " << r.y << ", width = " << r.width << ", height = " << r.height << endl;
		vector<esr::point> pshape = alignment.predict(alignment::read_image(image_name), r);
		cout << "Result: " << endl;
		print_comparison(shape, pshape);

	}
	return 0;
}
