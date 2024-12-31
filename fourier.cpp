#include "fourier.h"

#include "opencv4/opencv2/imgproc.hpp"

#include <QDebug>

cv::Mat universalConvertTo(const cv::Mat& src, int outputType)
{
    if (!src.data || src.empty())
    {
        qCritical() << "src is not valid\n";
        return cv::Mat();
    }

    if (outputType == CV_16S || outputType == CV_8S || outputType == CV_32S)
    {
        qCritical() << "CV_8S, CV_16S, CV_32S not implemented. Number of channels must be 1 or 3\n";
        return cv::Mat();
    }

    cv::Mat outputMat;
    int inputDataType = src.type();
    switch (inputDataType)
    {
        case CV_8UC1:
        {
            if (outputType == CV_8UC1)
                return src.clone();
            else if (outputType == CV_8UC3)
                cv::cvtColor(src, outputMat, cv::COLOR_GRAY2BGR);
            else if (outputType == CV_16UC1)
                src.convertTo(outputMat, CV_MAKETYPE(CV_16U, 1), 256.0, 0.0);
            else if (outputType == CV_16UC3)
            {
                cv::cvtColor(src, outputMat, cv::COLOR_GRAY2BGR);
                outputMat.convertTo(outputMat, CV_MAKETYPE(CV_16U, 3), 256.0, 0.0);
            }
            else if (outputType == CV_32FC1)
                src.convertTo(outputMat, CV_MAKETYPE(CV_32F, 1), 1.0 / 255.0, 0.0);
            else if (outputType == CV_32FC3)
            {
                cv::cvtColor(src, outputMat, cv::COLOR_GRAY2BGR);
                outputMat.convertTo(outputMat, CV_MAKETYPE(CV_32F, 3), 1.0 / 255.0, 0.0);
            }
            else if (outputType == CV_64FC1)
                src.convertTo(outputMat, CV_MAKETYPE(CV_64F, 1), 1.0 / 255.0, 0.0);
            else if (outputType == CV_64FC3)
            {
                cv::cvtColor(src, outputMat, cv::COLOR_GRAY2BGR);
                outputMat.convertTo(outputMat, CV_MAKETYPE(CV_64F, 3), 1.0 / 255.0, 0.0);
            }
            break;
        }
        case CV_8UC3:
        {
            if (outputType == CV_32FC1)
            {
                cv::cvtColor(src, outputMat, cv::COLOR_BGR2GRAY);
                outputMat.convertTo(outputMat, CV_MAKETYPE(CV_32F, 1), 1.0 / 255.0, 0.0);
            }
            else if (outputType == CV_64FC1)
            {
                cv::cvtColor(src, outputMat, cv::COLOR_BGR2GRAY);
                outputMat.convertTo(outputMat, CV_MAKETYPE(CV_64F, 1), 1.0 / 255.0, 0.0);
            }
            else if (outputType == CV_8UC3)
                return src.clone();
            else if (outputType == CV_8UC1)
                cv::cvtColor(src, outputMat, cv::COLOR_BGR2GRAY);
            else if (outputType == CV_32FC3)
                src.convertTo(outputMat, CV_MAKETYPE(CV_32F, 3), 1.0 / 255.0, 0.0);
            else if (outputType == CV_64FC3)
                src.convertTo(outputMat, CV_MAKETYPE(CV_64F, 3), 1.0 / 255.0, 0.0);
            else if (outputType == CV_16UC1)
            {
                cv::cvtColor(src, outputMat, cv::COLOR_BGR2GRAY);
                outputMat.convertTo(outputMat, CV_16UC1, 256.0, 0.0);
            }
            else if (outputType == CV_16UC3)
                src.convertTo(outputMat, CV_16UC3, 256.0, 0.0);
            break;
        }
        case CV_32FC1:
        {
            if (outputType == CV_8UC1)
                src.convertTo(outputMat, CV_MAKETYPE(CV_8U, 1), 255.0, 0.0);
            else if (outputType == CV_8UC3)
            {
                cv::cvtColor(src, outputMat, cv::COLOR_GRAY2BGR);
                src.convertTo(outputMat, CV_MAKETYPE(CV_8U, 3), 255.0, 0.0);
            }
            else if (outputType == CV_32FC1)
                return src.clone();
            else if (outputType == CV_32FC3)
                cv::cvtColor(src, outputMat, cv::COLOR_GRAY2BGR);
            else if (outputType == CV_64FC1)
                src.convertTo(outputMat, CV_MAKETYPE(CV_64F, 1));
            else if (outputType == CV_64FC3)
            {
                cv::cvtColor(src, outputMat, cv::COLOR_GRAY2BGR);
                src.convertTo(outputMat, CV_MAKETYPE(CV_64F, 3));
            }
            else if (outputType == CV_16UC1)
                src.convertTo(outputMat, CV_16UC1, 65535.0, 0.0);
            else if (outputType == CV_16UC3)
            {
                cv::cvtColor(src, outputMat, cv::COLOR_GRAY2BGR);
                outputMat.convertTo(outputMat, CV_16UC1, 65535.0, 0.0);
            }
            break;
        }
        case CV_32FC3:
        {
            if (outputType == CV_8UC1)
            {
                cv::cvtColor(src, outputMat, cv::COLOR_BGR2GRAY);
                cv::normalize(outputMat, outputMat, 0, 255, cv::NORM_MINMAX, CV_MAKETYPE(CV_8U, 1));
            }
            else if (outputType == CV_8UC3)
                cv::normalize(src, outputMat, 0, 255, cv::NORM_MINMAX, CV_MAKETYPE(CV_8U, 3));
            else if (outputType == CV_32FC1)
                cv::cvtColor(src, outputMat, cv::COLOR_BGR2GRAY);
            else if (outputType == CV_32FC3)
                return src.clone();
            else if (outputType == CV_64FC1)
            {
                cv::cvtColor(src, outputMat, cv::COLOR_BGR2GRAY);
                outputMat.convertTo(outputMat, CV_MAKETYPE(CV_64F, 1));
            }
            else if (outputType == CV_64FC3)
                src.convertTo(outputMat, CV_MAKETYPE(CV_64F, 3));
            else if (outputType == CV_16UC1)
            {
                cv::cvtColor(src, outputMat, cv::COLOR_BGR2GRAY);
                cv::normalize(outputMat, outputMat, 0.0, 65535.0, cv::NORM_MINMAX, CV_MAKETYPE(CV_16U, 1));
            }
            else if (outputType == CV_16UC3)
                src.convertTo(outputMat, CV_MAKETYPE(CV_16U, 3), 65535.0, 0.0);
            break;
        }
        case CV_64FC1:
        {
            if (outputType == CV_8UC1)
                src.convertTo(outputMat, CV_MAKETYPE(CV_8U, 1), 255.0, 0.0);
            else if (outputType == CV_8UC3)
            {
                src.convertTo(outputMat, CV_32FC1); // cv::cvtColor No soporta CV_64F
                cv::cvtColor(outputMat, outputMat, cv::COLOR_GRAY2BGR);
                outputMat.convertTo(outputMat, CV_MAKETYPE(CV_8U, 3), 255.0, 0.0);
            }
            else if (outputType == CV_16UC1)
                src.convertTo(outputMat, CV_MAKETYPE(CV_16U, 1), 65535.0, 0.0);
            else if (outputType == CV_16UC3)
                src.convertTo(outputMat, CV_MAKETYPE(CV_16U, 3), 65535.0, 0.0);
            else if (outputType == CV_32FC1)
                src.convertTo(outputMat, CV_MAKETYPE(CV_32F, 1));
            else if (outputType == CV_32FC3)
            {
                src.convertTo(outputMat, CV_32FC3); // cv::cvtColor No soporta CV_64F
                cv::cvtColor(outputMat, outputMat, cv::COLOR_GRAY2BGR);
                outputMat.convertTo(outputMat, CV_MAKETYPE(CV_32F, 3));
            }
            else if (outputType == CV_64FC1)
                return src.clone();
            else if (outputType == CV_64FC3)
            {
                src.convertTo(outputMat, CV_32FC3); // cv::cvtColor No soporta CV_64F
                cv::cvtColor(outputMat, outputMat, cv::COLOR_GRAY2BGR);
                outputMat.convertTo(outputMat, CV_64FC3); // cv::cvtColor No soporta CV_64F
            }
            break;
        }
        case CV_64FC3:
        {
            if (outputType == CV_8UC1)
            {
                src.convertTo(outputMat, CV_32FC1); // cv::cvtColor No soporta CV_64F
                cv::cvtColor(outputMat, outputMat, cv::COLOR_BGR2GRAY);
                outputMat.convertTo(outputMat, CV_MAKETYPE(CV_8U, 1), 1.0, 0.0);
            }
            else if (outputType == CV_8UC3)
            {
                src.convertTo(outputMat, CV_32FC3); // cv::cvtColor No soporta CV_64F
                cv::cvtColor(outputMat, outputMat, cv::COLOR_BGR2GRAY);
                outputMat.convertTo(outputMat, CV_MAKETYPE(CV_8U, 3));
            }
            else if (outputType == CV_32FC1)
            {
                cv::cvtColor(src, outputMat, cv::COLOR_BGR2GRAY);
                cv::normalize(outputMat, outputMat, 0.0, 1.0, cv::NORM_MINMAX, CV_MAKETYPE(CV_32F, 1));
            }
            else if (outputType == CV_32FC3)
                cv::normalize(src, outputMat, 0.0, 1.0, cv::NORM_MINMAX, CV_MAKETYPE(CV_32F, 3));
            else if (outputType == CV_64FC1)
            {
                src.convertTo(outputMat, CV_32FC3); // cv::cvtColor No soporta CV_64F
                cv::cvtColor(outputMat, outputMat, cv::COLOR_BGR2GRAY);
                src.convertTo(outputMat, CV_64FC1); // cv::cvtColor No soporta CV_64F
            }
            else if (outputType == CV_64FC3)
                return src.clone();
            else if (outputType == CV_16UC1)
            {
                cv::cvtColor(src, outputMat, cv::COLOR_BGR2GRAY);
                outputMat.convertTo(outputMat, CV_MAKETYPE(CV_16U, 1), 65535.0, 0.0);
            }
            else if (outputType == CV_16UC3)
                src.convertTo(outputMat, CV_MAKETYPE(CV_16U, 3), 65535.0, 0.0);
            break;
        }
        case CV_16UC1:
        {
            if (outputType == CV_8UC1)
                src.convertTo(outputMat, CV_MAKETYPE(CV_8U, 1), 255.0 / 65535.0, 0.0);
            else if (outputType == CV_8UC3)
            {
                cv::cvtColor(src, outputMat, cv::COLOR_GRAY2BGR);
                outputMat.convertTo(outputMat, CV_MAKETYPE(CV_8U, 3), 255.0 / 65535.0, 0.0);
            }
            else if (outputType == CV_16UC3)
                cv::cvtColor(src, outputMat, cv::COLOR_GRAY2BGR);
            else if (outputType == CV_16UC1)
                return src.clone();
            else if (outputType == CV_32FC1)
                src.convertTo(outputMat, CV_MAKETYPE(CV_32F, 1), 1.0 / 65535.0, 0.0);
            else if (outputType == CV_32FC3)
            {
                cv::cvtColor(src, outputMat, cv::COLOR_GRAY2BGR);
                outputMat.convertTo(outputMat, CV_MAKETYPE(CV_32F, 3), 1.0 / 65535.0, 0.0);
            }
            else if (outputType == CV_64FC1)
                src.convertTo(outputMat, CV_MAKETYPE(CV_64F, 1), 1.0 / 65535.0, 0.0);
            else if (outputType == CV_64FC3)
            {
                cv::cvtColor(src, outputMat, cv::COLOR_GRAY2BGR);
                outputMat.convertTo(outputMat, CV_MAKETYPE(CV_64F, 3), 1.0 / 65535.0, 0.0);
            }
            break;
        }
        case CV_16UC3:
        {
            if (outputType == CV_32FC1)
            {
                cv::cvtColor(src, outputMat, cv::COLOR_BGR2GRAY);
                outputMat.convertTo(outputMat, CV_MAKETYPE(CV_32F, 1), 1.0 / 65535.0, 0.0);
            }
            else if (outputType == CV_64FC1)
            {
                cv::cvtColor(src, outputMat, cv::COLOR_BGR2GRAY);
                outputMat.convertTo(outputMat, CV_MAKETYPE(CV_64F, 1), 1.0 / 65535.0, 0.0);
            }
            else if (outputType == CV_8UC3)
            {
                cv::normalize(src, outputMat, 0.0, 65535.0, cv::NORM_MINMAX, CV_MAKETYPE(CV_16U, 3));
                outputMat.convertTo(outputMat, CV_MAKETYPE(CV_8U, 3), 255.0 / 65535.0, 0.0);
            }
            else if (outputType == CV_32FC3)
                src.convertTo(outputMat, CV_MAKETYPE(CV_32F, 3), 1.0 / 65535.0, 0.0);
            else if (outputType == CV_64FC3)
                src.convertTo(outputMat, CV_MAKETYPE(CV_64F, 3), 1.0 / 65535.0, 0.0);
            else if (outputType == CV_8UC1)
            {
                cv::cvtColor(src, outputMat, cv::COLOR_BGR2GRAY);
                outputMat.convertTo(outputMat, CV_8UC1, 255.0 / 65535.0, 0.0);
            }
            else if (outputType == CV_16UC3)
                return src.clone();
            break;
        }
        default:
        {
            qCritical() << "Tipo de datos de entrada no compatible.\n";
            return cv::Mat();
        }
    }

    return outputMat;
}

