#include "stdafx.h"
#include "CppUnitTest.h"

#include <Tws.h>

#define DONT_CONNECT_TO_TWS

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace twsif_test
{		
	TEST_CLASS(TwsTest)
	{
	public:
		
		TEST_METHOD(TwsTest_Construction)
		{
            auto tws = Tws::MakeShared();
		}

#ifndef DONT_CONNECT_TO_TWS
        TEST_METHOD(Connection)
        {
            auto tws = Tws::MakeShared();
            Assert::IsTrue(tws->Connect("localhost", 1));
            Assert::IsTrue(tws->IsConnected());
            tws->Disconnect();
            Assert::IsFalse(tws->IsConnected());
        }
#endif

	};
}
