

#include <iostream>
#include "test.h"
#include "main.cpp"

namespace Testing {

    void TestEasy() {
        REQUIRE(true);
    }

    void RunAllTests() {
        Tests::Tester tester(0);
        tester.RegisterTest("Easy Test", TestEasy);
        tester.RunTests();
    }
} // namespace Testing

int main() {
    Testing::RunAllTests();
}