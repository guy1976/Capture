#pragma once

#include "FFMpegCommon.h"


class CVideoEncoder : CEncoder
{
public:
	CVideoEncoder(std::shared_ptr<CFileWriter> fileWriter) : CEncoder(fileWriter) {}
protected:

	std::unique_ptr<AVCodec, std::function<void(AVCodec *)>> m_videoCodec;
	std::unique_ptr<SwsContext, std::function<void(SwsContext *)>> m_swsContext;
	std::unique_ptr<AVFrame, std::function<void(AVFrame *)>> m_pictureYUV;
	std::unique_ptr<AVStream, std::function<void(AVStream *)>> m_videoStream;
public:
	void AddVideoStream(int width, int height, int bitrate);
	void Encode(AVFrame* frame);
};
