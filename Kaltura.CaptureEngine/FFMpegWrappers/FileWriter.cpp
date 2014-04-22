#include "FileWriter.h"

CFileWriter::CFileWriter()
{
}
//ffmpeg -f gdigrab -i desktop -vcodec libx264 -preset ultrafast -b:v 200k -f mp4 - movflags  frag_keyframe  c:\1.mp4

CFileWriter::~CFileWriter()
{
	Close();
}



void CFileWriter::Start()
{
	av_dump_format(m_avFormatContext.get(), 0, m_avFormatContext->filename, 1);

	/* open the output file, if needed */
	avio_open(&m_avFormatContext->pb, m_avFormatContext->filename, AVIO_FLAG_WRITE);
	avformat_write_header(m_avFormatContext.get(), NULL);
}


void CFileWriter::Close()
{
	if (m_avFormatContext)
	{
		av_write_trailer(m_avFormatContext.get());
		if (m_avFormatContext->pb)
			avio_close(m_avFormatContext->pb);

		m_avFormatContext.release();
	}
}






void CFileWriter::InitFile(std::string fileName)
{
	AVFormatContext *oc = NULL;
	avformat_alloc_output_context2(&oc, NULL, NULL, fileName.c_str());
	if (!oc)
	{
		printf("Could not deduce output format from file extension: using MPEG.\n");
		avformat_alloc_output_context2(&oc, NULL, "mp4", fileName.c_str());

	}
	if (!oc)
		return;

	AVOutputFormat* pFormat = oc->oformat;

	m_avFormatContext = std::unique_ptr<AVFormatContext, std::function<void(AVFormatContext *)>>(oc, [](AVFormatContext* f) {  av_free(f); });
	int x = av_opt_set(m_avFormatContext->priv_data, "movflags", "+frag_keyframe", 0);

}
