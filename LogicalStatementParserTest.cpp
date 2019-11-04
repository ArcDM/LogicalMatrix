// LogicalStatementParserTest.cpp

/** This file is used to test the correctness of the LogicalStatementParser class
 */

#include <iostream>
#include <string>
#include "LogicalStatementParser.h"

void test( std::string &tested, std::string &expected )
{
    std::cout << tested << std::endl << "Test " << ( tested.compare( expected ) == 0 )? "passed" : "FAILED";
}

int main( int argc, char const *argv[] )
{
    LogicalStatementParser test1( " a & (b || c) | d" ); // = a & b | a & c | d

    LogicalStatementParser test2( "a && (b | c) & d " ); // = a & b & d | a & c & d

    LogicalStatementParser test3( "a || (b && c) & d" ); // = a | c & b & d

    LogicalStatementParser test4( "a | (c & b) | d" ); // = a | c & b | d

    LogicalStatementParser test5( "(a | c) & (b | d)" ); // = a & b | a & d | c & b | c & d

    LogicalStatementParser test6( "(a & c) | (b & d)" ); // = a & c | b & d

    LogicalStatementParser test7( "!(a | b) | !( c & d ) | !(!a) | !!a" ); //  = !a & !b | !c | !d | a | a

    LogicalStatementParser test8( "a & ( b | ( c & d ) )" ); // = a & b | a & c & d

    LogicalStatementParser test9( " a & ( b & ( c | d ) ) " ); //  = a & b & c | a & b & d

    std::cout << "End testing" << std::endl;
    return 0;
}
