#include "stdafx.h"
#include "CppUnitTest.h"

#include <WinSock2.h>
#include <Windows.h>
#include <string>
#include <deque>
#include <vector>
#include <algorithm>
#include <memory>
#include <map>
#include <iostream>

#define IB_WIN32
#define TWSAPIDLLEXP

#include <CommissionReport.h>
#include <EClientSocket.h>
#include <EReader.h>
#include <EReaderOSSignal.h>
#include <EWrapper.h>
#include <Execution.h>
#include <Order.h>
#include <OrderState.h>

#include <AsyncRequest.h>

#include <Tws.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace twsif_test
{		
	TEST_CLASS(TwsTest)
	{
	public:
		
		TEST_METHOD(TwsTest_Construction)
		{
            auto tws = TwsInterface::MakeShared();
		}

	};
}
