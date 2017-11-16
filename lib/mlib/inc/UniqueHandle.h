#pragma once

#include <Windows.h>
#include <stdint.h>
#include <exception>
#include <iostream>

#include <MLibException.h>

class UniqueHandle
{
public:
    UniqueHandle(HANDLE handle) :
        m_handle(handle)
    {

    }

    UniqueHandle(UniqueHandle&) = delete;

    UniqueHandle(UniqueHandle&& other)
    {
        m_handle = other.m_handle;
        other.m_handle = INVALID_HANDLE_VALUE;
    }

    virtual ~UniqueHandle()
    {
        CloseHandle(m_handle);
    }

    HANDLE Get()
    {
        return m_handle;
    }

private:
    HANDLE m_handle;
};

class UniqueEvent : public UniqueHandle
{
public:
    UniqueEvent(HANDLE handle) :
        UniqueHandle(handle)
    {

    }

    UniqueEvent(_In_opt_ LPSECURITY_ATTRIBUTES eventAttributes, _In_ BOOL manualReset, _In_ BOOL initialState, _In_opt_ LPCTSTR name) :
        UniqueHandle(CreateEvent(eventAttributes, manualReset, initialState, name))
    {

    }

    UniqueEvent(UniqueEvent&) = delete;

    UniqueEvent(UniqueEvent&& other) :
        UniqueHandle(std::forward<UniqueHandle>(other))
    {

    }

    void Reset()
    {
        THROW_HR_IF_FAILED_GET_LAST_ERROR(ResetEvent(Get()));
    }

    void Set()
    {
        THROW_HR_IF_FAILED_GET_LAST_ERROR(SetEvent(Get()));
    }

    // Returns true if event is set. Returns false on timeout.
    bool Wait(uint32_t timeout = INFINITE)
    {
        auto result = WaitForSingleObject(Get(), timeout);
        switch (result)
        {
        case WAIT_OBJECT_0:
            return true;
        case WAIT_TIMEOUT:
            return false;
        case WAIT_FAILED:
            THROW_HR_GET_LAST_ERROR();
        case WAIT_ABANDONED:
        default:
            throw MLibException{E_FAIL};
        }
    }
};
