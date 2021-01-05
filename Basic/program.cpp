/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include <string>
#include "program.h"
#include "statement.h"
#include "../StanfordCPPLib/tokenscanner.h"
#include "../StanfordCPPLib/error.h"
using namespace std;

Program::Program() {
    // Replace this stub with your own code
}

Program::~Program() {
    // Replace this stub with your own code
}

void Program::clear() {

    all_program.erase(all_program.begin(), all_program.end()) ;
}

void Program::addSourceLine(int lineNumber, string line) {
    // Replace this stub with your own code
    program_line temp = line ;
    all_program[lineNumber] = temp ;
}

void Program::removeSourceLine(int lineNumber) {
    // Replace this stub with your own code
    if ( !all_program.count(lineNumber) ) return;
    delete all_program[lineNumber].parsed_representation ;
    all_program[lineNumber].parsed_representation = nullptr ;
    all_program.erase(lineNumber) ;
}

string Program::getSourceLine(int lineNumber) {
    return all_program[lineNumber].source_line;    // Replace this stub with your own code
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
    // Replace this stub with your own code
    all_program[lineNumber].parsed_representation = stmt ;
}

Statement *Program::getParsedStatement(int lineNumber) {
    return all_program[lineNumber].parsed_representation;  // Replace this stub with your own code
}

int Program::getFirstLineNumber() {
    if ( all_program.empty() ) return -1 ;
    return all_program.begin()->first;     // Replace this stub with your own code
}

int Program::getNextLineNumber(int lineNumber) {
    auto it = all_program.find(lineNumber) ;
    it++ ;
    if ( it == all_program.end() ) return -1 ;
    return it->first;     // Replace this stub with your own code
}

void Program::run(EvalState &a)
{
    if ( all_program.empty() ) return ;
    auto it = all_program.begin() ;
    while ( it != all_program.end() )
    {
        try
        {
            (it->second.parsed_representation)->execute(a) ;
            it++;
        } catch ( ErrorException &ex ) //todo finish ErrorException
        {
            TokenScanner scanner ;
            if ( scanner.getTokenType(ex.getMessage()) == NUMBER )
            {
                int num1 = stringToInteger(ex.getMessage()) ;
                if ( all_program.count(num1) )
                {
                    it = this->all_program.find(num1) ;
                    continue;
                }
                error("LINE NUMBER ERROR") ;
            }
            else
                error(ex.getMessage()) ;
        }
    }
}

void Program::show_all_program()
{
    if ( all_program.empty() ) return;
    auto it = all_program.begin() ;
    while ( it != all_program.end() )
    {
        cout << it->second.source_line << endl ;
        it++ ;
    }
}
