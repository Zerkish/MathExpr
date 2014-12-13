// Copyright Peter Mårtensson 2014

#include <iostream>
#include <cstdio>
#include "parser.h"
#include <crtdbg.h>
#include <cassert>
#include <time.h>

#define START_TESTS bool _testStatus001 = true
#define END_TESTS \
    if (_testStatus001) \
	std::cout << "All tests ok!" << std::endl; \
    else \
	std::cout << "Some test(s) failed!" << std::endl;


#define TEST(EXPR, RESULT)					\
    if (MathExpr(EXPR).eval() != (RESULT)) {			    \
	std::cout <<  "TEST(" << #EXPR << "==" << #RESULT << ") FAILED!" << std::endl; \
	_testStatus001 = false;					    \
    } else {							\
	std::cout << "TEST(" << #EXPR << "==" << #RESULT << ") SUCCEEDED!" << std::endl; }


class TestProvider : public ValueProvider {
public:

    int provide(const std::string &expr) const {
	if (expr == "RANDOM") {
	    return rand();
	} else if (expr == "myVar") {	    // You could do your variable lookup here.
	    return 42;
	}

	return ValueProvider::provide(expr); // Let defaults deal with numbers etc.
    }
};

int main(int argc, char *argv[]) {

#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // _DEBUG

    srand(time(NULL));

    START_TESTS;
    // Basic init
    TEST(10, 10);
    TEST(-4, -4);
    TEST((-4), -4);
    TEST(((-4)), -4);
    TEST(0, 0);

    // Add
    TEST(10+2, 12);
    TEST((10+2), 12);
    TEST(10+(2), 12);
    TEST(10 + 0, 10);

    // Sub
    TEST(10-2, 8);
    TEST((10-2), 8);
    TEST(10 + (-2), 8);
    TEST((-10) - 2, -12);
    TEST(10-(-4), 14);
    TEST(10 - 0, 10);

    // Mul
    TEST(10*  10, 100);
    TEST((10 * 10) * 10, 1000);
    TEST((4) * 2, 8);
    TEST((-4) * (-4), 16);
    TEST(10 * (-1), -10);
    TEST(10 * 0, 0);

    // Div
    TEST(10 / 2, 5);
    TEST(0 / 2, 0);
    TEST((10) / 5, 2);
    TEST((24 / 6), 4);

    // Modulo
    TEST(1 % 2, 1);
    TEST(2 % 2, 0);
    TEST(14 % 10, 4);
    TEST((14) % 10, 4);
    TEST((14 % 10), 4);
    TEST((-14) % 10, -4);
    
    // Complex expressions
    TEST(5*(4 +2), 30);
    TEST(7*(21 / (2+1)) / 7, 7);
    TEST(10*(6-8), -20);
    TEST(1+2+3, 6);
    TEST((1+2+3) / ((9 + 3) / 4), 2);
    TEST((1+2+3) / ((9 + 3) / (-4)), -2);
    TEST(12 % 5 * 3, 6);
    TEST((10 - 4) * (7 % (4 % 10)) - 8, 10);

    
    END_TESTS;

    // Value lookup
    TestProvider provider;
    MathExpr exp1("myVar * 10");
    exp1.setValueProvider(&provider);
    std::cout << "myVar * 10" << '=' << exp1.eval() << std::endl;

    MathExpr exp2("RANDOM");
    exp2.setValueProvider(&provider);
    std::cout << "RANDOM" << '=' << exp2.eval() << std::endl;

    MathExpr exp3("4 * RANDOM + myVar");
    exp3.setValueProvider(&provider);
    std::cout << "4 * RANDOM + myVar" << '=' << exp3.eval() << std::endl;
    
   

    system("PAUSE");
    return 0;
}
