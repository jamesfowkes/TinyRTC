/*
 * C Library Includes
 */

#include <stdbool.h>
#include <stdint.h>
#include <iostream>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "ast_node.h"
#include "syntax_parser.h"

static char const * s_pToTest = NULL;
bool s_expected = false;

static bool fTrue(void) { return true; }
static bool fFalse(void) { return false; }

#define RUN_SUCCESS_TEST(arg, expected) \
   s_pToTest = arg; \
   s_expected = expected; \
   TestForParseSuccess();

#define RUN_FAILURE_TEST(arg) \
   s_pToTest = arg; \
   TestForParseFailure();

class SyntaxParserTest : public CppUnit::TestFixture  {

   CPPUNIT_TEST_SUITE(SyntaxParserTest);
   CPPUNIT_TEST(ValidSyntaxTests);
   CPPUNIT_TEST(InvalidSyntaxTests);
   CPPUNIT_TEST_SUITE_END();

   void TestForParseSuccess(void)
   {
      Parser parser;
      ASTNode * pNode = LEP_Parse(&parser, s_pToTest);
      CPPUNIT_ASSERT_MESSAGE(parser.m_errorMessage, parser.m_success);

      bool actual = LEP_Evaluate(pNode);
      CPPUNIT_ASSERT_EQUAL(s_expected, actual);
   }

   void TestForParseFailure(void)
   {
      Parser parser;
      ASTNode * pNode = LEP_Parse(&parser, s_pToTest);
      CPPUNIT_ASSERT(!pNode);
      CPPUNIT_ASSERT(!parser.m_success);
   }

   void ValidSyntaxTests()
   {
      RUN_SUCCESS_TEST("F", false);
      RUN_SUCCESS_TEST("T", true);
      RUN_SUCCESS_TEST("0", false);
      RUN_SUCCESS_TEST("1", true);

      RUN_SUCCESS_TEST("0|0", false);
      RUN_SUCCESS_TEST("0|1", true);
      RUN_SUCCESS_TEST("1|0", true);
      RUN_SUCCESS_TEST("1|1", true);
     
      RUN_SUCCESS_TEST("0&0", false);
      RUN_SUCCESS_TEST("0&1", false);
      RUN_SUCCESS_TEST("1&0", false);
      RUN_SUCCESS_TEST("1&1", true);

      RUN_SUCCESS_TEST("F|F", false);
      RUN_SUCCESS_TEST("F|T", true);
      RUN_SUCCESS_TEST("T|F", true);
      RUN_SUCCESS_TEST("T|T", true);
     
      RUN_SUCCESS_TEST("F&F", false);
      RUN_SUCCESS_TEST("F&T", false);
      RUN_SUCCESS_TEST("T&F", false);
      RUN_SUCCESS_TEST("T&T", true);

      RUN_SUCCESS_TEST("0|T", true);
      RUN_SUCCESS_TEST("0|F", false);
      RUN_SUCCESS_TEST("1|T", true);
      RUN_SUCCESS_TEST("1|F", true);
      RUN_SUCCESS_TEST("0&T", false);
      RUN_SUCCESS_TEST("0&F", false);
      RUN_SUCCESS_TEST("1&T", true);
      RUN_SUCCESS_TEST("1&F", false);

      RUN_SUCCESS_TEST("!F", true);
      RUN_SUCCESS_TEST("!T", false);

      RUN_SUCCESS_TEST("!0", true);
      RUN_SUCCESS_TEST("!1", false);

      RUN_SUCCESS_TEST("!0|0", true);
      RUN_SUCCESS_TEST("!0|1", true);
      RUN_SUCCESS_TEST("!1|0", false);
      RUN_SUCCESS_TEST("!1|1", true);

      RUN_SUCCESS_TEST("!0&0", false);
      RUN_SUCCESS_TEST("!0&1", true);
      RUN_SUCCESS_TEST("!1&0", false);
      RUN_SUCCESS_TEST("!1&1", false);

      RUN_SUCCESS_TEST("!F|F", true);
      RUN_SUCCESS_TEST("!F|T", true);
      RUN_SUCCESS_TEST("!T|F", false);
      RUN_SUCCESS_TEST("!T|T", true);

      RUN_SUCCESS_TEST("!F&F", false);
      RUN_SUCCESS_TEST("!F&T", true);
      RUN_SUCCESS_TEST("!T&F", false);
      RUN_SUCCESS_TEST("!T&T", false);

      RUN_SUCCESS_TEST("(0)", false);
      RUN_SUCCESS_TEST("(1)", true);

      RUN_SUCCESS_TEST("(F)", false);
      RUN_SUCCESS_TEST("(T)", true);

      RUN_SUCCESS_TEST("(1)&(0|1)", true);
      RUN_SUCCESS_TEST("(1)|(0&1)", true);
      RUN_SUCCESS_TEST("(0)|(0&1)", false);
   }

   void InvalidSyntaxTests()
   {
      RUN_FAILURE_TEST("   1|2,5");
      RUN_FAILURE_TEST("   1|2.5e2");
      RUN_FAILURE_TEST("M1 & 2.5");
      RUN_FAILURE_TEST("1 | 2.5.6");
      RUN_FAILURE_TEST("1 || 2.5");
      RUN_FAILURE_TEST("|1 & 2.5");
   }
};

int main()
{
   LEP_Init();

   LEP_RegisterFunction(0, fFalse);
   LEP_RegisterFunction(1, fTrue);

   CppUnit::TextUi::TestRunner runner;
   
   CPPUNIT_TEST_SUITE_REGISTRATION( SyntaxParserTest );

   CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();

   runner.addTest( registry.makeTest() );
   runner.run();

   return 0;
}