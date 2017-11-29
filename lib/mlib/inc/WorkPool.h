#pragma once

#include <sal.h>
#include <mutex>
#include <thread>
#include <vector>
#include <UniqueHandle.h>
#include <MLibException.h>

class WorkPool
{
public:
    WorkPool(uint32_t threads = 4, uint32_t batchSize = 50) :
        m_batchSize{batchSize},
        m_threadCount{threads}
    {
        if ((threads < 1) || (batchSize < 1))
        {
            throw MLibException(E_INVALIDARG);
        }
    }

    ~WorkPool()
    {
        WaitForCompletion();
    }

    void QueueWork(_In_ std::function<void()> work)
    {
        if (m_threads.empty())
        {
            for (uint32_t i = 0; i < m_threadCount; i++)
            {
                m_threads.push_back(std::thread(&WorkPool::Worker, this));
            }
        }

        std::lock_guard<std::mutex> lock(m_mutex);
        m_work.push_back(work);
        m_wake.Set();
    }

    void WaitForCompletion()
    {
        m_halt.Set();
        for (auto& th : m_threads)
        {
            th.join();
        }
        m_threads.clear();
    }

private:
    void Worker()
    {
        HANDLE handles[2]{m_halt.Get(), m_wake.Get()};

        switch (WaitForMultipleObjects(ARRAYSIZE(handles), handles, false, INFINITE))
        {
        case WAIT_OBJECT_0:
            return;
        case WAIT_OBJECT_0 + 1:
            while (1)
            {
                std::vector<std::function<void()>> work;

                {
                    std::lock_guard<std::mutex> lock(m_mutex);

                    while (!m_work.empty() && (work.size() < m_batchSize))
                    {
                        work.push_back(std::move(m_work.back()));
                        m_work.pop_back();
                    }

                    if (!m_work.empty())
                    {
                        m_wake.Set();
                    }
                }

                if (work.empty())
                {
                    m_done.Set();
                    break;
                }

                for (auto& w : work)
                {
                    w();
                }
            }
            break;
        default:
            throw MLibException(E_UNEXPECTED);
        }
    }

    uint32_t m_batchSize;
    uint32_t m_threadCount;
    std::vector<std::thread> m_threads;
    std::mutex m_mutex;
    _Guarded_by_(m_mutex) std::vector<std::function<void()>> m_work;
    UniqueEvent m_halt{nullptr, true, false, nullptr};
    UniqueEvent m_wake{nullptr, false, false, nullptr};
    UniqueEvent m_done{nullptr, true, false, nullptr};
};
