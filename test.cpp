

#include <iostream>
#include "test.h"
#include "main.cpp"

namespace Testing {

    void TestEasy() {
        REQUIRE(true);
    }

    void TestFromLatLon() {
        REQUIRE(true);

        Point p = helper::FromLatLon(12.56, 34.56);
        Point pCorrect(34.56, 12.661819703548765);

        Point zero = p - pCorrect;
        if (!(std::abs(zero.x) < 1e-5 && std::abs(zero.y) < 1e-5))
            throw std::runtime_error("not zero");
    }

    void RunAllTests() {
        Tests::Tester tester(0);
        tester.RegisterTest("TestFromLatLon", TestFromLatLon);
        tester.RunTests();
    }
} // namespace Testing

int main() {
    Testing::RunAllTests();
}