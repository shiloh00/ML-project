#include <iostream>
#include <esr.hpp>
#include <fstream>
#include <string>

using namespace std;
using namespace esr;

void print_usage(int argc, char **argv) {
	cout << "usage: " << argv[0] << " num rect_list shape_list model_path" << endl;
}

double do_comparison(vector<point> &real_shape, vector<point> &predicted_shape, int num_landmark) {
	double err = 0;
	for(int i = 0; i < num_landmark; ++i) {
		double ex = real_shape[i].x - predicted_shape[i].x,
		       ey = real_shape[i].y - predicted_shape[i].y;
		err += sqrt(ex * ex + ey * ey);
	}
	return err / (double)num_landmark;
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
	    num_landmark = 29;
	    //num_regressor = 10,
	    //num_regressor = 3,
	    //num_fern = 50,
	    //num_fern = 500,
	    //num_pixel = 400,
	    //num_fern_pixel = 5,
	    //num_augment = 20;
	cout << "Welcome to ESR error detect" << endl;
	alignment alignment;
	cout << "Begin to load model " << model_path << endl;
	if(!alignment.load_model(model_path)) {
		cout << "Failed to load model! Exit.." << endl;
		return 0;
	}
	ifstream rect_file(rect_list_path), shape_file(shape_list_path); //valid_file("./valid_list.txt");
	double err = 0;
	for(int i = 0; i < num; ++i) {
		string image_name = "./dataset/testImages/" + to_string(i+1) + ".jpg";
		//string image_name = "./final_dataset/train/";
		//image_name = image_name + to_string(i+1) + ".jpg";
		vector<point> shape, pshape;
		shape.resize(num_landmark);
		double x, y, w, h;
		rect_file >> x >> y >> w >> h;
		for(int j = 0; j < num_landmark; ++j) 
			shape_file >> shape[j].x;
		for(int j = 0; j < num_landmark; ++j) 
			shape_file >> shape[j].y;
		//rect_file >> x >> y >> w >> h;
		esr::rect r(x, y, w, h);
		//cout << "rect.x = " << r.x << ", y = " << r.y << ", width = " << r.width << ", height = " << r.height << endl;
		pshape = alignment.predict(alignment::read_image(image_name), r);
		//cout << "Result: " << endl;
		double tr = do_comparison(shape, pshape, num_landmark);
		//tr = sqrt(tr / 2);
		cout << "Prediction " << i << " => " << tr << endl;
		err += tr;
		//alignment.feed_sample(alignment::read_image(image_name), r, shape);
	}
	double final_err = (double)(err / (double)num);
	cout << final_err << endl;

	//alignment.train(num_regressor, num_fern, num_augment, num_pixel, num_fern_pixel);
	//alignment.save_model(model_path);
	
	return 0;
}
