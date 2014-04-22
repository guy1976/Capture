#pragma once

#include "FFMpegCommon.h"


class CAudioEncoder : CEncoder
{
public:
	CAudioEncoder(std::shared_ptr<CFileWriter> fileWriter) : CEncoder(fileWriter) {}
protected:
	int m_frame_size;

	AVAudioFifo* m_fifo;
	std::unique_ptr<AVCodec, std::function<void(AVCodec *)>> m_audioCodec;
	std::unique_ptr<AVStream, std::function<void(AVStream *)>> m_audioStream;
	std::unique_ptr<AVFrame, std::function<void(AVFrame *)>> m_tempFrame;
public:
	void AddAudioStream(int bitrate);
	void Encode(AVFrame* inputSample);
};
