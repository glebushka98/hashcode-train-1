

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
        helper::Graph g("input.in");
    }

    void TestValidation() {
        int64_t cost;
        helper::validation("test_graph.in", "test_answer.out", cost);
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

    template <typename T>
    void TestEqual(T const & a, T const & b)
    {
        if (a == b)
            return;

        std::stringstream ss;
        ss << a << " not equal " << b;
        throw std::runtime_error(ss.str());
    }


    void TestSegments() {
        REQUIRE(true);

        vector<Point> coordinates = {
            {0,0}, {4,1}, {10, 3}, {3,2}, {2,3}, {1, 5}, {-1,4}, {-2,3}, {-3,2},
            {-4, 1}, {-5, 0}, {-3, -1}, {-2, -3}, {0, -5}, {1, -3},
            {4, -2}
        };

        auto mapa = helper::Graph<bool>::GetSegments(coordinates, 0 /* startJunctionNumber */);

        TestEqual<int>(mapa[0], 0);
        TestEqual<int>(mapa[13], 0);
        TestEqual<int>(mapa[14], 0);
        TestEqual<int>(mapa[15], 1);
        TestEqual<int>(mapa[1], 1);
        TestEqual<int>(mapa[2], 2);
        TestEqual<int>(mapa[3], 2);
        TestEqual<int>(mapa[4], 3);
        TestEqual<int>(mapa[5], 3);
        TestEqual<int>(mapa[6], 4);
        TestEqual<int>(mapa[7], 4);
        TestEqual<int>(mapa[8], 5);
        TestEqual<int>(mapa[9], 5);
        TestEqual<int>(mapa[10], 6);
        TestEqual<int>(mapa[11], 6);
        TestEqual<int>(mapa[12], 7);
    }

    void RunAllTests() {
        Tests::Tester tester(0);
        tester.RegisterTest("Easy Test", TestEasy);
        tester.RegisterTest("Test Validation", TestValidation);
        tester.RegisterTest("TestFromLatLon", TestFromLatLon);
        tester.RegisterTest("TestReadGraph", TestReadGraph);
        tester.RegisterTest("TestSegments", TestSegments);
        tester.RunTests();
    }
} // namespace Testing

int main() {
    Testing::RunAllTests();
}