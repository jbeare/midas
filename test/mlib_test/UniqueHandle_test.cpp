#include "stdafx.h"
#include "CppUnitTest.h"

#include <UniqueHandle.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mlib_test
{		
	TEST_CLASS(UniqueHandleTest)
	{
	public:
		
		TEST_METHOD(UniqueHandleTest_Construction)
		{
            auto h1 = UniqueHandle(INVALID_HANDLE_VALUE);
            auto h2 = UniqueEvent(nullptr, false, false, nullptr);
            auto h3 = UniqueEvent(INVALID_HANDLE_VALUE);

            Assert::AreEqual(h1.Get(), INVALID_HANDLE_VALUE);
            Assert::AreNotEqual(h2.Get(), INVALID_HANDLE_VALUE);
            Assert::AreEqual(h3.Get(), INVALID_HANDLE_VALUE);

            auto h4(std::move(h2));

            Assert::AreNotEqual(h4.Get(), INVALID_HANDLE_VALUE);
            Assert::AreEqual(h2.Get(), INVALID_HANDLE_VALUE);
		}

        TEST_METHOD(UniqueHandleTest_Events)
        {
            auto h1 = UniqueEvent(nullptr, false, false, nullptr);
            auto h2 = UniqueEvent(nullptr, true, false, nullptr);

            // Auto-reset
            Assert::IsFalse(h1.Wait(0));
            h1.Set();
            Assert::IsTrue(h1.Wait(0));
            Assert::IsFalse(h1.Wait(0));
            h1.Reset();
            Assert::IsFalse(h1.Wait(0));
            h1.Set();
            Assert::IsTrue(h1.Wait(0));
            Assert::IsFalse(h1.Wait(0));

            // Manual-reset
            Assert::IsFalse(h2.Wait(0));
            h2.Set();
            Assert::IsTrue(h2.Wait(0));
            Assert::IsTrue(h2.Wait(0));
            h2.Reset();
            Assert::IsFalse(h2.Wait(0));
            h2.Set();
            Assert::IsTrue(h2.Wait(0));
            Assert::IsTrue(h2.Wait(0));
        }

        TEST_METHOD(UniqueHandleTest_Exceptions)
        {
            auto h1 = UniqueEvent(nullptr);
            Assert::ExpectException<MLibException>([&]() { h1.Set(); });
            Assert::ExpectException<MLibException>([&]() { h1.Reset(); });
        }
	};
}
