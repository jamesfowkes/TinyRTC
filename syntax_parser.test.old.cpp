#include <stdint.h>
#include <stdlib.h>

#include <iostream>

#include "syntax_parser_old.h"
#include "unity.h"

static char const * s_pToTest = NULL;
double s_expected = 0.0f;

#define RUN_SUCCESS_TEST(arg, expected) \
   s_pToTest = arg; \
   s_expected = expected; \
   RUN_TEST(TestForParseSuccess);

#define RUN_FAILURE_TEST(arg) \
   s_pToTest = arg; \
   RUN_TEST(TestForParseFailure);

void TestForParseSuccess(void)
{
   Parser parser;
   ASTNode * pNode = Parse(&parser, s_pToTest);
   TEST_ASSERT_TRUE(parser.m_success);

   char message[30];
   double actual = Evaluate(pNode);

   sprintf(message, "Expected: %.3f, actual: %.3f", s_expected, actual);
   TEST_ASSERT_EQUAL_FLOAT_MESSAGE(s_expected, actual, message);
}

void TestForParseFailure(void)
{
   Parser parser;
   Parse(&parser, s_pToTest);
   TEST_ASSERT_FALSE(parser.m_success);
}

int main()
{
   UnityBegin("syntax_parser_old.test.cpp");

   RUN_SUCCESS_TEST("1+2+3+4", 10.0f);
   RUN_SUCCESS_TEST("1*2*3*4", 24.0f);
   RUN_SUCCESS_TEST("1-2-3-4", -8.0f);
   RUN_SUCCESS_TEST("1/2/3/4", 1.0f/2.0f/3.0f/4.0f);
   RUN_SUCCESS_TEST("1*2+3*4", 14.0f);
   RUN_SUCCESS_TEST("1+2*3+4", 11.0f);
   RUN_SUCCESS_TEST("(1+2)*(3+4)", 21.0f);
   RUN_SUCCESS_TEST("1+(2*3)*(4+5)", 55.0f);
   RUN_SUCCESS_TEST("1+(2*3)/4+5", 7.5f);
   RUN_SUCCESS_TEST("5/(4+3)/2", 5.0f/(4.0f+3.0f)/2.0f);
   RUN_SUCCESS_TEST("1 + 2.5", 3.5f);
   RUN_SUCCESS_TEST("125", 125.0f);
   RUN_SUCCESS_TEST("-1", -1.0f);
   RUN_SUCCESS_TEST("-1+(-2)", -3.0f);
   RUN_SUCCESS_TEST("-1+(-2.0)", -3.0f);

   RUN_FAILURE_TEST("   1*2,5");
   RUN_FAILURE_TEST("   1*2.5e2");
   RUN_FAILURE_TEST("M1 + 2.5");
   RUN_FAILURE_TEST("1 + 2&5");
   RUN_FAILURE_TEST("1 * 2.5.6");
   RUN_FAILURE_TEST("1 ** 2.5");
   RUN_FAILURE_TEST("*1 / 2.5");

   return 0;
}