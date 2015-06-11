//#include <boost/property_tree/ptree.hpp>
//#include <boost/property_tree/json_parser.hpp>
//#include <boost/foreach.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <json.hpp>


/*
   template<class T> 
   T string_to(std::string &str) { return T(0); }

   template<> double string_to<double>(std::string &str) {
   return std::stod(str);
   }

   template<> int string_to<int>(std::string &str) {
   return std::stoi(str);
   }
*/


nlohmann::json write_vec(const std::vector<nlohmann::json> &pt_vec);
std::vector<nlohmann::json> read_vec(nlohmann::json &pt);
std::string json_read(const nlohmann::json &js, const std::string &key);

// a template for writing matrix to a JSON object
template<class T>
nlohmann::json write_mat(const cv::Mat_<T> &mat) {
	nlohmann::json pt, matpt;
	pt["rows"] = std::to_string(mat.rows);
	pt["cols"] = std::to_string(mat.cols);
	for(int r = 0; r < mat.rows; ++r) {
		for(int c = 0; c < mat.cols; ++c) {
			//matpt.push_back(mat(r, c));
			matpt.push_back(std::to_string(mat(r, c)));
		}
	}
	pt["mat"] = matpt;
	return pt;
}

// ta template for reading JSON object to a matrix
template<class T>
cv::Mat_<T> read_mat(nlohmann::json &pt) {
	int rows = std::stoi(json_read(pt, "rows")), cols = std::stoi(json_read(pt, "cols"));
//	std::cout << "rows = " << rows << ", cols = " << cols << std::endl;
	cv::Mat_<T> res(rows, cols);
	int cr = 0, cc = 0;
	auto endi = pt["mat"].end();
	for(auto iter = pt["mat"].begin(); iter != endi; iter++) {
		std::string temp((const std::string&)*iter);
		res(cr, cc) = std::stod(temp);
		++cc;
		if(cc == cols) {
			++cr;
			cc = 0;
		}
	}
	return res;
}

