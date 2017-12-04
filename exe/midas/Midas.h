#pragma once

#include <iostream>
#include <memory>
#include <sal.h>
#include <Classifier.h>
#include <Tws.h>
#include <thread>
#include <UniqueHandle.h>
#include <Analyzer.h>
#include <TwsMock.h>

class Midas
{
public:
    typedef std::map<std::string, std::pair<std::shared_ptr<Classifier>, Analysis>> ClassifierMap;
    static std::shared_ptr<Midas> MakeShared(_In_ ClassifierMap classifierMap, _In_ std::shared_ptr<Tws> tws)
    {
        return std::make_shared<Midas>(classifierMap, tws);
    }

    Midas(_In_ ClassifierMap classifierMap, _In_ std::shared_ptr<Tws> tws) :
        m_classifierMap(classifierMap),
        m_tws(tws)
    {

    }

    ~Midas()
    {

    }

    void Trade()
    {
        /*printf("Trading, press any key to stop...\n");
        std::thread t(&Midas::_Trade, this);
        std::getchar();
        m_halt.Set();
        t.join();*/

        std::vector<Bar> bars;
        do
        {
            bars = std::dynamic_pointer_cast<TwsMock>(m_tws)->GetNextBars();

        } while (!bars.empty());
    }

private:
    void _Trade()
    {
        HANDLE handles[2]{m_halt.Get(), m_wake.Get()};

        switch (WaitForMultipleObjects(ARRAYSIZE(handles), handles, false, INFINITE))
        {
        case WAIT_OBJECT_0: // m_halt
            // Clear all positions.
            return;
        case WAIT_OBJECT_0 + 1: // m_wake
            // Get latest bar.
            // Generate features.
            // Classify.
            // Trade?
            break;
        default:
            throw MLibException(E_UNEXPECTED);
        }
    }

    std::map<std::string, std::pair<std::shared_ptr<Classifier>, Analysis>> m_classifierMap;
    std::shared_ptr<Tws> m_tws;
    UniqueEvent m_halt{nullptr, true, false, nullptr};
    UniqueEvent m_wake{nullptr, false, false, nullptr};
};
