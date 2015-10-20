/*
 * C Library Includes
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "trigger.h"

class TriggerTest : public CppUnit::TestFixture  {

   CPPUNIT_TEST_SUITE(TriggerTest);
   
   CPPUNIT_TEST_SUITE_END();

public:

   void setUp(void)
   {

   }

   void tearDown(void)
   {

   }

private:
   

protected:

};

int main()
{
   CppUnit::TextUi::TestRunner runner;
   
   CPPUNIT_TEST_SUITE_REGISTRATION( TriggerTest );

   CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();

   runner.addTest( registry.makeTest() );
   runner.run();

   return 0;
}