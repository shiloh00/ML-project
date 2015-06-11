#include <esr.hpp>
#include <fstream>
#include <vector>

using namespace std;
using namespace esr;
using namespace nlohmann;


// initialize the alignment instance
alignment::alignment() {
	m_trained = false;
	m_num_regressor = 0;
	m_num_fern = 0;
	m_num_augment = 0;
	m_num_pixel = 0;
	m_num_fern_pixel = 0;
	m_num_landmark = 0;
}


// help read the image as a gray-scale image
// this method will be called when loading training or test images
cv::Mat_<uint8_t> alignment::read_image(string &path) {
	//return cv::imread(path, 0);
	return cv::imread(path, CV_LOAD_IMAGE_GRAYSCALE);
}

// save the model into a JSON format file
bool alignment::save_model(string &path) {
	cout << "Start to save the model..." << endl;
	json pt, mean_shape_pt;
	// construct property tree
	pt["num_regressor"] = to_string(m_num_regressor);
	pt["num_fern"] = to_string(m_num_fern);
	pt["num_initial"] = to_string(m_num_augment);
	pt["num_candidate_pixel"] = to_string(m_num_pixel);
	pt["num_fern_pixel"] = to_string(m_num_fern_pixel);
	pt["num_landmark"] = to_string(m_num_landmark);
	mean_shape_pt = write_mat(m_mean_shape);
	pt["mean_shape"] = mean_shape_pt;
	vector<json> pt_vec;
	
	// call regressors' store method to get a regressor model
	for(uint32_t i = 0; i < m_num_regressor; ++i) {
		pt_vec.push_back(m_regressor_list[i].store());
		cout << "Done with saving " << (i+1) << " regressors" << endl;
	}
	pt["regressors"] = write_vec(pt_vec);

	pt_vec.clear();
	int len = m_shape_list.size();
	for(int i = 0; i < len; ++i) {
		pt_vec.push_back(write_mat(m_shape_list[i]));
	}
	pt["shapes"] = write_vec(pt_vec);
	//cout << pt << endl;

	pt_vec.clear();
	len = m_rect_list.size();
	for(int i = 0; i < len; ++i) {
		json tmp_pt;
		rect r = m_rect_list[i];
		tmp_pt["x"] = to_string(r.x);
		tmp_pt["y"] = to_string(r.y);
		tmp_pt["width"] = to_string(r.width);
		tmp_pt["height"] = to_string(r.height);
		//cout << "serialize: " << r.x << "," << r.y << "," << r.width << "," << r.height << endl;
		pt_vec.push_back(tmp_pt);
	}
	pt["rects"] = write_vec(pt_vec);

	// write property tree to the model file
	ofstream ofs(path);
	ofs << pt;
	//boost::property_tree::write_json(ofs, pt, false);
	ofs.close();
	cout << "Save model done" << endl;
	return true;
}

// restore the model from a model file
bool alignment::load_model(string &path) {
	// read the property tree from the model file
	ifstream ifs(path);
	json pt;
	// load the JSON-form model from a file stream
	ifs >> pt;
	//boost::property_tree::read_json(ifs, pt);
	ifs.close();

	// begin to resolve the property tree
	m_num_regressor = stoi(json_read(pt, "num_regressor"));
	m_num_fern = stoi(json_read(pt, "num_fern"));
	m_num_augment = stoi(json_read(pt, "num_initial"));
	m_num_pixel = stoi(json_read(pt, "num_candidate_pixel"));
	m_num_fern_pixel = stoi(json_read(pt, "num_fern_pixel"));
	m_num_landmark = stoi(json_read(pt, "num_landmark"));
	m_mean_shape = read_mat<double>(pt["mean_shape"]);
	vector<json> pt_vec = read_vec(pt["regressors"]);
	for(uint32_t i = 0; i < m_num_regressor; ++i) {
		cout << "Load " << i << " regressor(s)" << endl;
		m_regressor_list.push_back(regressor(m_num_fern, m_num_pixel, m_num_fern_pixel, m_num_landmark));
		if(!m_regressor_list[i].restore(pt_vec[i]))
			return false;
	}
	pt_vec = read_vec(pt["shapes"]);
	int len = pt_vec.size();
	for(int i = 0; i < len; ++i) {
		m_shape_list.push_back(read_mat<double>(pt_vec[i]));
	}
	pt_vec = read_vec(pt["rects"]);
	len = pt_vec.size();
	cout << "Begin to load rect_list, len = " << len << endl;
	for(int i = 0; i < len; ++i) {
		json tpt = pt_vec[i];
		m_rect_list.push_back(rect(stod(json_read(tpt, "x")), stod(json_read(tpt, "y")), 
					stod(json_read(tpt, "width")), stod(json_read(tpt, "height"))));
	}

	m_trained = true;
	cout << "Load model done" << endl;

	return true;
}

// this function is to augment the dataset given a augment times
static vector<cv::Mat_<double> > augment_dataset(const vector<rect> &r, const vector<cv::Mat_<double> > &shape_list, uint32_t augment_times) {
	cv::RNG rng(RNG_SEED);
	int len = shape_list.size();
	vector<cv::Mat_<double> > res;
	for(int i = 0; i < len; ++i) {
		for(uint32_t j = 0; j < augment_times; ++j) {
			int idx = (i + rng.uniform(1, len)) % len;
			// each image has more than one initial shape for starting training
			res.push_back(denorm_shape(norm_shape(shape_list[idx], r[idx]), r[i]));
		}
	}
	return res;
}

