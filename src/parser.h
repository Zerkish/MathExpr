// Copyright Peter Mårtensson 2014
//
//
// Math expression parser
// Supports most common operations, and can be somewhat easily extended
// through the ValueProvider API.
// With the current design supporting functions such as cos/log etc could be a problem.
// The parser was written with a very simple basic interpreter in mind.

// Given an expression such as 10 *(4 - (2 * 3)) + 12, the following happens:
// 1. All whitespace is removed.
// 2. A graph gets built, with the whole expression as the root node.
//    The graph would look like this for the above expression:
//
//               _ [10*(4-(2*3))+12] __
//             /     /     |        \   \
//            /     /      |         \   \
//           10    [*]  [4-(2*3)]    [+]  12
//                     /   |     \
//                    /    |      \
//                   4    [-]    [2*3]
//                              /  |  \
//                             /   |   \
//                            2   [*]   3
//
// The graph is the evaluated in depth first order, while prioritizing *, / and % above + and -.
//

#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>

// Base class that can be used to "fetch" values from whatever source you want.
// (To support variables etc)
class ValueProvider {
public:
    virtual ~ValueProvider() {}

    virtual int provide(const std::string &expr) const = 0;
};

class MathExpr : public ValueProvider {
public:
    friend int evalTerms(int, MathExpr*, MathExpr*);
    friend int evalTerms(int, MathExpr*, MathExpr*);
    
    explicit MathExpr(const std::string &expr);
    explicit MathExpr(int v);
    ~MathExpr();

    const ValueProvider *getValueProvider() const;
    void setValueProvider(ValueProvider *p);
    
    int eval();

    void writeTree(std::ostream &, int lvl = 0);


    inline int provide(const std::string &expr) const {
	return ValueProvider::provide(expr);
    }
private:
    enum type_t {
	OPERATOR = 1,
	VALUE
    } _type;
    
    MathExpr(const std::string &expr, bool doTokenize, type_t t, ValueProvider *);
    void tokenize();

    typedef int(*opFunction_t)(int op, MathExpr *lhs, MathExpr *rhs);

    int evalOp(const char *ops, opFunction_t func);
    
    std::vector<MathExpr*> _children;
    std::string _token;

    ValueProvider *_provider;
};

inline const ValueProvider *MathExpr::getValueProvider() const {
    return _provider;
}

inline void MathExpr::setValueProvider(ValueProvider *p) {
    _provider = p;
    for (MathExpr *c : _children) {
	c->setValueProvider(p);
    }
}


#endif /* PARSER_H */
