#include <esr.hpp>
#include <algorithm>

using namespace std;
using namespace esr;
using namespace nlohmann;



// calculate the covariance of two vector
double esr::get_covar(const cv::Mat_<double> &mat1, const cv::Mat_<double> &mat2) {
	return mean((mat1 - cv::mean(mat1)[0]).mul(mat2 - cv::mean(mat2)[0]))[0];
}

// normalize a real shape into a fixed range using the provided face rectangle
cv::Mat_<double> esr::norm_shape(const cv::Mat_<double> &shape, const rect &r) {
	cv::Mat_<double> res = cv::Mat::zeros(shape.rows, 2, CV_64FC1);
	res.col(0) = (shape.col(0) - r.center_x) / (r.width / 2.0);
	res.col(1) = (shape.col(1) - r.center_y) / (r.height / 2.0);
	return res;
}

// de-normalize a normalized shape back to a real shape given a face rectangle
cv::Mat_<double> esr::denorm_shape(const cv::Mat_<double> &shape, const rect &r) {
	cv::Mat_<double> res = cv::Mat::zeros(shape.rows, 2, CV_64FC1);
	res.col(0) = shape.col(0) * r.width / 2.0 + r.center_x;
	res.col(1) = shape.col(1) * r.height / 2.0 + r.center_y;
	return res;
}

// initialize the fern
fern::fern(uint32_t num_landmark, uint32_t num_fern_pixel) {
	m_num_fern_pixel = num_fern_pixel;
	m_num_landmark = num_landmark;
}

// call this method for training the current fern
vector<cv::Mat_<double> > fern::train(std::vector<cv::Mat_<double> > &all_pixel_val,
		cv::Mat_<double> &covar_mat, cv::Mat_<double> &all_feature_pos,
		cv::Mat_<int> &nearest_landmark_index, std::vector<cv::Mat_<double> > &target_list) {
	//cout << "fern train" << endl;
	int all_pixel_num = all_feature_pos.rows;
	cv::Mat_<int> feature_index = cv::Mat_<int>(m_num_fern_pixel, 2);
	m_feature_pos = cv::Mat_<double>(m_num_fern_pixel, 4);
	m_nearest_landmark = cv::Mat_<int>(m_num_fern_pixel, 2);
	m_threshold = cv::Mat_<double>(m_num_fern_pixel, 1);
	cv::RNG rng(RNG_SEED);
	int len_regression = target_list.size();
	vector<cv::Mat_<double> > delta;
	delta.resize(len_regression);

	cv::Mat_<double> random_direction(m_num_landmark, 2);
	cv::Mat_<double> covar_val(all_pixel_num, 1);
	cv::Mat_<uint8_t> find_mat = cv::Mat::zeros(all_pixel_num, all_pixel_num, CV_8UC1);

	// loop for selecting the feature pixels pairs
	for(uint32_t i = 0; i < m_num_fern_pixel; ++i) {
		// generate a random direction to find the best pixel pairs
		rng.fill(random_direction, cv::RNG::UNIFORM, -1.0, 1.0);

		cv::normalize(random_direction, random_direction);
		cv::Mat_<double> tmp_norm_shape = cv::Mat::zeros(target_list.size(), 1, CV_64FC1);

		for(int j = 0; j < len_regression; ++j) {
			tmp_norm_shape(j) = cv::sum(target_list[j].mul(random_direction))[0];
		}
		for(int j = 0; j < all_pixel_num; ++j) {
			covar_val(j) = get_covar(tmp_norm_shape, all_pixel_val[j]);
		}

		double cur_max = -1.0;
		int pixel_idx1 = 0, pixel_idx2 = 0;
		// select the feature pixel pair with the maximal correlation
		for(int j = 0; j < all_pixel_num; ++j) {
			for(int k = 0; k < all_pixel_num; ++k) {
				double div_val = covar_mat(j, j) + covar_mat(k, k) - 2 * covar_mat(j, k);
				// if the pixel diff is zero or this pair has ever showed up
				if(abs(div_val) < 0.0001 || find_mat(j, k)) {
					continue;
				}
				double cur_covar = (covar_val(j) - covar_val(k)) / sqrt(div_val);
				if(abs(cur_covar) > cur_max) {
					cur_max = cur_covar;
					pixel_idx1 = j;
					pixel_idx2 = k;
				}
			}
		}
		feature_index.at<int>(i, 0) = pixel_idx1;
		feature_index.at<int>(i, 1) = pixel_idx2;

		// set the corresponding value to 1 so that we could find out 
		// if a pair of pixels has showed up ever
		find_mat(pixel_idx1, pixel_idx2) = find_mat(pixel_idx2, pixel_idx1) = 1;
		
		all_feature_pos.row(pixel_idx1).copyTo(m_feature_pos.row(i).colRange(0, 2));
		all_feature_pos.row(pixel_idx2).copyTo(m_feature_pos.row(i).colRange(2, 4));

		m_nearest_landmark.at<int>(i, 0) = nearest_landmark_index.at<int>(pixel_idx1);
		m_nearest_landmark.at<int>(i, 1) = nearest_landmark_index.at<int>(pixel_idx2);

		// use median as the current threshold
		int len_val = all_pixel_val[pixel_idx1].rows;
		vector<double> val_list;
		for(int j = 0; j < len_val; ++j) {
			val_list.push_back(all_pixel_val[pixel_idx1](j) - all_pixel_val[pixel_idx2](j));
		}
		// first sort
		sort(val_list.begin(), val_list.end());
		// select the median
		m_threshold.at<double>(i) = val_list[val_list.size() / 2];
	}

	vector<vector<int> > bins;
	int num_bin = pow(2.0, m_num_fern_pixel);
	bins.resize(num_bin);
	// put each of the shaped into the bin which it belongs to
	for(int i = 0; i < len_regression; ++i) {
		int idx = 0;
		for(uint32_t j = 0; j < m_num_fern_pixel; ++j) {
			double val1 = all_pixel_val[feature_index.at<int>(j, 0)](i), val2 = all_pixel_val[feature_index.at<int>(j, 1)](i);
			if(val1 - val2 >= m_threshold.at<double>(j)) {
				idx += 1 << j;
			}
		}
		bins[idx].push_back(i);
	}

	// calculate the move value for each bin
	double beta = 1000.0;
	m_move.resize(num_bin);
	for(int i = 0; i < num_bin; ++i) {
		cv::Mat_<double> temp = cv::Mat::zeros(m_num_landmark, 2, CV_64FC1);
		int bin_size = bins[i].size();
		for(int j = 0; j < bin_size; ++j) {
			temp += target_list[bins[i][j]];
		}
		if(bin_size == 0) {
			m_move[i] = temp;
			continue;
		}
		// beta will control how large the example will affect the move value
		// mentioned in the original paper
		temp = 1.0 / (1.0 + beta / bin_size) * temp / bin_size;
		m_move[i] = temp;
		
		// give the delta to each example in the current bin
		for(int j = 0; j < bin_size; ++j) {
			delta[bins[i][j]] = temp;
		}
	}

	return delta;
}

