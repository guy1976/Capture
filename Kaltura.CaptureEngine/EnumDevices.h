#pragma once
#include <vector>

class CEnumDevices
{
public:
	CEnumDevices();
	virtual ~CEnumDevices();
	HRESULT EnumInputs(bool bVideo,std::vector<std::wstring>& devices);
};

