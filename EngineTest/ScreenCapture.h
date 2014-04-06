#pragma once

class CSample;

#include<stack>
class CScreenCapture
{
	HDC m_sourceHDC,m_destHDC;
	HWND m_hWND;
	std::stack<HBITMAP> m_hBitmaps;
	RECT m_currentRect;

	void Free();

	HBITMAP GetHBitmap();
	void ReleaseHBitmap(HBITMAP hBmp);
public:
	CScreenCapture();
	virtual ~CScreenCapture();
	HRESULT Init(HWND hWND, RECT rect);
	HRESULT Grab(HWND hWND, CSample** frame);
};