void fftshift(const cv::Mat& inputImg, cv::Mat& outputImg)
{
    outputImg.create(inputImg.size(), inputImg.type());
    int cx = inputImg.cols / 2;
    int cy = inputImg.rows / 2;

    cv::Mat q0(outputImg, cv::Rect(0, 0, cx, cy));
    cv::Mat q1(outputImg, cv::Rect(cx, 0, cx, cy));
    cv::Mat q2(outputImg, cv::Rect(0, cy, cx, cy));
    cv::Mat q3(outputImg, cv::Rect(cx, cy, cx, cy));

    inputImg(cv::Rect(cx, cy, cx, cy)).copyTo(q0);
    inputImg(cv::Rect(0, cy, cx, cy)).copyTo(q1);
    inputImg(cv::Rect(cx, 0, cx, cy)).copyTo(q2);
    inputImg(cv::Rect(0, 0, cx, cy)).copyTo(q3);
}

cv::Mat fftshift(const cv::Mat& inputImg)
{
    cv::Mat outputImg(inputImg.size(), inputImg.type());
    int cx = inputImg.cols / 2;
    int cy = inputImg.rows / 2;

    cv::Mat q0(outputImg, cv::Rect(0, 0, cx, cy));
    cv::Mat q1(outputImg, cv::Rect(cx, 0, cx, cy));
    cv::Mat q2(outputImg, cv::Rect(0, cy, cx, cy));
    cv::Mat q3(outputImg, cv::Rect(cx, cy, cx, cy));

    // cv::Mat tmp;
    // q0.copyTo(tmp);
    // q3.copyTo(q0);
    // tmp.copyTo(q3);
    // q1.copyTo(tmp);
    // q2.copyTo(q1);
    // tmp.copyTo(q2);
    inputImg(cv::Rect(cx, cy, cx, cy)).copyTo(q0);
    inputImg(cv::Rect(0, cy, cx, cy)).copyTo(q1);
    inputImg(cv::Rect(cx, 0, cx, cy)).copyTo(q2);
    inputImg(cv::Rect(0, 0, cx, cy)).copyTo(q3);

    return outputImg;
}

