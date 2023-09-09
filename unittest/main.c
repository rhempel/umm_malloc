#include "CppUTest/CommandLineTestRunner.h"

IMPORT_TEST_GROUP (FirstTestGroup);

int main(int ac, char** av)
{
   return CommandLineTestRunner::RunAllTests(ac, av);
}