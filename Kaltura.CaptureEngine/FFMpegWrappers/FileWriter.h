#pragma once

#include "FFMpegCommon.h"

class CFileWriter
{

	std::unique_ptr<AVOutputFormat, std::function<void(AVOutputFormat *)>> m_avOutputFormat;
	std::unique_ptr<AVFormatContext, std::function<void(AVFormatContext *)>> m_avFormatContext;

public:
	AVFormatContext* GetContext() { return m_avFormatContext.get(); }
	CFileWriter();
	virtual ~CFileWriter();
	void InitFile(std::string fileName);
	void Start();
	void Close();
};


class CEncoder
{
protected:
	std::shared_ptr<CFileWriter> m_pfileWriter;
	CEncoder(std::shared_ptr<CFileWriter> fileWriter) : m_pfileWriter(fileWriter) {}
};