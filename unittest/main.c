#include "CppUTest/CommandLineTestRunner.h"

IMPORT_TEST_GROUP (FirstMalloc);
IMPORT_TEST_GROUP (TooBigMalloc);
IMPORT_TEST_GROUP (Free);
IMPORT_TEST_GROUP (Realloc);
IMPORT_TEST_GROUP (MultiMalloc);
IMPORT_TEST_GROUP (Metrics);
IMPORT_TEST_GROUP (Poison);
IMPORT_TEST_GROUP (Stress);

int main(int ac, char** av)
{
   return CommandLineTestRunner::RunAllTests(ac, av);
}