//#include <boost/property_tree/ptree.hpp>
#include <esr.hpp>
#include <assert.h>

using namespace std;
using namespace esr;
using namespace nlohmann;

// initialize a regressor
regressor::regressor(uint32_t num_fern, uint32_t num_pixel, uint32_t num_fern_pixel, uint32_t num_landmark) {
	// initialize the current regressor
	m_num_fern = num_fern;
	m_num_pixel = num_pixel;
	m_num_fern_pixel = num_fern_pixel;
	m_num_landmark = num_landmark;
}

// do the similarity transformation
static cv::Mat_<double> similarity_transform(const cv::Mat_<double> &src, const cv::Mat_<double> &dst) {
	//construct linear system
	int n = src.rows;
	double a = 0, b = 0, d = 0;
	for(int i = 0; i < n; i++) {
		d += src(i,0) * src(i,0) + src(i,1) * src(i,1);
		a += src(i,0) * dst(i,0) + src(i,1) * dst(i,1);
		b += src(i,0) * dst(i,1) - src(i,1) * dst(i,0);
	}
	a /= d;
	b /= d;
	return (cv::Mat_<double>(2,2) << a,-b,b,a);
}

// calculate the covariance matrix
static inline cv::Mat_<double> generate_covar_mat(const vector<cv::Mat_<double> > &pixel_val) {
	int len = pixel_val.size();
	cv::Mat_<double> res = cv::Mat::zeros(len, len, CV_64FC1);
	for(int i = 0; i < len; ++i) {
		for(int j = i; j < len; ++j) {
			// res = res^T
			// TODO: res(i, j) == res(j, i)
			res(i, j) = res(j, i) = get_covar(pixel_val[i], pixel_val[j]);
		}
	}
	return res;
}

// generate the candidate feature pixel offset randomly
static inline cv::Mat_<double> generate_random_pos(int num) {
	cv::RNG rng(RNG_SEED);
	cv::Mat_<double> res = cv::Mat::zeros(num, 2, CV_64FC1);
	rng.fill(res, cv::RNG::UNIFORM, -0.08, 0.08);
	return res;
}

// extract the rotate matrix and scale from the result of similarity transformation
static void extract_rotate_scale(const cv::Mat_<double> &shape1, const cv::Mat_<double> &shape2, 
		cv::Mat_<double> &rotate, double &scale) {
	cv::Mat_<double> tmat1 = shape1.clone(), tmat2 = shape2.clone();
	double cx1 = cv::mean(tmat1.col(0))[0], cy1 = cv::mean(tmat1.col(1))[0], 
	       cx2 = cv::mean(tmat2.col(0))[0], cy2 = cv::mean(tmat2.col(1))[0];

	// translate to the center
	tmat1.col(0) -= cx1;
	tmat1.col(1) -= cy1;
	tmat2.col(0) -= cx2;
	tmat2.col(1) -= cy2;

	// use the center of the rectangle as the rotate center
	// get the similarity transformation result
	rotate = similarity_transform(tmat2, tmat1);
	double r00 = rotate(0,0), r01 = rotate(0,1);
	scale = sqrt(r00*r00+r01*r01);
	// revert the rotate matrix back to normal form
	rotate = rotate / scale;

	//cout << "rotate => " << rotate << endl;
	//cout << "scale => " << scale << endl;
	//assert(false);
}


