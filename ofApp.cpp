#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofSetWindowTitle("openFrameworks");

	ofBackground(239);
	ofSetLineWidth(2);
	ofEnableDepthTest();
	ofDisableArbTex();

	this->cap_size = cv::Size(640, 360);
	this->rect_size = 10;
	for (int x = 0; x < this->cap_size.width; x += this->rect_size) {

		for (int y = 0; y < this->cap_size.height; y += this->rect_size) {

			auto image = make_unique<ofImage>();
			image->allocate(this->rect_size, this->rect_size, OF_IMAGE_COLOR);
			cv::Mat frame = cv::Mat(cv::Size(image->getWidth(), image->getHeight()), CV_MAKETYPE(CV_8UC3, image->getPixels().getNumChannels()), image->getPixels().getData(), 0);
			cv::Rect rect = cv::Rect(x, y, this->rect_size, this->rect_size);

			this->rect_image_list.push_back(move(image));
			this->cv_rect_list.push_back(rect);
			this->rect_frame_list.push_back(frame);
			this->draw_rect_size_list.push_back(this->rect_size);
		}
	}

	auto file_path = "D:\\MP4\\Pexels Videos 2880.mp4";
	this->cap.open(file_path);
	this->number_of_frames = this->cap.get(cv::CAP_PROP_FRAME_COUNT);
	for (int i = 0; i < this->number_of_frames; i++) {

		cv::Mat src, tmp;
		this->cap >> src;
		if (src.empty()) {

			continue;
		}

		cv::resize(src, tmp, this->cap_size);
		cv::cvtColor(tmp, tmp, cv::COLOR_BGR2RGB);

		this->frame_list.push_back(tmp);
	}
}

//--------------------------------------------------------------
void ofApp::update() {

	ofSeedRandom(39);

	int frame_index = (int)(ofGetFrameNum() * 0.42) % this->number_of_frames;
	int next_frame_index = (frame_index + 1) % this->number_of_frames;

	for (int i = 0; i < this->draw_rect_size_list.size(); i++) {

		this->draw_rect_size_list[i] = this->draw_rect_size_list[i] <= 0 ? 0 : this->draw_rect_size_list[i] - 0.1;
	}

	if (frame_index < next_frame_index) {

		cv::Mat gap = this->frame_list[frame_index] - this->frame_list[next_frame_index];

		for (int i = 0; i < this->cv_rect_list.size(); i++) {

			int x = this->cv_rect_list[i].x + this->rect_size * 0.5;
			int y = this->cv_rect_list[i].y + this->rect_size * 0.5;

			cv::Vec3b* gap_value = gap.ptr<cv::Vec3b>(y, 0);
			cv::Vec3b* color_value = this->frame_list[frame_index].ptr<cv::Vec3b>(y, 0);
			cv::Vec3b gv = gap_value[x];
			cv::Vec3b cv = color_value[x];
			if ((gv[0] + gv[1] + gv[2]) > 50) {

				this->draw_rect_size_list[i] = 1;
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	int frame_index = (int)(ofGetFrameNum() * 0.42) % this->number_of_frames;

	this->cam.begin();
	ofRotateX(180);

	for (int i = 0; i < this->rect_frame_list.size(); i++) {

		auto tremor = glm::vec3(
			ofMap(ofNoise(ofRandom(1000), ofGetFrameNum() * 0.005), 0, 1, this->rect_size * -2.5, this->rect_size * 2.5) * this->draw_rect_size_list[i],
			ofMap(ofNoise(ofRandom(1000), ofGetFrameNum() * 0.005), 0, 1, this->rect_size * -2.5, this->rect_size * 2.5) * this->draw_rect_size_list[i],
			ofMap(ofNoise(ofRandom(1000), ofGetFrameNum() * 0.005), 0, 1, this->rect_size * -2.5, this->rect_size * 2.5) * this->draw_rect_size_list[i]);

		ofPushMatrix();
		ofTranslate(this->cv_rect_list[i].x + 50 - ofGetWidth() * 0.5 + tremor.x, this->cv_rect_list[i].y + 180 - ofGetHeight() * 0.5 + tremor.y, tremor.z);
		ofRotateX(180);

		cv::Mat tmp_box_image(this->frame_list[frame_index], this->cv_rect_list[i]);
		tmp_box_image.copyTo(this->rect_frame_list[i]);

		ofFill();
		ofSetColor(255);
		this->rect_image_list[i]->update();
		this->rect_image_list[i]->getTexture().bind();

		ofDrawBox(glm::vec3(), this->rect_size);
		this->rect_image_list[i]->unbind();

		if (this->draw_rect_size_list[i] > 0) {

			ofNoFill();
			ofSetColor(0);

			ofDrawBox(glm::vec3(), this->rect_size);
		}

		ofPopMatrix();
	}

	ofNoFill();
	ofSetColor(39);
	ofDrawBox(glm::vec3(this->rect_size * 0.5, this->rect_size * -0.5, 0), 640, 360, this->rect_size);

	this->cam.end();
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}