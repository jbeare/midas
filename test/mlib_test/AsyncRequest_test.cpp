#include "stdafx.h"
#include "CppUnitTest.h"

#include <AsyncRequest.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mlib_test
{
    TEST_CLASS(AsyncRequestTest)
    {
    public:

        TEST_METHOD(Construction)
        {
            auto req1 = AsyncRequest<int>::MakeShared();
            auto req2 = AsyncRequest<int>::MakeShared([]() -> bool { return true; });
        }

        TEST_METHOD(Completion)
        {
            auto req1 = AsyncRequest<int>::MakeShared();
            auto res1 = req1->GetAsyncResult();
           
            Assert::IsFalse(res1->WaitForResult(0));
            req1->SetResult(1);
            Assert::AreEqual(res1->GetResultSync(), 1);
        }

        TEST_METHOD(CompletionRoutine)
        {
            auto req1 = AsyncRequest<int>::MakeShared();
            auto res1 = req1->GetAsyncResult();
            int result{0};

            res1->SetCompletionRoutine([&](int r) { result = r; });
            Assert::IsFalse(res1->WaitForResult(0));
            req1->SetResult(1);
            Assert::AreEqual(result, 1);

            auto req2 = AsyncRequest<int>::MakeShared();
            auto res2 = req2->GetAsyncResult();

            Assert::IsFalse(res2->WaitForResult(0));
            req2->SetResult(2);
            res2->SetCompletionRoutine([&](int r) { result = r; });
            Assert::AreEqual(result, 2);
        }

        TEST_METHOD(Cancellation)
        {
            bool cancelled{false};
            auto req1 = AsyncRequest<int>::MakeShared([&]() -> bool { cancelled = true; return cancelled; });
            auto res1 = req1->GetAsyncResult();

            Assert::IsFalse(res1->WaitForResult(0));
            Assert::IsTrue(res1->CancelRequest());
            req1->SetResult(1);

            cancelled = false;
            auto req2 = AsyncRequest<int>::MakeShared([&]() -> bool { cancelled = true; return cancelled; });
            auto res2 = req2->GetAsyncResult();

            Assert::IsFalse(res2->WaitForResult(0));
            req2->SetResult(2);
            Assert::IsFalse(res2->CancelRequest());
            Assert::AreEqual(res2->GetResultSync(), 2);
        }

        TEST_METHOD(Exceptions)
        {
            
            auto req1 = AsyncRequest<int>::MakeShared();
            auto res1 = req1->GetAsyncResult();
            int result{0};

            Assert::ExpectException<MLibException>([&]() { res1->GetResult(); });
            res1->SetCompletionRoutine([&](int r) { result = r; });
            Assert::ExpectException<MLibException>([&]() { res1->SetCompletionRoutine([](int r) {}); });

            req1->SetResult(1);
            Assert::ExpectException<MLibException>([&]() { req1->SetResult(1); });
            Assert::ExpectException<MLibException>([&]() { res1->GetResult(); });
            Assert::AreEqual(result, 1);

            bool cancelled{false};
            auto req2 = AsyncRequest<int>::MakeShared([&]() -> bool { cancelled = true; return cancelled; });
            auto res2 = req2->GetAsyncResult();

            res2->CancelRequest();
            Assert::ExpectException<MLibException>([&]() { res2->CancelRequest(); });

            auto req3 = AsyncRequest<int>::MakeShared();
            auto res3 = req3->GetAsyncResult();

            req3->SetResult(3);
            Assert::AreEqual(res3->GetResult(), 3);
            Assert::ExpectException<MLibException>([&]() { res3->GetResult(); });
        }
    };
}