// train the model for the current regressor
// this method will call its own fern to do the further training
vector<cv::Mat_<double> > regressor::train(vector<cv::Mat_<uint8_t> > &image_list, vector<cv::Mat_<double> > &shape_list,
		vector<rect> &rect_list, cv::Mat_<double> &mean_shape, vector<cv::Mat_<double> > &next_shape_list) {

	cv::Mat_<double> pixel_pos;
	cv::Mat_<int> nearest_landmark_index = cv::Mat_<int>(m_num_pixel, 1);
	cv::Mat_<double> rotate;
	vector<cv::Mat_<double> > target_list;
	double scale;

	int shapes_len = next_shape_list.size(), images_len = image_list.size();
	// to calculate the regression targets
	for(int i = 0; i < shapes_len; ++i) {
		target_list.push_back(norm_shape(shape_list[i], rect_list[i]) - norm_shape(next_shape_list[i], rect_list[i]));
		extract_rotate_scale(mean_shape, norm_shape(next_shape_list[i], rect_list[i]), rotate, scale);
		transpose(rotate, rotate);
		target_list[i] = scale * target_list[i] * rotate;
	}

	int num_landmark = mean_shape.rows;
	// each landmark has several pixel around it as its feature pixel candidates
	pixel_pos = generate_random_pos(m_num_pixel);
	for(uint32_t i = 0; i < m_num_pixel; ++i) {
		nearest_landmark_index(i) = ((int)i) % num_landmark;
	}

	//cout << nearest_landmark_index << endl;
	//cout << pixel_pos << endl;
	vector<cv::Mat_<double> > pixel_val;
	for(uint32_t i = 0; i < m_num_pixel; ++i) {
		pixel_val.push_back(cv::Mat::zeros(images_len, 1, CV_64FC1));
	}
	for(int i = 0; i < images_len; ++i) {
		cv::Mat_<double> rotate, temp = norm_shape(next_shape_list[i], rect_list[i]);
		extract_rotate_scale(temp, mean_shape, rotate, scale);
		for(uint32_t j = 0; j < m_num_pixel; ++j) {
			double px, py;
			int idx = nearest_landmark_index(j);
			apply_transform(rotate, scale, pixel_pos(j, 0), pixel_pos(j, 1), px, py);
			px = cut_value(0.0, image_list[i].cols - 1.0, px * rect_list[i].width / 2.0 + next_shape_list[i](idx, 0));
			py = cut_value(0.0, image_list[i].rows - 1.0, py * rect_list[i].height / 2.0 + next_shape_list[i](idx, 1));
			// put the real pixel value into the matrix
			pixel_val[j](i) = (int)image_list[i]((int)py, (int)px);
			//cout "pixel_val >>> " << pixel_val[j](i) << endl;
		}
	}

	// calculate the covariance for any two pixels
	cv::Mat_<double> covar_mat = generate_covar_mat(pixel_val);

	vector<cv::Mat_<double> > delta;
	int regression_len = target_list.size();
	// initialize the second level ferns for the current regressor
	for(uint32_t i = 0; i < m_num_fern; ++i) {
		m_fern_list.push_back(fern(num_landmark, m_num_fern_pixel));
	}

	// initialize the improving delta
	for(int i = 0; i < regression_len; ++i) {
		delta.push_back(cv::Mat::zeros(num_landmark, 2, CV_64FC1));
	}
	
	// using the fern in second level for further training
	for(uint32_t i = 0; i < m_num_fern; ++i) {
		vector<cv::Mat_<double> > temp = m_fern_list[i].train(pixel_val, covar_mat, pixel_pos, 
				nearest_landmark_index, target_list);
		for(uint32_t j = 0; j < temp.size(); ++j) {
			delta[j] = delta[j] + temp[j];
			target_list[j] = target_list[j] - temp[j];
		}

		// print the current training status
		if((i+1) % 10 == 0) {
			cout << "\r";
			cout << "Finished " << (i+1) << " out of " << m_num_fern << flush;
		}
	}
	cout << endl;

	int delta_len = delta.size();

	// calculate the improving delta for the upper level alignment unit
	for(int i = 0; i < delta_len; ++i) {
		extract_rotate_scale(norm_shape(next_shape_list[i], rect_list[i]), mean_shape, rotate, scale);
		transpose(rotate, rotate);
		delta[i] = scale * delta[i] * rotate;
	}

	return delta;
}

// regressor-level predict method
// will call its chold fern to complete the predicting process
cv::Mat_<double> regressor::predict(const cv::Mat_<uint8_t> &image, rect &r, 
		cv::Mat_<double> &mean_shape, cv::Mat_<double> &shape_list) {
	cv::Mat_<double> res = cv::Mat::zeros(shape_list.rows, 2, CV_64FC1), rotate;
	double scale;
	// get the rotation matrix and scale rate
	extract_rotate_scale(norm_shape(shape_list, r), mean_shape, rotate, scale);
	for(uint32_t i = 0; i < m_num_fern; ++i) {
		res = res + m_fern_list[i].predict(image, r, shape_list, rotate, scale);
	}
	transpose(rotate, rotate);
	// do the similarity transformation
	res = scale * res * rotate;
	return res;
}

// load from JSON file to restore the current regressor
// return false if the restore operation gets a failure
bool regressor::restore(json &pt) {
	vector<json> pt_vec = read_vec(pt["ferns"]);
	for(uint32_t i = 0; i < m_num_fern; ++i) {
		m_fern_list.push_back(fern(m_num_landmark, m_num_fern_pixel));
		if(!m_fern_list[i].restore(pt_vec[i]))
			return false;
	}
	return true;
}

// save the current regressor to the specified JSON file
json regressor::store() {
	json pt;
	vector<json> pt_vec;
	for(uint32_t i = 0; i < m_num_fern; ++i) {
		// each fern will return a JSON object
		pt_vec.push_back(m_fern_list[i].store());
	}
	pt["ferns"] = write_vec(pt_vec);
	// cout << "current pt" << pt << endl;
	return pt;
}