// to train the alignment instance
bool alignment::train(uint32_t num_regressor, uint32_t num_fern, uint32_t num_augment,
		uint32_t num_pixel, uint32_t num_fern_pixel) {

	vector<cv::Mat_<uint8_t> > new_image_list;
	vector<rect> new_rect_list;
	vector<cv::Mat_<double> > new_shape_list, next_shape_list;
	cv::RNG rng(RNG_SEED);
	int len = m_image_list.size();

	m_num_regressor = num_regressor;
	m_num_fern = num_fern;
	m_num_augment = num_augment;
	m_num_pixel = num_pixel;
	m_num_fern_pixel = num_fern_pixel;

	// generate the augmented dataset from the original dataset
	next_shape_list = augment_dataset(m_rect_list, m_shape_list, num_augment);
	
	// calculate the mean shape
	m_mean_shape = cv::Mat::zeros(m_num_landmark, 2, CV_64FC1);
	for(int i = 0; i < len; ++i) {
		for(uint32_t j = 0; j < num_augment; ++j) {
			new_image_list.push_back(m_image_list[i]);
			new_rect_list.push_back(m_rect_list[i]);
			new_shape_list.push_back(m_shape_list[i]);
		}
		m_mean_shape += norm_shape(m_shape_list[i], m_rect_list[i]);
	}
	m_mean_shape /= len;
	//cout << "mean shape => " << m_mean_shape << endl;

	vector<cv::Mat_<double> > delta;

	// to make sure the regressor list is empty
	m_regressor_list.clear();

	// generate the regressors for the current alignment instance
	for(uint32_t i = 0; i < m_num_regressor; ++i) {
		m_regressor_list.push_back(regressor(m_num_fern, m_num_pixel, m_num_fern_pixel, m_num_landmark));
	}

	// calling the regressors' training method for further training
	for(uint32_t i = 0; i < m_num_regressor; ++i) {
		// use the regressor in the next level for further training
		cout << "Training Regressor " << i + 1 << endl;
		delta = m_regressor_list[i].train(new_image_list, new_shape_list, new_rect_list, m_mean_shape, next_shape_list);

		// apply the predicted delta from regressor to each of examples
		int plen = delta.size();
		for(int j = 0; j < plen; ++j) {
			cv::Mat_<double> tmat = delta[j] + norm_shape(next_shape_list[j], new_rect_list[j]);
			next_shape_list[j] = denorm_shape(tmat, new_rect_list[j]);
		}
	}

	return true;
}

// method for predicting the input example
// the level parameter is developed just for data visualization
vector<point> alignment::predict(const cv::Mat_<uint8_t> &image, rect &r, int level) {
	vector<point> res;
	cv::Mat_<double> rmat = cv::Mat::zeros(m_num_landmark, 2, CV_64FC1);
	//cv::RNG rng(cv::getTickCount());
	cv::RNG rng(RNG_SEED);
	int num_level = level;
	// if the level is less than 0, do the full prediction on the current input
	if(num_level == -1 || num_level > (int)m_num_regressor)
		num_level = m_num_regressor;
	for(uint32_t i = 0; i < m_num_augment; ++i) {
		int idx = rng.uniform(0, m_shape_list.size());
		cv::Mat_<double> shape_result = m_shape_list[idx];
		rect current_rect = m_rect_list[idx];
		shape_result = denorm_shape(norm_shape(shape_result, current_rect), r);

		for(int j = 0; j < num_level; ++j) {
			// use the next level regressor for further prediction
			cv::Mat_<double> delta = m_regressor_list[j].predict(image, r, m_mean_shape, shape_result);
			shape_result = denorm_shape(delta + norm_shape(shape_result, r), r);
		}
		rmat += shape_result;
	}
	// calculate the mean shape of all output
	rmat /= m_num_augment;
	//cout << "rmat => " << rmat << endl;

	// generate the result point vector
	for(uint32_t i = 0; i < m_num_landmark; ++i) {
		res.push_back(point(rmat(i, 0), rmat(i, 1)));
	}
	return res;
}

// use this method to feed the new examples into the current alignment instance
bool alignment::feed_sample(const cv::Mat_<uint8_t> &image, rect &r, vector<point> &shape) {
	// check if the landmark size match the previous input shapes
	if(m_num_landmark) {
		if(m_num_landmark != shape.size()) {
			cerr << "shape size not fit: " << shape.size() << ", should be " << m_num_landmark << endl;
			return false;
		}
	} else {
		m_num_landmark = shape.size();
	}
	//cout << "num_landmark => " << m_num_landmark << endl;
	m_image_list.push_back(image);
	m_rect_list.push_back(r);
	cv::Mat_<double> shape_mat = cv::Mat::zeros(shape.size(), 2, CV_64FC1);
	int len = shape.size();
	// transform the vector-form shape to a matrix-form
	for(int i = 0; i < len; ++i) {
		shape_mat(i, 0) = shape[i].x;
		shape_mat(i, 1) = shape[i].y;
	}
	m_shape_list.push_back(shape_mat);
	return true;
}

