#include <mat_serialize.hpp>

using namespace nlohmann;

json write_vec(const std::vector<json> &pt_vec) {
	json pt;
	int len = pt_vec.size();
	for(int i = 0; i < len; ++i) {
		pt.push_back(pt_vec[i]);
	}
	return pt;
}

std::vector<json> read_vec(json &pt) {
	std::vector<json> vec;
	for(auto iter = pt.begin(); iter != pt.end(); ++iter) {
		vec.push_back(*iter);
	}
	return vec;
}

std::string json_read(const json &js, const std::string &key) {
	return std::string((const std::string&)js[key]);
}
