#include <stdint.h>
#include <stdlib.h>

#include <iostream>

#include "syntax_parser.h"
#include "unity.h"

static char const * pToTest = NULL;

#define RUN_SUCCESS_TEST(arg) \
   pToTest = arg; \
   RUN_TEST(TestForParseSuccess);

#define RUN_FAILURE_TEST(arg) \
   pToTest = arg; \
   RUN_TEST(TestForParseFailure);
   
void TestForParseSuccess(void)
{
   Parser parser;
   Parse(&parser, pToTest);
   TEST_ASSERT_TRUE(parser.m_success);
}

void TestForParseFailure(void)
{
   Parser parser;
   Parse(&parser, pToTest);
   TEST_ASSERT_FALSE(parser.m_success);
}

int main()
{
   UnityBegin("syntax_parser.test.cpp");

   RUN_SUCCESS_TEST("1+2+3+4");
   RUN_SUCCESS_TEST("1*2*3*4");
   RUN_SUCCESS_TEST("1-2-3-4");
   RUN_SUCCESS_TEST("1/2/3/4");
   RUN_SUCCESS_TEST("1*2+3*4");
   RUN_SUCCESS_TEST("1+2*3+4");
   RUN_SUCCESS_TEST("(1+2)*(3+4)");
   RUN_SUCCESS_TEST("1+(2*3)*(4+5)");
   RUN_SUCCESS_TEST("1+(2*3)/4+5");
   RUN_SUCCESS_TEST("5/(4+3)/2");
   RUN_SUCCESS_TEST("1 + 2.5");
   RUN_SUCCESS_TEST("125");
   RUN_SUCCESS_TEST("-1");
   RUN_SUCCESS_TEST("-1+(-2)");
   RUN_SUCCESS_TEST("-1+(-2.0)");
 
   RUN_FAILURE_TEST("   1*2,5");
   RUN_FAILURE_TEST("   1*2.5e2");
   RUN_FAILURE_TEST("M1 + 2.5");
   RUN_FAILURE_TEST("1 + 2&5");
   RUN_FAILURE_TEST("1 * 2.5.6");
   RUN_FAILURE_TEST("1 ** 2.5");
   RUN_FAILURE_TEST("*1 / 2.5");
 
   return 0;
}