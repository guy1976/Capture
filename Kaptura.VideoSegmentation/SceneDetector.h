#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "..\Kaltura.CaptureEngine.Common\Utils.h"
#include <chrono>

class CSceneDetector
{
	cv::Mat PrepareAnalysisImage(const cv::Mat& src);
	cv::Mat Canny(const cv::Mat&);
	bool IsDifferent(const cv::Mat&, const cv::Mat&);
	std::chrono::system_clock::time_point m_lastCaptureTime;
	std::chrono::system_clock::time_point m_lastDifferentImageTime;
	cv::Mat m_lastCanny;
	cv::Mat m_lastStableImage;
	int m_index;
	bool m_bIsSeq;
public:
	CSceneDetector();
	~CSceneDetector();

	void ProcessImage(CSample *pSample);
};

