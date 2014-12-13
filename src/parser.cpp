// Copyright Peter Mårtensson 2014

#include "parser.h"
#include <cstdlib>
#include <string>
#include <vector>
#include <cassert>


int evalFactors(int op, MathExpr *lhs, MathExpr *rhs);
int evalTerms(int op, MathExpr *lhs, MathExpr *rhs);


static int isOperator(char ch) {
    return (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%');
}

static std::string trimWs(const std::string &exp) {
    std::string str;
    for (const char ch : exp) {
	if (!(ch == ' ' || ch == '\n')) {
	    str += ch;
	}
    }
    return str;
}

static std::string intToString(int v) {
    const int bufferSize = 16;
    char *buffer = new char[bufferSize];
    sprintf(buffer, "%i", v);
    std::string s = buffer;
    delete[] buffer;
    return s;
}


int ValueProvider::provide(const std::string &expr) const {
    return atoi(expr.c_str());
}

MathExpr::MathExpr(const std::string &expr)  {
    _token = trimWs(expr);
    tokenize();
    _type = VALUE;
    _provider = this;
}

MathExpr::MathExpr(int value) {
    _token = intToString(value);
    _type = VALUE;
    _provider = this;
}

MathExpr::MathExpr(const std::string &expr, bool doTokenize, MathExpr::type_t t, ValueProvider *p) {
    _token = trimWs(expr);
    if (doTokenize) {
	tokenize();
    }
    _type = t;
    _provider = p;
}

MathExpr::~MathExpr() {
    for (MathExpr* c : _children) {
	delete c;
    }
}

int MathExpr::eval() {
    evalOp("*/%", evalFactors);
    return evalOp("+-", evalTerms);
}

void MathExpr::writeTree(std::ostream &o, int lvl) {
    for (int i = 0; i < lvl; i++) {
	o << "\t";
    }

    o << _token << std::endl;

    for (MathExpr *c : _children) {
	c->writeTree(o, lvl + 1);
    }
}

void MathExpr::tokenize() {
    int parenc = 0;
    char ch;

    std::string expr;

    for (int i = 0; i < _token.length(); i++) {
	ch = _token[i];
	expr += ch;

	if (ch == '(') {
	    ++parenc;
	} else if (ch == ')') {
	    --parenc;
	    if (parenc == 0) {
		_children.push_back(new MathExpr(expr.substr(1, expr.length() - 2), true, VALUE, _provider));
		expr = "";
	    }
	} else if (isOperator(ch) && parenc == 0) {
	    if (expr.length() > 1) {
		_children.push_back(new MathExpr(expr.substr(0, expr.length() - 1), false, VALUE, _provider));
	    }
	    _children.push_back(new MathExpr(std::string() + ch, false, OPERATOR, _provider));
	    expr = "";
	}
    }

    if (expr.length() > 0) {
	_children.push_back(new MathExpr(expr, false, VALUE, _provider));
    }
}

int MathExpr::evalOp(const char *ops, opFunction_t func) {
    int value = 0;

    if (_children.size() == 0) {
	value = _provider->provide(_token);
    } else if (_children.size() == 1) {
	value = _children[0]->eval();
    } else {
	for (auto it = _children.begin(); it != _children.end(); ++it) {

	    if ((*it)->_type != OPERATOR) continue;
	    
	    // Find a matching operator if any.
	    for (int i = 0; i < strlen(ops); i++) {
		if (ops[i] == (*it)->_token[0]) {
		    MathExpr *lhs = *(it - 1);
		    MathExpr *rhs = *(it + 1);

		    const int op = ops[i];

		    value = func(op, lhs, rhs);

		    --it;
		    delete *it;
		    it = _children.erase(it);
		    delete *it;
		    it = _children.erase(it);
		    delete *it;

		    *it = new MathExpr(intToString(value), false, VALUE, _provider);
		    break;
		}
	    }
	}
    }

    return value;
}

int evalTerms(int op, MathExpr *lhs, MathExpr *rhs) {
    if (op == '+') {
	return lhs->eval() + rhs->eval();
    } else if (op == '-') {
	return lhs->eval() - rhs->eval();
    }

    assert(0 && "Should never reach this!");
    return 0;
}

int evalFactors(int op, MathExpr *lhs, MathExpr *rhs) {
    if (op == '*') {
	return lhs->eval() * rhs->eval();
    } else if (op == '/') {
	return lhs->eval() / rhs->eval();
    } else if (op == '%') {
	return lhs->eval() % rhs->eval();
    }

    assert(0 && "Should never reach this!");
    return 0;
}
