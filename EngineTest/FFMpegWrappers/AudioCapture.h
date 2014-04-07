#pragma once

#include "FFMpegCommon.h"
#include "..\Capture\Capture.h"

class CAudioCapture : public CCapture
{
	std::unique_ptr<AVInputFormat, std::function<void(AVInputFormat *)>> m_inputDevice;
	std::unique_ptr<AVFormatContext, std::function<void(AVFormatContext *)>> m_avFormatContext;
	AVFrame* ReadPacket();
	CSample* Capture();
public:
	void Init();

};