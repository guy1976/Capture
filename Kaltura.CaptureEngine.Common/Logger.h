#pragma once

enum LogLevel
{
	Error = 0,
	Warn = 1,
	Info = 2,
	Verbose = 3,
	Debug = 4
};

void DLL_DECL Log(LogLevel level, const wchar_t* vSourceFileName, int vSourceLineNumber, const wchar_t* sText, ...);

#define __WFILE3__(x) L ## x
#define __WFILE2__(x) __WFILE3__(x)
#define __WFILE__ __WFILE2__(__FILE__)

// VC++ 8.0 Variadic macro

#define ADDLOGRAW(level,...)\
	if(true )\
{\
	Log(level,__WFILE__, __LINE__,__VA_ARGS__);\
}


#define ADDLOG(level,...) ADDLOGRAW( (LogLevel)level,__VA_ARGS__)


#ifndef TRACEGFUNC
#define TRACEGFUNC(level,frmt,...) ADDLOG(level,  ## frmt TEXT(__FUNCTION__),__VA_ARGS__)
#define TRACEID(level,id,frmt,...)  ADDLOG(level, ## TEXT("%s[this=%p] ")  TEXT(__FUNCTION__) _T(" ") ## frmt,id,this,__VA_ARGS__)
#define TRACEFUNC(level,frmt,...)  { __if_exists (m_pName) {  TRACEID(level,m_pName,frmt,__VA_ARGS__) } __if_not_exists(m_pName) {  TRACEID(level,L"",frmt,__VA_ARGS__)  } }

#define TRACEFUNC1(level)		   TRACEFUNC(level,TEXT(""))
#define TRACEFUNC0()			   TRACEFUNC(CAT_VRB,TEXT(""))
#endif
