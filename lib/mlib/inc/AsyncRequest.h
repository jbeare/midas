#pragma once

#include <Windows.h>
#include <memory>
#include <mutex>
#include <UniqueHandle.h>

template<class Result>
class AsyncResult
{
public:
    virtual bool CancelRequest() = 0;

    Result GetResult()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_completed && !m_cancelled && !m_resultRetrieved)
        {
            m_resultRetrieved = true;
            return std::move(m_result);
        }
        else
        {
            throw MLibException{E_ILLEGAL_STATE_CHANGE};
        }
    }

    Result GetResultSync()
    {
        WaitForResult();
        return GetResult();
    }

    void SetCompletionRoutine(std::function<void(Result)> completionRoutine)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_cancelled && !m_resultRetrieved && !m_completionRoutine)
        {
            m_completionRoutine = completionRoutine;
            if (m_completed && m_completionRoutine)
            {
                m_resultRetrieved = true;
                m_completionRoutine(std::move(m_result));
            }
        }
        else
        {
            throw MLibException{E_ILLEGAL_STATE_CHANGE};
        }
    }

    bool WaitForResult(uint32_t timeout = INFINITE)
    {
        return m_event.Wait(timeout);
    }

protected:
    std::mutex m_mutex;
    UniqueEvent m_event{nullptr, true, false, nullptr};
    _Guarded_by_(m_mutex) Result m_result;
    _Guarded_by_(m_mutex) std::function<void(Result)> m_completionRoutine;
    _Guarded_by_(m_mutex) bool m_completed{false};
    _Guarded_by_(m_mutex) bool m_cancelled{false};
    _Guarded_by_(m_mutex) bool m_resultRetrieved{false};
};

template<class Result>
class AsyncRequest : public AsyncResult<Result>, public std::enable_shared_from_this<AsyncRequest<Result>>
{
public:
    static std::shared_ptr<AsyncRequest<Result>> MakeShared(std::function<bool()> cancellationRoutine = nullptr)
    {
        return std::make_shared<AsyncRequest<Result>>(cancellationRoutine);
    }

    _No_competing_thread_ AsyncRequest(std::function<bool()> cancellationRoutine) :
        m_cancellationRoutine(cancellationRoutine)
    {

    }

    std::shared_ptr<AsyncResult<Result>> GetAsyncResult()
    {
        return std::static_pointer_cast<AsyncResult<Result>>(shared_from_this());
    }

    void SetResult(Result result)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_completed && !m_cancelled)
        {
            m_completed = true;
            m_result = std::move(result);
            m_event.Set();
            if (m_completionRoutine)
            {
                m_resultRetrieved = true;
                m_completionRoutine(std::move(m_result));
            }
        }
        else if (m_completed)
        {
            throw MLibException{E_ILLEGAL_STATE_CHANGE};
        }
    }

private:
    virtual bool CancelRequest()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_completed && !m_cancelled && m_cancellationRoutine)
        {
            m_cancelled = m_cancellationRoutine();
            return m_cancelled;
        }
        else if (m_cancelled)
        {
            throw MLibException{E_ILLEGAL_STATE_CHANGE};
        }
        return false;
    }

    _Guarded_by_(m_mutex) std::function<bool()> m_cancellationRoutine;
};
