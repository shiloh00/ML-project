#ifndef __ESR_FERN_HPP__
#define __ESR_FERN_HPP__

#include <esr.hpp>

class fern {
	public:
		// constructor
		fern(uint32_t num_landmark, uint32_t num_fern_pixel);

		// method for fern to train
		std::vector<cv::Mat_<double> > train(std::vector<cv::Mat_<double> > &all_pixel_val,
				cv::Mat_<double> &covariance, cv::Mat_<double> &all_feature_pos, 
				cv::Mat_<int> &nearest_landmark_idx, std::vector<cv::Mat_<double> > &target_list);

		// method for fern to predict
		cv::Mat_<double> predict(const cv::Mat_<uint8_t> &image, rect &r, cv::Mat_<double> &shape, 
				cv::Mat_<double> &rotate, double scale);

		// manipulate the JSON model file
		bool restore(nlohmann::json &pt);
		nlohmann::json store();
	private:
		uint32_t m_num_fern_pixel;
		uint32_t m_num_landmark;
		cv::Mat_<int> m_nearest_landmark;			// Mat<int>
		cv::Mat_<double> m_threshold;				// Mat<double>
		cv::Mat_<double> m_feature_pos;		// Mat<double>
		std::vector<cv::Mat_<double> > m_move;		// Mat<double>
};

double get_covar(const cv::Mat_<double> &v1, const cv::Mat_<double> &v2);

cv::Mat_<double> norm_shape(const cv::Mat_<double> &shape, const rect &r);

cv::Mat_<double> denorm_shape(const cv::Mat_<double> &shape, const rect &r);

// a tool for a value limitation
inline double cut_value(double down, double up, double val) {
	if(val > up)
		val = up;
	if(val < down)
		val = down;
	return val;
}

// a tool to apply similarity transformation to a point
inline void  apply_transform(const cv::Mat_<double> &rotate, double scale, double ix, double iy, double &ox, double &oy) {
	ox = scale * (rotate(0, 0) * ix + rotate(0, 1) * iy);
	oy = scale * (rotate(1, 0) * ix + rotate(1, 1) * iy);
}

#endif // __ESR_FERN_HPP__
