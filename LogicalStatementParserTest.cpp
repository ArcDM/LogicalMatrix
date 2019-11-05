// LogicalStatementParserTest.cpp

/** This file is used to test the correctness of the LogicalStatementParser class
 */

#include <iostream>
#include <string>
#include <sstream>
#include "LogicalStatementParser.h"

void test( const std::string &tested, const std::string &expected )
{
    std::ostringstream stringstream;
    LogicalStatementParser test_parser( tested );
    stringstream << test_parser;

    std::cout << "Testing \"" << tested << "\" and \"" << expected << "\"" << std::endl
        << "Result: " << test_parser << std::endl << "Test "
        << ( ( expected.compare( stringstream.str() ) == 0 )? "passed" : "FAILED" ) << std::endl << std::endl;
}

int main( int argc, char const *argv[] )
{
    test( " a & b & c & d", "a & b & c & d" );
    test( " a & b & c | d ", "a & b & c | d" );
    test( "a & b | c & d ", "a & b | c & d" );
    test( "a &  b | c | d", "a & b | c | d" );
    test( "a  |  b & c & d", "a | b & c & d" );
    test( "a|b&c|d", "a | b & c | d" );
    test( "a | b | c & d", "a | b | c & d" );
    test( "a | b | c | d", "a | b | c | d" );
    /*LogicalStatementParser test1( " a & (b || c) | d" ); // = a & b | a & c | d

    LogicalStatementParser test2( "a && (b | c) & d " ); // = a & b & d | a & c & d

    LogicalStatementParser test3( "a || (b && c) & d" ); // = a | c & b & d

    LogicalStatementParser test4( "a | (c & b) | d" ); // = a | c & b | d

    LogicalStatementParser test5( "(a | c) AND (b | d)" ); // = a & b | a & d | c & b | c & d

    LogicalStatementParser test6( "(a & c) OR (b & d)" ); // = a & c | b & d

    LogicalStatementParser test7( "!(a | b) | !( c & d ) | !(!a) | !!a" ); //  = !a & !b | !c | !d | a | a

    LogicalStatementParser test8( "a & ( b | ( c & d ) )" ); // = a & b | a & c & d

    LogicalStatementParser test9( " a & ( b & ( c | d ) ) " ); //  = a & b & c | a & b & d*/

    std::cout << "End testing" << std::endl;
    return 0;
}
