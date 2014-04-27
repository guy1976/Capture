#include "stdafx.h"
#include "Common.h"


void Log(LogLevel level,const wchar_t* vSourceFileName, int vSourceLineNumber, const wchar_t* sText, ...)
{
	va_list args;
	va_start(args, sText);

	wchar_t buff[2048];
	int nSize = 0;// wprintf((buff, _countof(buff), _TRUNCATE, sTexts, args);
	va_end(args);
	if (nSize <= 0)
	{
		return;
	}
	buff[nSize++] = 0;

	OutputDebugString(buff);
}