void transformadaFourier2D(const cv::Mat& inputImage,
                           cv::Mat* outputImage_mag, cv::Mat* outputImage_fase,
                           cv::Mat* outputImage_real, cv::Mat* outputImage_imag,
                           cv::Mat* complex_imag, bool scale)
{
    // Obtener el tamaño óptimo de la DFT (potencias de 2)
    int optimalRows = cv::getOptimalDFTSize(inputImage.rows);
    int optimalCols = cv::getOptimalDFTSize(inputImage.cols);
    cv::Mat padded;
    cv::copyMakeBorder(inputImage, padded, 0, optimalRows - inputImage.rows, 0, optimalCols - inputImage.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

    cv::Mat planes[] = { cv::Mat_<double>(padded), cv::Mat::zeros(padded.size(), CV_64F) };
    cv::Mat complexI;
    cv::Mat plane1 = cv::Mat::zeros(padded.rows, padded.cols, CV_64F);
    cv::merge(planes, 2, complexI);

    cv::Mat complex_imag_local;
    if (scale)
        cv::dft(complexI, complex_imag_local, cv::DFT_SCALE);
    else
        cv::dft(complexI, complex_imag_local);

    if (complex_imag) *complex_imag = complex_imag_local;

    cv::split(complex_imag_local, planes);
    if (outputImage_real) *outputImage_real = planes[0];
    if (outputImage_imag) *outputImage_imag = planes[1];
    if (outputImage_mag) cv::magnitude(planes[0], planes[1], *outputImage_mag);
    if (outputImage_fase) cv::phase(planes[0], planes[1], *outputImage_fase, false);

    // Recortar las imágenes de salida al tamaño original de la entrada
    if (outputImage_mag) *outputImage_mag = (*outputImage_mag)(cv::Rect(0, 0, inputImage.cols, inputImage.rows));
    if (outputImage_fase) *outputImage_fase = (*outputImage_fase)(cv::Rect(0, 0, inputImage.cols, inputImage.rows));
    if (outputImage_real) *outputImage_real = (*outputImage_real)(cv::Rect(0, 0, inputImage.cols, inputImage.rows));
    if (outputImage_imag) *outputImage_imag = (*outputImage_imag)(cv::Rect(0, 0, inputImage.cols, inputImage.rows));
}

void calcularDFT(const cv::Mat& src, cv::Mat& dst, bool scale, bool optimumSize)
{
    if (optimumSize)
    {
        int optimalRows = cv::getOptimalDFTSize(src.rows);
        int optimalCols = cv::getOptimalDFTSize(src.cols);
        cv::Mat padded;
        cv::copyMakeBorder(src, padded, 0, optimalRows - src.rows, 0, optimalCols - src.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
        cv::Mat planes[] = { cv::Mat_<double>(src), cv::Mat::zeros(src.size(), CV_64FC1) };
        cv::Mat complexImg(src.size(), CV_64FC2);
        cv::merge(planes, 2, complexImg);
        scale ? cv::dft(complexImg, dst, cv::DFT_COMPLEX_OUTPUT | cv::DFT_SCALE) : cv::dft(complexImg, dst, cv::DFT_COMPLEX_OUTPUT);
        dst = dst(cv::Rect(0, 0, dst.cols, dst.rows));
    }
    else
    {
        cv::Mat planes[] = { cv::Mat_<double>(src), cv::Mat::zeros(src.size(), CV_64FC1) };
        cv::Mat complexImg(src.size(), CV_64FC2);
        cv::merge(planes, 2, complexImg);
        scale ? cv::dft(complexImg, dst, cv::DFT_COMPLEX_OUTPUT | cv::DFT_SCALE) : cv::dft(complexImg, dst, cv::DFT_COMPLEX_OUTPUT);
    }
}

void filter2DFreq(const cv::Mat& src, cv::Mat& dst, const cv::Mat& H, bool normalize, bool usingFrecDomH)
{
    // I(u, v) = F{ I(x, y) }
    cv::Mat complexI(src.size(), CV_64FC2);
    calcularDFT(src, complexI);

    //			 { H			,	si usingFrecDomH == true
    // H(u, v) = {
    //			 { F{ h(x, y) },	si usingFrecDomH == false
    cv::Mat complexH(H.size(), CV_64FC2);
    if (!usingFrecDomH)
        calcularDFT(H, complexH);
    else
    {
        cv::Mat planesH[2] = { H, cv::Mat::zeros(H.size(), CV_64FC1) };
        cv::merge(planesH, 2, complexH);
    }

    // G(u, v) = F^-1{ H(u, v).I(x, y) }
    // fftshift() al filtro es equivalente a fftshift a la imagen antes y después del filtrado
    cv::Mat complexIH(src.size(), CV_64FC2);
    fftshift(complexH, complexH);
    cv::mulSpectrums(complexI, complexH, complexIH, 0, false);
    cv::dft(complexIH, dst, cv::DFT_REAL_OUTPUT | cv::DFT_INVERSE | cv::DFT_SCALE);

    if (normalize)
        cv::normalize(dst, dst, 0, 1, cv::NORM_MINMAX, CV_64F);
}
