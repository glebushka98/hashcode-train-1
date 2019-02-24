

#include <iostream>
#include <assert.h>
#include "test.h"
#include "main.cpp"

namespace Testing {

    void TestEasy() {
        REQUIRE(true);
    }

    void TestValidation() {
//        int64_t cost;
//        validation("", cost);
//        assert(cost == 4342);
    }

    void RunAllTests() {
        Tests::Tester tester(0);
        tester.RegisterTest("Easy Test", TestEasy);
        tester.RegisterTest("Test Validation", TestValidation);
        tester.RunTests();
    }
} // namespace Testing

int main() {
    Testing::RunAllTests();
}