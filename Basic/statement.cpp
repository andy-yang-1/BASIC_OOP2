/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include <string>
#include "statement.h"
#include "../StanfordCPPLib/simpio.h"

using namespace std;

/* Implementation of the Statement class */

Statement::Statement() {
    /* Empty */
}

Statement::~Statement() {
    /* Empty */
}

REM_statement::REM_statement() {} ;
REM_statement::~REM_statement() {} ;
void REM_statement::execute(EvalState &state) {} ;

INPUT_statement::INPUT_statement(string &a) { var_name = a ; }
INPUT_statement::~INPUT_statement() {} ;

void INPUT_statement::execute(EvalState &state)
{
    int temp_value = 0 ;
    string value_str ;
    while(true)
    {
        try
        {
            value_str = getLine( " ? " ) ;
            temp_value = stringToInteger( value_str ) ;
        } catch (...)
        {
            cout << "INVALID NUMBER" << endl ;
            continue;
        }
        break;
    }
    state.setValue( var_name , temp_value ) ;
}

PRINT_statement::PRINT_statement(Expression *a)
{
    exp = a ;
}

PRINT_statement::~PRINT_statement() { delete exp ; }

void PRINT_statement::execute(EvalState &state) { cout << exp->eval(state) << endl ; }

END_statement::END_statement() {} ;
END_statement::~END_statement() {} ;
void END_statement::execute(EvalState &state) { error("[tag] end") ; }

LET_statement::LET_statement( Expression *a ) { exp = a ; }
LET_statement::~LET_statement() { delete exp ; }
void LET_statement::execute(EvalState &state) { exp->eval(state) ; }

GOTO_statement::GOTO_statement( int a ) { next_line = a ; }
GOTO_statement::~GOTO_statement() {  } ;
void GOTO_statement::execute(EvalState &state) { error( integerToString(next_line) ) ; }

IF_statement::IF_statement(string &a, Expression *b, Expression *c, GOTO_statement *d) : op(a) , _first(b) , _second(c) , go_to(d) {} ;
IF_statement::~IF_statement()
{
    delete _first ;
    delete _second ;
    delete go_to ;
}
void IF_statement::execute(EvalState &state)
{
    bool flag = 0 ;
    int real_first = _first->eval(state) , real_second = _second->eval(state) ;
    if ( op == "=" ) flag = ( real_first == real_second ) ;
    if ( op == "<" ) flag = ( real_first < real_second ) ;
    if ( op == ">" ) flag = ( real_first > real_second ) ;
    if ( !flag ) return ;
    else go_to->execute(state) ;
}
