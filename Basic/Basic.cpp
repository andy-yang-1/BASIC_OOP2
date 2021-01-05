/*
 * File: Basic.cpp
 * ---------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the starter project for the BASIC interpreter from
 * Assignment #6.
 * [TODO: extend and correct the documentation]
 */

#include <cctype>
#include <iostream>
#include <string>
#include "exp.h"
#include "parser.h"
#include "program.h"
#include "../StanfordCPPLib/error.h"
#include "../StanfordCPPLib/tokenscanner.h"

#include "../StanfordCPPLib/simpio.h"
#include "../StanfordCPPLib/strlib.h"
using namespace std;

/* Function prototypes */

void processLine(string line, Program & program, EvalState & state);

/* Main program */

int main() {
    EvalState state;
    Program program;
//    cout << "Stub implementation of BASIC" << endl;
    while (true) {
        try {
            processLine(getLine(), program, state);
        } catch (ErrorException & ex) {
            cout << ex.getMessage() << endl;
        }
    }
    return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version,
 * the implementation does exactly what the interpreter program
 * does in Chapter 19: read a line, parse it as an expression,
 * and then print the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC tokens, such as LIST or RUN.
 */

void processLine(string line, Program & program, EvalState & state) {
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.setInput(line);
    string token ;
    if ( scanner.hasMoreTokens() ) token = scanner.nextToken() ;
    else return ;
    TokenType real_type = scanner.getTokenType(token) ; //todo be careful the sequence


    if ( real_type == NUMBER )
    {
        int line_n = stringToInteger(token) ;
        if ( line_n <= 0 ) throw ErrorException( "line num" ) ;
        if ( scanner.hasMoreTokens() )
        {
            try
            {
                Statement *stmt = parsestmt( scanner , line ) ;
                program.addSourceLine(line_n,line) ;
                program.setParsedStatement(line_n,stmt) ;
            } catch ( ErrorException &ex )
            {
                program.removeSourceLine(line_n) ;
                throw ErrorException( ex.getMessage() ) ;
            }
        }
        else program.removeSourceLine(line_n) ;
        return ;
    }

    if ( token == "LET" )
    {
        string var_name = scanner.nextToken() ;
        if ( !check_word(var_name) ) cout << "SYNTAX ERROR" << endl;
        scanner.nextToken() ;
        Expression *exp = parseExp(scanner) ;
        int value = exp->eval(state) ;
        state.setValue(var_name,value) ;
        delete exp ;
        return ;
    }

    if ( token == "PRINT" )
    {
        Expression *exp = parseExp(scanner) ;
        cout << exp->eval(state) << endl ;
        delete exp ;
        return ;
    }

    if ( token == "INPUT" )
    {
        string var_name = scanner.nextToken() , str_value ;
        int value ;
        if ( scanner.hasMoreTokens() ) throw ErrorException( "SYNTAX ERROR" ) ;
        if ( !check_word(var_name) ) throw ErrorException ( "SYNTAX ERROR" ) ;
        while (true)
        {
            try
            {
                str_value = getLine(" ? ") ;
                value = stringToInteger(str_value) ;
            }catch (...)
            {
                cout << "INVALID NUMBER" << endl ;
                continue;
            }
            break ;
        }
        state.setValue(var_name,value) ;
        return ;
    }

    if ( token == "LIST" ) { program.show_all_program() ; return ; }

    if ( token == "CLEAR" ) {
        program.clear();
        state.clear();
        return ;
    }

    if ( token == "QUIT" ) { exit(0) ; }

    if ( token == "RUN" ) {
        try{
            program.run(state) ;
        }catch(ErrorException & s){
            if(s.getMessage() == "[tag] end"){
                return;
            }else{
                error(s.getMessage());
            }
        }
        return ;
    }

    throw ErrorException( "SYNTAX ERROR" ) ;

}


