#include "stdafx.h"
#include "CppUnitTest.h"

#include <SimpleMatrix.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mlib_test
{		
	TEST_CLASS(SimpleMatrixTest)
	{
	public:
		
		TEST_METHOD(SimpleMatrixTest_Construction)
		{
            std::vector<uint32_t> v1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
            SimpleMatrix<uint32_t> m1(v1, 3, 3);

            Assert::AreEqual(m1.Row(0)[0], 1u);
            Assert::AreEqual(m1.Row(0)[1], 2u);
            Assert::AreEqual(m1.Row(0)[2], 3u);
            Assert::AreEqual(m1.Row(1)[0], 4u);
            Assert::AreEqual(m1.Row(1)[1], 5u);
            Assert::AreEqual(m1.Row(1)[2], 6u);
            Assert::AreEqual(m1.Row(2)[0], 7u);
            Assert::AreEqual(m1.Row(2)[1], 8u);
            Assert::AreEqual(m1.Row(2)[2], 9u);

            Assert::AreEqual(m1.Col(0)[0], 1u);
            Assert::AreEqual(m1.Col(0)[1], 4u);
            Assert::AreEqual(m1.Col(0)[2], 7u);
            Assert::AreEqual(m1.Col(1)[0], 2u);
            Assert::AreEqual(m1.Col(1)[1], 5u);
            Assert::AreEqual(m1.Col(1)[2], 8u);
            Assert::AreEqual(m1.Col(2)[0], 3u);
            Assert::AreEqual(m1.Col(2)[1], 6u);
            Assert::AreEqual(m1.Col(2)[2], 9u);

            std::vector<uint32_t> v2 = {1, 2, 3, 4, 5, 6};
            SimpleMatrix<uint32_t> m2(v1, 2, 3);

            Assert::AreEqual(m2.Row(0)[0], 1u);
            Assert::AreEqual(m2.Row(0)[1], 2u);
            Assert::AreEqual(m2.Row(0)[2], 3u);
            Assert::AreEqual(m2.Row(1)[0], 4u);
            Assert::AreEqual(m2.Row(1)[1], 5u);
            Assert::AreEqual(m2.Row(1)[2], 6u);

            Assert::AreEqual(m2.Col(0)[0], 1u);
            Assert::AreEqual(m2.Col(0)[1], 4u);
            Assert::AreEqual(m2.Col(1)[0], 2u);
            Assert::AreEqual(m2.Col(1)[1], 5u);
            Assert::AreEqual(m2.Col(2)[0], 3u);
            Assert::AreEqual(m2.Col(2)[1], 6u);
		}
	};
}
