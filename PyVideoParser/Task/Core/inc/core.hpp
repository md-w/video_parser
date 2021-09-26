#ifndef core_h
#define core_h
#pragma once
#if defined(_WIN32)
#define DllExport __declspec(dllexport)
#else
#define DllExport
#endif

#endif