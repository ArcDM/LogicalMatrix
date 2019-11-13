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

bool test( const LogicalStatementParser &test_parser, const std::string &expected )
{
    bool result = expected.compare( test_parser.to_string() ) == 0;

    std::cout << "Result: ";

    if( test_parser.empty() )
    {
        std::cout << "is empty";
    }
    else
    {
        std::cout << test_parser;
    }

    std::cout << std::endl << "Test "<< ( result? "passed" : "FAILED" ) << std::endl
        << "Unique Identifiers: "; // spaced to comment out

    if( test_parser.empty() )
    {
        std::cout << "none" << std::endl;
    }
    else
    {
        std::cout << test_parser.get_unique_identifiers() << std::endl;
    }

    return result;
}

bool test( const std::string &tested, const std::string &expected )
{
    LogicalStatementParser test_parser( tested );

    std::cout << "Testing \"" << tested << "\" and \"" << expected << "\"" << std::endl;
    return test( test_parser, expected );
}

int main( int argc, char const *argv[] )
{
    bool result;

    if( true )
    {
        result = test( " a & b & c & space space", "a & b & c & space space" );
        result &= test( " NOT a AND b & c OR d ", "!a & b & c | d" );
        result &= test( "a & !b | c & d ", "a & !b | c & d" );
        result &= test( "a &  b | !c | space     space", "a & b | !c | space     space" );
        result &= test( "a  |  b & c & !   d", "a | b & c & !d" );
        result &= test( "!a|b&c|!d", "!a | b & c | !d" );
        result &= test( "a | !b | ! !  !c & d", "a | !b | !c & d" );
        result &= test( "!a | !!b | !!!c | !!!!d", "!a | b | !c | d" );

        std::cout << std::endl << "Tests "<< ( result? "passed" : "FAILED" ) << std::endl << std::endl;
    }

    if( true )
    {
        result = test( LogicalStatementParser( "a & b" ) | LogicalStatementParser( "c & d" ), "a & b | c & d" );
        result &= test( LogicalStatementParser( "a & b" ) | LogicalStatementParser( "a & b" ), "a & b" );
        result &= test( LogicalStatementParser( "a & b" ) | LogicalStatementParser( "a | a & b | a & c" ), "a | a & b | a & c" );
        result &= test( LogicalStatementParser( "!a & b & c" ) | LogicalStatementParser( "d & !e & f" ) | LogicalStatementParser( "g & h & !i" ),
            "!a & b & c | d & !e & f | g & h & !i" );

        std::cout << std::endl << "Tests "<< ( result? "passed" : "FAILED" ) << std::endl << std::endl;
    }

    if( true )
    {
        result = test( LogicalStatementParser( "a & b" ) & LogicalStatementParser( "c & d" ), "a & b & c & d" );
        result &= test( LogicalStatementParser( "a & b" ) & LogicalStatementParser( "a & b" ), "a & b" );
        result &= test( LogicalStatementParser( "a & b" ) & LogicalStatementParser( "a | a & b | a & c" ), "a & b | a & b & c" );
        result &= test( LogicalStatementParser( "!a | b | c" ) & LogicalStatementParser( "d | !e | f" ) & LogicalStatementParser( "g | h | !i" ),
            "!a & d & g | !a & d & h | !a & d & !i | !a & !e & g | !a & !e & h | !a & !e & !i | !a & f & g | !a & f & h | !a & f & !i | b & d & g | b & d & h | b & d & !i | b & !e & g | b & !e & h | b & !e & !i | b & f & g | b & f & h | b & f & !i | c & d & g | c & d & h | c & d & !i | c & !e & g | c & !e & h | c & !e & !i | c & f & g | c & f & h | c & f & !i" );

        std::cout << std::endl << "Tests "<< ( result? "passed" : "FAILED" ) << std::endl << std::endl;
    }

    if( true )
    {
        result = test( !LogicalStatementParser( "a & b" ), "!a | !b");
        result &= test( !LogicalStatementParser( "!a | !b" ), "a & b");
        result &= test( !LogicalStatementParser( "a & !b | c & d" ), "!a & !c | !a & !d | b & !c | b & !d" );
        result &= test( !LogicalStatementParser( "!a & b & c | d & !e & f | g & h & !i" ),
            "a & !d & !g | a & !d & !h | a & !d & i | a & e & !g | a & e & !h | a & e & i | a & !f & !g | a & !f & !h | a & !f & i | !b & !d & !g | !b & !d & !h | !b & !d & i | !b & e & !g | !b & e & !h | !b & e & i | !b & !f & !g | !b & !f & !h | !b & !f & i | !c & !d & !g | !c & !d & !h | !c & !d & i | !c & e & !g | !c & e & !h | !c & e & i | !c & !f & !g | !c & !f & !h | !c & !f & i" );

        std::cout << std::endl << "Tests "<< ( result? "passed" : "FAILED" ) << std::endl << std::endl;
    }

    /*LogicalStatementParser test1( " a & (b || c) | d" ); // = a & b | a & c | d

    LogicalStatementParser test2( "a && (b | c) & d " ); // = a & b & d | a & c & d

    LogicalStatementParser test3( "a || (b && c) & d" ); // = a | c & b & d

    LogicalStatementParser test4( "a | (c & b) | d" ); // = a | c & b | d

    LogicalStatementParser test5( "(a | c) AND (b | d)" ); // = a & b | a & d | c & b | c & d

    LogicalStatementParser test6( "(a & c) OR (b & d)" ); // = a & c | b & d

    LogicalStatementParser test7( "!(a | b) | !( c & d ) | !(!a) | !!a" ); //  = !a & !b | !c | !d | a

    LogicalStatementParser test8( "a & ( b | ( c & d ) )" ); // = a & b | a & c & d

    LogicalStatementParser test9( " a & ( b & ( c | d ) ) " ); //  = a & b & c | a & b & d*/

    std::cout << "End testing" << std::endl;
    return 0;
}
