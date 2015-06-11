#ifndef __ESR_ALIGNEMTN_HPP__
#define __ESR_ALIGNEMTN_HPP__



class alignment {
	public:
		alignment();
		bool feed_sample(const cv::Mat_<uint8_t> &image, rect &r, std::vector<point> &shape);
		bool train(uint32_t num_regressor, uint32_t num_fern, uint32_t num_augment,
				uint32_t num_pixel, uint32_t num_fern_pixel);
		std::vector<point> predict(const cv::Mat_<uint8_t> &image, rect &r, int level = -1);
		bool save_model(std::string &path);
		bool load_model(std::string &path);
		static cv::Mat_<uint8_t> read_image(std::string &path);

	private:

		std::vector<rect> m_rect_list;
		std::vector<cv::Mat_<double> > m_shape_list;	// Mat<double>
		std::vector<cv::Mat_<uint8_t> > m_image_list;	// Mat<uint8_t>
		std::vector<regressor> m_regressor_list;

		// sotre the current mean shape 
		cv::Mat_<double> m_mean_shape;

		bool m_trained;
		uint32_t m_num_regressor;
		uint32_t m_num_fern;
		uint32_t m_num_augment;
		uint32_t m_num_pixel;
		uint32_t m_num_fern_pixel;
		uint32_t m_num_landmark;
};

#endif // __ESR_ALIGNEMTN_HPP__
