#pragma once

#include "opencv4/opencv2/core.hpp"

cv::Mat universalConvertTo(const cv::Mat& src, int desired_type);

void fftshift(const cv::Mat& inputImg, cv::Mat& outputImg);

cv::Mat fftshift(const cv::Mat& inputImg);

void transformadaFourier2D(const cv::Mat& inputImage,
                                  cv::Mat* outputImage_mag = nullptr, cv::Mat* outputImage_fase = nullptr, cv::Mat* outputImage_real = nullptr,
                                  cv::Mat* outputImage_imag = nullptr, cv::Mat* complex_imag = nullptr,
                                  bool scale = false);

void calcularDFT(const cv::Mat& src, cv::Mat& dst, bool scale = false, bool optimumSize = false);

void filter2DFreq(const cv::Mat& src, cv::Mat& dst, const cv::Mat& H, bool normalize = true, bool usingFrecDomH = true);
