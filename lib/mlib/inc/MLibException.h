#pragma once

#include <Windows.h>
#include <exception>
#include <iostream>

#define MLIB_ASSERT(condition, hresult) {if(!(condition)) {throw MLibException{hresult};}}

#define THROW_HR_IF_FAILED_GET_LAST_ERROR(condition) {if(!(condition)) {throw MLibException{HRESULT_FROM_WIN32(GetLastError())};}}
#define THROW_HR_GET_LAST_ERROR() {throw MLibException{HRESULT_FROM_WIN32(GetLastError())};}
#define THROW_HR_IF_FAILED_BOOL(condition)  {if(!(condition)) {throw MLibException{E_FAIL};}}

class MLibException : public std::exception
{
public:
    MLibException(HRESULT hr) :
        m_hr(hr)
    {

    }

    virtual const char* what() const throw()
    {
        static char buffer[20];
        const char format[] = "Exception HRESULT: 0x%8x\n";
        if (sprintf_s(buffer, sizeof(buffer), format, sizeof(format)) == -1)
        {
            return std::exception::what();
        }
        return buffer;
    }

private:
    HRESULT m_hr;
};
