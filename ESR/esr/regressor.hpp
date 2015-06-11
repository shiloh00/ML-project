#ifndef __ESR_REGRESSOR_HPP__
#define __ESR_REGRESSOR_HPP__

#include <vector>
#include <esr.hpp>


class regressor {
	public:
		regressor(uint32_t num_fern, uint32_t num_pixel, uint32_t num_fern_pixel, uint32_t num_landmark);

		std::vector<cv::Mat_<double> > train(std::vector<cv::Mat_<uint8_t> > &image_list, 
				std::vector<cv::Mat_<double> > &shape_list, std::vector<rect> &rect_list, 
				cv::Mat_<double> &mean_shape, std::vector<cv::Mat_<double> > &current_shapes);
		
		cv::Mat_<double> predict(const cv::Mat_<uint8_t> &image, rect &r, 
				cv::Mat_<double> &mean_shape, cv::Mat_<double> &current_shape);

		bool restore(nlohmann::json &pt);
		nlohmann::json store();
	private:
		std::vector<fern> m_fern_list;

		uint32_t m_num_regressor;
		uint32_t m_num_fern;
		uint32_t m_num_pixel;
		uint32_t m_num_fern_pixel;
		uint32_t m_num_landmark;
};


#endif // __ESR_REGRESSOR_HPP__
