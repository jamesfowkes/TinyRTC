#include <stdint.h>
#include <stdlib.h>

#include <iostream>

#include "syntax_parser.h"
#include "unity.h"

static char const * s_pToTest = NULL;
bool s_expected = false;

static bool fTrue(void) { return true; }
static bool fFalse(void) { return false; }

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

   bool actual = Evaluate(pNode);
   TEST_ASSERT_EQUAL(s_expected, actual);
}

void TestForParseFailure(void)
{
   Parser parser;
   Parse(&parser, s_pToTest);
   TEST_ASSERT_FALSE(parser.m_success);
}

int main()
{
   UnityBegin("syntax_parser_new.test.cpp");

   RegisterFunction(0, fFalse);
   RegisterFunction(1, fTrue);

   RUN_SUCCESS_TEST("1", true);
   RUN_SUCCESS_TEST("0", false);
   RUN_SUCCESS_TEST("0|1", true);
   RUN_SUCCESS_TEST("0&1", false);
   RUN_SUCCESS_TEST("0|1&1|0", true);
   RUN_SUCCESS_TEST("0|1|0|1", true);
   RUN_SUCCESS_TEST("(0&1)|(0&1)", false);
   RUN_SUCCESS_TEST("(0|1)|(0|1)", true);
   RUN_SUCCESS_TEST("1&(2|3)|(4&5)", 55.0f);
   RUN_SUCCESS_TEST("1", 125.0f);
   RUN_SUCCESS_TEST("!1", -1.0f);
   RUN_SUCCESS_TEST("!1&(!2)", -3.0f);

   RUN_FAILURE_TEST("   1|2,5");
   RUN_FAILURE_TEST("   1|2.5e2");
   RUN_FAILURE_TEST("M1 & 2.5");
   RUN_FAILURE_TEST("1 | 2.5.6");
   RUN_FAILURE_TEST("1 || 2.5");
   RUN_FAILURE_TEST("|1 & 2.5");

   return 0;
}