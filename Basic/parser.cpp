/*
 * File: parser.cpp
 * ----------------
 * Implements the parser.h interface.
 */

#include <iostream>
#include <string>

#include "exp.h"
#include "parser.h"

#include "../StanfordCPPLib/error.h"
#include "../StanfordCPPLib/strlib.h"
#include "../StanfordCPPLib/tokenscanner.h"
#include "statement.h"
using namespace std;



/*
 * Implementation notes: parseExp
 * ------------------------------
 * This code just reads an expression and then checks for extra tokens.
 */

Expression *parseExp(TokenScanner & scanner) {
   Expression *exp = readE(scanner);
   if (scanner.hasMoreTokens()) {
      error("parseExp: Found extra token: " + scanner.nextToken());
   }
   return exp;
}

/*
 * Implementation notes: readE
 * Usage: exp = readE(scanner, prec);
 * ----------------------------------
 * This version of readE uses precedence to resolve the ambiguity in
 * the grammar.  At each recursive level, the parser reads operators and
 * subexpressions until it finds an operator whose precedence is greater
 * than the prevailing one.  When a higher-precedence operator is found,
 * readE calls itself recursively to read in that subexpression as a unit.
 */

Expression *readE(TokenScanner & scanner, int prec) {
   Expression *exp = readT(scanner);
   string token;
   while (true) {
      token = scanner.nextToken();
      int newPrec = precedence(token);
      if (newPrec <= prec) break;
      Expression *rhs = readE(scanner, newPrec);
      exp = new CompoundExp(token, exp, rhs);
   }
   scanner.saveToken(token);
   return exp;
}

/*
 * Implementation notes: readT
 * ---------------------------
 * This function scans a term, which is either an integer, an identifier,
 * or a parenthesized subexpression.
 */

Expression *readT(TokenScanner & scanner) {
   string token = scanner.nextToken();
   TokenType type = scanner.getTokenType(token);
   if (type == WORD) return new IdentifierExp(token);
   if (type == NUMBER) return new ConstantExp(stringToInteger(token));
   if (token != "(") error("Illegal term in expression");
   Expression *exp = readE(scanner);
   if (scanner.nextToken() != ")") {
      error("Unbalanced parentheses in expression");
   }
   return exp;
}

/*
 * Implementation notes: precedence
 * --------------------------------
 * This function checks the token against each of the defined operators
 * and returns the appropriate precedence value.
 */

int precedence(string token) {
   if (token == "=") return 1;
   if (token == "+" || token == "-") return 2;
   if (token == "*" || token == "/") return 3;
   return 0;
}

Statement *parsestmt( TokenScanner &scanner , string line )
{
    Expression *exp ;
    string token = scanner.nextToken() ;
    TokenType real_type = scanner.getTokenType(token) ;
    if ( real_type != WORD ) error( "[Waring SYNTAX ERROR]" ) ;
    if ( token == "PRINT" )
    {
        exp = parseExp(scanner) ;
        if ( exp->getType() == COMPOUND )
        {
            if(((CompoundExp *)exp)->getOp() == "="){delete exp;error("[Warning] SYNTAX ERROR");}
        }
        return new PRINT_statement(exp) ;
    }
    if ( token == "INPUT" )
    {
        if ( !scanner.hasMoreTokens() ) error("[Warning] SYNTAX ERROR") ;
        token = scanner.nextToken() ;
        real_type = scanner.getTokenType(token) ;
        if ( real_type != WORD || scanner.hasMoreTokens() ) error("[Warning] SYNTAX ERROR") ;
        return new INPUT_statement( token ) ;
    }
    if ( token == "REM" ) return new REM_statement ;
    if ( token == "END" ) return new END_statement ;
    if ( token == "LET" )
    {
        if ( !scanner.hasMoreTokens() ) error("[Warning] SYNTAX ERROR") ;
        exp = parseExp(scanner) ;
        if(exp->getType() != COMPOUND || ((CompoundExp *)exp)->getOp() != "=" || (((CompoundExp *)exp)->getLHS())->getType() != IDENTIFIER || !check_word(((IdentifierExp *)(((CompoundExp *)exp)->getLHS()))->getName())  )
        {delete exp;error("[Warning] SYNTAX ERROR");}
        return new LET_statement (exp) ;
    }
    if ( token == "GOTO" )
    {
        if ( !scanner.hasMoreTokens() ) error("[Warning] SYNTAX ERROR") ;
        token = scanner.nextToken() ;
        real_type = scanner.getTokenType(token) ;
        if ( real_type != NUMBER || scanner.hasMoreTokens() ) error("[Warning] SYNTAX ERROR") ;
        int t_line ;
        try{  t_line = stringToInteger(token) ; } catch (...) {error("[Warning] SYNTAX ERROR") ;}
        return new GOTO_statement(t_line) ;
    }
    if ( token == "IF" )
    {
        if ( !scanner.hasMoreTokens() ) error("[Warning] SYNTAX ERROR") ;
        Expression *f_st , *s_nd ;
        GOTO_statement *g_to ;
        if ( line.find('=') == string::npos )
        {
            f_st = readE(scanner) ;
            string op = scanner.nextToken() ;
            if ( op != "<" && op != ">" ) error("[Warning] SYNTAX ERROR") ;
            try { s_nd = readE(scanner) ; } catch (...) { error("[Warning] SYNTAX ERROR") ; }
            if ( ( token = scanner.nextToken()) != "THEN" ) error("[Warning] SYNTAX ERROR") ;
            real_type = scanner.getTokenType((token = scanner.nextToken())) ;
            if ( real_type != NUMBER || scanner.hasMoreTokens() ) error("[Warning] SYNTAX ERROR") ;
            int t_line ;
            try { t_line = stringToInteger(token) ; } catch (...) { error("[Warning] SYNTAX ERROR") ; }
            g_to = new GOTO_statement(t_line) ;
            return new IF_statement( op , f_st , s_nd , g_to ) ;
        }
        else
        {
            if ( line.find_first_of('=') != line.find_last_of('=') ) error("[Warning] SYNTAX ERROR") ;
            string op = "=" ;
            string sub_str ;
            while ( scanner.hasMoreTokens() )
            {
                if ( ( token = scanner.nextToken() ) == "=" ) break ;
                sub_str += token + " " ;
            }
            s_nd = readE(scanner) ;
            token = scanner.nextToken() ;
            if ( token != "THEN" ) error("[Warning] SYNTAX ERROR") ;
            token = scanner.nextToken() ;
            real_type = scanner.getTokenType(token) ;
            if ( real_type != NUMBER || scanner.hasMoreTokens() ) error("[Warning] SYNTAX ERROR") ;
            int t_line ;
            try { t_line = stringToInteger(token) ; } catch (...) { error("[Warning] SYNTAX ERROR") ; }
            scanner.setInput(sub_str) ;
            try { f_st = readE(scanner) ; } catch (...) { error("[Warning] SYNTAX ERROR") ; }
            g_to = new GOTO_statement(t_line) ;
            return new IF_statement(op,f_st,s_nd,g_to) ;
        }

    }
    error("[Warning] SYNTAX ERROR") ;
}

bool check_word ( string token )
{
    if(token == "LET" || token == "REM" || token == "PRINT" || token == "END" || token == "IF" ||
       token == "THEN" || token == "GOTO" || token == "RUN" || token == "LIST" || token == "CLEAR" ||
       token == "QUIT" || token == "HELP") return false ;
    return true ;
}
