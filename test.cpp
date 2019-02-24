

#include <iostream>
#include <assert.h>
#include <sstream>
#include "test.h"
#include "main.cpp"

namespace Testing {

    void TestEasy() {
        REQUIRE(true);
    }

    void TestReadGraph() {
        Graph g("input.in");
    }

    void TestValidation() {
        int64_t cost;
        validation("test_graph.in", "test_answer.out", cost);
        if (cost != 450) {
            stringstream ss;
            ss << "wrong answer: " << cost << " correct is: " << 450 << "\n";
            throw ss.str();
        }
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
        tester.RegisterTest("Easy Test", TestEasy);
        tester.RegisterTest("Test Validation", TestValidation);
        tester.RegisterTest("TestFromLatLon", TestFromLatLon);
        tester.RegisterTest("TestReadGraph", TestReadGraph);
        tester.RunTests();
    }
} // namespace Testing

int main() {
    Testing::RunAllTests();
}