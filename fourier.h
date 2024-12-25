#pragma once

#include "opencv4/opencv2/opencv.hpp"

void transformadaFourier2D(const cv::Mat& input, cv::Mat& output_mag);

cv::Mat universalConvertTo(const cv::Mat& input, int desired_type);

cv::Mat fftshift(const cv::Mat& input);


