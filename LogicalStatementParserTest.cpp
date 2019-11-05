// LogicalStatementParserTest.cpp

/** This file is used to test the correctness of the LogicalStatementParser class
 */

#include <iostream>
#include <string>
#include "LogicalStatementParser.h"

// used to print unique identifiers from LogicalStatementParser
std::ostream &operator<<( std::ostream &output, const std::set< std::string > &object_arg )
{
    for( auto &value : object_arg )
    {
        output << "\"" << value << "\" ";
    }

    return output;
}

void test( const std::string &tested, const std::string &expected )
{
    LogicalStatementParser test_parser( tested );

    std::cout << "Testing \"" << tested << "\" and \"" << expected << "\"" << std::endl
        << "Result: " << test_parser << std::endl
        << "Test "<< ( ( expected.compare( test_parser.to_string() ) == 0 )? "passed" : "FAILED" ) << std::endl
        << "Unique Identifiers: " << test_parser.get_unique_identifiers() << std::endl // spaced to comment out
        << std::endl;
}

int main( int argc, char const *argv[] )
{
    test( " a & b & c & d", "a & b & c & d" );
    test( " NOT a AND b & c OR d ", "!a & b & c | d" );
    test( "a & !b | c & d ", "a & !b | c & d" );
    test( "a &  b | !c | d", "a & b | !c | d" );
    test( "a  |  b & c & !   d", "a | b & c & !d" );
    test( "!a|b&c|!d", "!a | b & c | !d" );
    test( "a | !b | ! !  !c & d", "a | !b | !c & d" );
    test( "!a | !!b | !!!c | !!!!d", "!a | b | !c | d" );
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
