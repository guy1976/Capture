#pragma once

#include "D3D9.h"


class CScreenCapture
{
	HDC m_sourceHDC,m_destHDC;
	HWND m_hWND;
	BITMAP m_bmp;
	RECT m_currentRect;
public:
	CScreenCapture();
	virtual ~CScreenCapture();
	HRESULT Init(HWND hWND, RECT rect);
	HRESULT Grab(HWND hWND);
};

