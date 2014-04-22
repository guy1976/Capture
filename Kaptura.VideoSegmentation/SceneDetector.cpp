#include "stdafx.h"
#include "SceneDetector.h"

#if _DEBUG
#pragma comment(lib, "opencv_core248d.lib")
#pragma comment(lib, "opencv_highgui248d.lib")
#pragma comment(lib, "opencv_imgproc248d.lib")
#pragma comment(lib, "opencv_contrib248d.lib")
#pragma comment(lib, "opencv_calib3d248d.lib")
#pragma comment(lib, "opencv_features2d248d.lib")
#pragma comment(lib, "opencv_flann248d.lib")
#pragma comment(lib, "opencv_nonfree248d.lib")
#pragma comment(lib, "zlibd.lib")
#pragma comment(lib, "libjpegd.lib")
#pragma comment(lib, "IlmImfd.lib")
#pragma comment(lib, "libjasperd.lib")
#pragma comment(lib, "libjasperd.lib")
#pragma comment(lib, "libpngd.lib")
#pragma comment(lib, "libtiffd.lib")
#else
#pragma comment(lib, "opencv_core248.lib")
#pragma comment(lib, "opencv_highgui248.lib")
#pragma comment(lib, "opencv_imgproc248.lib")
#pragma comment(lib, "opencv_contrib248.lib")
#pragma comment(lib, "opencv_calib3d248.lib")
#pragma comment(lib, "opencv_features2d248.lib")
#pragma comment(lib, "opencv_nonfree248.lib")
#pragma comment(lib, "opencv_flann248.lib")
#pragma comment(lib, "zlib.lib")
#pragma comment(lib, "libjpeg.lib")
#pragma comment(lib, "IlmImf.lib")
#pragma comment(lib, "libjasper.lib")
#pragma comment(lib, "libjasper.lib")
#pragma comment(lib, "libpng.lib")
#pragma comment(lib, "libtiff.lib")
#endif

#pragma comment(lib,"comctl32.lib")


char* window_name = "Edge Map";
char* window_name2 = "Edge Map2";


extern "C" __declspec(dllexport) CCaptureEngineSamplesProcessor* CreateSceneDetector()
{
	return new CSceneDetector();
}

CSceneDetector::CSceneDetector() : m_index(0)
{

}


CSceneDetector::~CSceneDetector()
{
}



int const max_lowThreshold = 100;

int ratio = 3;
int kernel_size = 3;


void CSceneDetector::ProcessSample(CCaptureEngineSample *pSample)
{
	auto pVideoSample = (CCaptureEngineVideoSample*)pSample;

	auto time = std::chrono::steady_clock::now();

	if (time < m_lastCaptureTime + std::chrono::seconds(1))
		return;


	cv::namedWindow(window_name, CV_WINDOW_AUTOSIZE);
	auto t0 = clock();
	cv::Mat original(pVideoSample->GetHeight(), pVideoSample->GetWidth(), CV_8UC4, pVideoSample->GetBuffer());
	cv::Mat currentImage = PrepareAnalysisImage(original);
	cv::Mat canny = Canny(currentImage);


	cv::Mat res;

	bool bIsDifferent = m_lastCanny.empty() || IsDifferent(canny, m_lastCanny);
	if (bIsDifferent)
	{
		m_lastDifferentImageTime = time;
		m_bIsSeq = false;
	}
	else
	{
		if (time > m_lastDifferentImageTime + std::chrono::seconds(1))
		{
			if (!m_bIsSeq)
			{
				m_bIsSeq = true;
				m_lastStableImage = original;
				m_index++;
				char buffer[1024];
				sprintf_s(buffer, "c:\\1\\Scene_%d.jpg", m_index);
				cv::imwrite(buffer, m_lastStableImage);
			}

		}
	}

	auto t1 = clock();
//	printf("time to  process video (%d)\n", t1 - t0);
	m_lastCaptureTime = time;
	m_lastCanny = canny;
	
	cv::imshow(window_name, canny);
	cv::waitKey(1);

}


cv::Mat CSceneDetector::PrepareAnalysisImage(const cv::Mat& src)
{
	
	cv::Mat src_gray, resized;
	cvtColor(src, src_gray, CV_RGB2GRAY);

	cv::resize(src_gray, resized, cv::Size(src.cols/ 4, src.rows/ 4), 0, 0, cv::INTER_NEAREST);

	return resized;
}

cv::Mat CSceneDetector::Canny(const cv::Mat& source)
{
	int lowThreshold = 0;

	cv::Mat detected_edges, dst;

	/// Reduce noise with a kernel 3x3
	blur(source, detected_edges, cv::Size(3, 3));

	/// Canny detector
	cv::Canny(detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size);

	/// Using Canny's output as a mask, we display our result
	dst = cv::Scalar::all(0);

	source.copyTo(dst, detected_edges);
	return dst;
}


bool CSceneDetector::IsDifferent(const cv::Mat& in1, const cv::Mat& in2)
{
	cv::Mat diff;
	cv::compare(in1, in2, diff, cv::CMP_NE);
	int value = cv::countNonZero(diff);

	int threshold = (double)(in1.rows * in1.cols) * 0.05;

	printf("Found (%d) pixels changes\n", value);
	return  (value > threshold);
}