// to make prediction for a input image
cv::Mat_<double> fern::predict(const cv::Mat_<uint8_t> &image, rect &r, cv::Mat_<double> &mean_shape, 
		cv::Mat_<double> &rotate, double scale) {
	int idx = 0;
	double px, py;
	for(uint32_t i = 0; i < m_num_fern_pixel; ++i) {
		// calculate the first
		int nearest_landmark1 = m_nearest_landmark(i, 0), nearest_landmark2 = m_nearest_landmark(i, 1);
		apply_transform(rotate, scale, m_feature_pos(i, 0), m_feature_pos(i, 1), px, py);
		px = cut_value(0.0, image.cols - 1.0, px * r.width / 2.0 + mean_shape(nearest_landmark1, 0));
		py = cut_value(0.0, image.rows - 1.0, py * r.height / 2.0 + mean_shape(nearest_landmark1, 1));
		double val1 = (int)(image((int)py, (int)px));

		// calculate the second
		apply_transform(rotate, scale, m_feature_pos(i, 2), m_feature_pos(i, 3), px, py);
		px = cut_value(0.0, image.cols - 1.0, px * r.width / 2.0 + mean_shape(nearest_landmark2, 0));
		py = cut_value(0.0, image.rows - 1.0, py * r.height / 2.0 + mean_shape(nearest_landmark2, 1));
		double val2 = (int)(image((int)py, (int)px));

		// select the correct bin according to the threshold
		// calculate the bin index
		if(val1 - val2 >= m_threshold.at<double>(i)) {
			idx += 1 << i;
		}
	}
	return m_move[idx];
}


// load the matrix from JSON back to fern to reconstruct the fern
bool fern::restore(json &pt) {
	m_feature_pos = read_mat<double>(pt["selected_pixel_locations"]);
	m_nearest_landmark = read_mat<int>(pt["selected_nearest_landmark_index"]);
	m_threshold = read_mat<double>(pt["threshold"]);
	vector<json> pt_vec = read_vec(pt["bin_outputs"]);
	int len = pt_vec.size();
	for(int i = 0; i < len; ++i) {
		m_move.push_back(read_mat<double>(pt_vec[i]));
	}
	return true;
}

// save all the necessary matrix to JSON file so that we could load them when needed
json fern::store() {
	json pt;
	pt["selected_pixel_locations"] = write_mat(m_feature_pos);
	pt["selected_nearest_landmark_index"] = write_mat(m_nearest_landmark);
	pt["threshold"] = write_mat(m_threshold);

	// first serialized to a vector
	vector<json> pt_vec;
	int len = m_move.size();
	for(int i = 0; i < len; ++i) {
		pt_vec.push_back(write_mat(m_move[i]));
	}
	// then serialized to JSON object
	pt["bin_outputs"] = write_vec(pt_vec);
	return pt;
}
