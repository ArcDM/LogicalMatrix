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
    try
    {
        LogicalStatementParser test_parser( tested );

        std::cout << "Testing \"" << tested << "\" and \"" << expected << "\"" << std::endl;
        return test( test_parser, expected );
    }
    catch( LogicalStatementParser::Logicalstatementexception &e )
    {
        std::cout << "Error caught for \"" << tested << "\": \"" << e.what() << "\"" << std::endl << "Test FAILED" << std::endl;
    }

    return false;
}

bool test_error( const std::string &tested )
{
    try
    {
        LogicalStatementParser test_parser( tested );

        std::cout << "Result: ";

        if( test_parser.empty() )
        {
            std::cout << "is empty" << std::endl;
        }
        else
        {
            std::cout << test_parser << std::endl;
        }
    }
    catch( LogicalStatementParser::Logicalstatementexception &e )
    {
        std::cout << "Error caught for \"" << tested << "\": \"" << e.what() << "\"" << std::endl << "Test passed" << std::endl;
        return true;
    }

    std::cout << "No error caught for \"" << tested << "\"" << std::endl << "Test FAILED" << std::endl;
    return false;
}

int main( int argc, char const *argv[] )
{
    bool result;

    if( true )
    {
        result = test( " a & b & c & space space", "a & b & c & space space" );
        result &= test( " NOT a AND b & c OR d ", "!a & b & c | d" );
        result &= test( "a && !b || c & d ", "a & !b | c & d" );
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

    if( true )
    {
        result = test( " a & (b || c) | d", "a & b | a & c | d" );
        result &= test( "a && (b | c) & d ", "a & b & d | a & c & d" );
        result &= test( "a || (b && c) & d", "a | c & b & d" );
        result &= test( "a | (c & b) | d", "a | c & b | d" );
        result &= test( "(a | c) AND (b | d)", "a & b | a & d | c & b | c & d" );
        result &= test( "(a & c) OR (b & d)", "a & c | b & d" );
        result &= test( "!(a | b) | !( c & d ) | !(!a) | !!a", "!a & !b | !c | !d | a" );
        result &= test( "a & ( b | ( c & d ) )", "a & b | a & c & d" );
        result &= test( " a & ( b & ( c | d ) ) ", "a & b & c | a & b & d" );

        std::cout << std::endl << "Tests "<< ( result? "passed" : "FAILED" ) << std::endl << std::endl;
    }

    if( true )
    {
        result = test_error( "a |" );
        result &= test_error( "| b" );
        result &= test_error( "a &" );
        result &= test_error( "& b" );
        result &= test_error( "& b | c & d" );
        result &= test_error( "a & | c & d" );
        result &= test_error( "a & b | & d" );
        result &= test_error( "a & b | c &" );
        result &= test_error( "a & b |" );
        result &= test_error( "| c & d" );
        result &= test_error( "( a" );
        result &= test_error( "a )" );
        result &= test_error( "!" );

        std::cout << std::endl << "Tests "<< ( result? "passed" : "FAILED" ) << std::endl << std::endl;
    }

    std::cout << "End testing" << std::endl;
    return 0;
}
