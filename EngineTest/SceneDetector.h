#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "Capture\Utils.h"
#include <chrono>

class CSceneDetector
{
	cv::Mat PrepareAnalysisImage(int width, int height, unsigned char* pData);
	cv::Mat Canny(const cv::Mat&);
	std::chrono::system_clock::time_point m_lastCaptureTime;
	std::chrono::system_clock::time_point m_lastStable;
	cv::Mat m_lastCapture;
public:
	CSceneDetector();
	~CSceneDetector();

	void ProcessImage(CSample *pSample);
};

