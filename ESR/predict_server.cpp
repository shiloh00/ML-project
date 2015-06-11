#include <iostream>
#include <cassert>
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <cstdio>
#include <sstream>
#include <opencv2/opencv.hpp>
#include <esr.hpp>

using namespace std;
using namespace esr;

int main(int argc, char *argv[])
{
	//bool is_parent = true;
	const int num_fork = 3;
	const int len_buffer = 8192;
	string sock_file("/tmp/alignment.sock");
	socklen_t len_addr = sizeof(struct sockaddr_un);

	if(argc != 3) {
		cout << "./esr-predict-server <cascade-path> <model-path>" << endl;
		return 0;
	}

	string cascade_model(argv[1]);
	cv::CascadeClassifier faceDetector;
	faceDetector.load(cascade_model);

	string model_path(argv[2]);
	alignment align;
	align.load_model(model_path);

	char data_buffer[len_buffer];
	int count = 0;

	// initialize the UNIX domain socket
	int sock;
	struct sockaddr_un addr;

	sock = socket(PF_UNIX,SOCK_STREAM, 0);
	if(sock < 0) {
		cout << "socket init failed" << endl;
		return 1;
	}

	memset(&addr, 0, sizeof(struct sockaddr_un));

	unlink(sock_file.c_str());

	addr.sun_family = AF_UNIX;
	sprintf(addr.sun_path, "%s", sock_file.c_str());

	if(bind(sock, (struct sockaddr*)&addr, len_addr) != 0) {
		cout << "bind failed" << endl;
		return 1;
	}

	if(listen(sock, 9) != 0) {
		cout << "listen failed" << endl;
		return 1;
	}


	// TODO: do fork here
	for(int i = 0; i < num_fork; i++) {
		pid_t pid = fork();
		if(!pid) {
			//is_parent = false;
			break;
		}
	}


	cout << "Alignment Daemon launched!" << endl;

	while(true) {
		int cli_sock = accept(sock, (struct sockaddr*)&addr, &len_addr);

		if(cli_sock < 0) {
			continue;
		}

		int num = read(cli_sock, data_buffer, len_buffer);
		if(num <= 0) {
			close(cli_sock);
			continue;
		}
		data_buffer[num] = 0;
		for(int i = num - 1; i > 0; i--) {
			char ch = data_buffer[i];
			if(ch == 0 || ch == '\r' || ch == '\n' || ch == '\t' || ch == ' ') {
				data_buffer[i] = 0;
			} else 
				break;
		}

		string img_path(data_buffer);

		cv::Mat_<uint8_t> img = alignment::read_image(img_path);

		vector<cv::Rect> faces;

		faceDetector.detectMultiScale(img, faces, 1.2, 5, 0, cv::Size(30, 30));
		count = faces.size();

		cout << "request => " << img_path << ", faces => " << count << endl;

		stringstream ss;

		for(int j = 0; j < count; j++) {
			esr::rect r(faces[j].x, faces[j].y, faces[j].width, faces[j].height);
			vector<point> shape = align.predict(img, r);
			int len = shape.size();
			for(int k = 0; k < len; k++) {
				ss << (shape[k].x/img.cols) << "," << (shape[k].y/img.rows);
				if(k != len - 1)
					ss << ",";
			}
			if(j != count - 1)
				ss << " ";
		}

		string res = ss.str();
		strcpy(data_buffer, res.c_str());
		num = write(cli_sock, data_buffer, res.size());

		
		close(cli_sock);
	}
	return 0;
}

