// LogicalStatementParserTest.cpp

/** This file is used to test the correctness of the LogicalStatementParser class
 */

#include <iostream>
#include <string>
#include "LogicalStatementParser.h"
#include "LogicalStatementParser.cpp"

// used to print unique identifiers from LogicalStatementParser
std::ostream &operator<<( std::ostream &output, const std::set< std::string > &object_arg )
{
    for( auto &value : object_arg )
    {
        output << "\"" << value << "\" ";
    }

    return output;
}

// Testing function for a given LogicalStatementParser versus the expected result
bool test( const LogicalStatementParser &test_parser, const std::string &expected, const bool &display = false )
{
    bool result = expected.compare( test_parser.to_string() ) == 0;

    if( display || !result )
    {
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

        if( !result )
        {
            std::cout << "Expected: \"" << expected << "\"" << std::endl;
        }

        std::cout << std::endl;
    }

    return result;
}

// Testing function for a given string versus the expected result
bool test( const std::string &tested, const std::string &expected, const bool &display = false )
{
    try
    {
        LogicalStatementParser test_parser( tested );

        return test( test_parser, expected, display );
    }
    catch( LogicalStatementParser::Logicalstatementexception &e )
    {
        std::cout << "Testing \"" << tested << "\" and \"" << expected << "\"" << std::endl
            << "Error caught for \"" << tested << "\": \"" << e.what() << "\"" << std::endl << "Test FAILED" << std::endl << std::endl;
    }

    return false;
}

// Testing function for Logicalstatementexception during parsing
bool test_error( const std::string &tested, const bool &display = false )
{
    std::string result;

    try
    {
        LogicalStatementParser test_parser( tested );

        if( test_parser.empty() )
        {
            result = "is empty";
        }
        else
        {
            result = test_parser.to_string();
        }
    }
    catch( LogicalStatementParser::Logicalstatementexception &e )
    {
        if( display )
        {
            std::cout << "Error caught for \"" << tested << "\": \"" << e.what() << "\"" << std::endl << "Test passed" << std::endl << std::endl;
        }

        return true;
    }

    std::cout << "Result: " << result << std::endl << "No error caught for \"" << tested << "\"" << std::endl << "Test FAILED" << std::endl << std::endl;
    return false;
}

int main( int argc, char const *argv[] )
{
    bool result = true;

    if( true )
    {
        result &= test( " d & c & b & a", "a & b & c & d" );
        result &= test( " a & b & c & space space", "a & b & c & space space" );
        result &= test( " NOT a AND b & c OR d ", "!a & b & c | d" );
        result &= test( "a && !b || c & d ", "a & !b | c & d" );
        result &= test( "a &  b | !c | space     space", "a & b | !c | space     space" );
        result &= test( "a  |  b & c & !   d", "a | b & c & !d" );
        result &= test( "!a|b&c|!d", "!a | b & c | !d" );
        result &= test( "a | !b | ! !  !c & d", "a | !b | !c & d" );
        result &= test( "!a | !!b | !!!c | !!!!d", "!a | b | !c | d" );
    }

    if( true )
    {
        try
        {
            LogicalStatementParser left_value = LogicalStatementParser( "a & b" );
            LogicalStatementParser right_values[] = {
                LogicalStatementParser(),
                LogicalStatementParser( "c & d" ),
                LogicalStatementParser( "a & b" ),
                LogicalStatementParser( "a | a & b | a & c" )
            };

            std::string OR_expected[] = { "a & b", "a & b | c & d", "a & b", "a | a & b | a & c" };
            std::string AND_expected[] = { "a & b", "a & b & c & d", "a & b", "a & b | a & b & c" };

            if( true )
            {
                for( short index = 0; index < 4; ++index )
                {
                    result &= test( left_value | right_values[ index ], OR_expected[ index ] )
                            & test( left_value & right_values[ index ], AND_expected[ index ] );
                }
            }

            if( true )
            {
                result &= test( LogicalStatementParser( "!a & b & c" ) | LogicalStatementParser( "d & !e & f" ) | LogicalStatementParser( "g & h & !i" ),
                    "!a & b & c | d & !e & f | g & h & !i" );

                result &= test( LogicalStatementParser( "!a | b | c" ) & LogicalStatementParser( "d | !e | f" ) & LogicalStatementParser( "g | h | !i" ),
                    "!a & d & g | !a & d & h | !a & d & !i | !a & !e & g | !a & !e & h | !a & !e & !i | !a & f & g | !a & f & h | !a & f & !i | b & d & g | b & d & h | b & d & !i | b & !e & g | b & !e & h | b & !e & !i | b & f & g | b & f & h | b & f & !i | c & d & g | c & d & h | c & d & !i | c & !e & g | c & !e & h | c & !e & !i | c & f & g | c & f & h | c & f & !i" );
            }

            if( true )
            {
                result &= test( !LogicalStatementParser( "a & b" ), "!a | !b");
                result &= test( !LogicalStatementParser( "!a | !b" ), "a & b");
                result &= test( !LogicalStatementParser( "a & !b | c & d" ), "!a & !c | !a & !d | b & !c | b & !d" );
                result &= test( !LogicalStatementParser( "!a & b & c | d & !e & f | g & h & !i" ),
                    "a & !d & !g | a & !d & !h | a & !d & i | a & e & !g | a & e & !h | a & e & i | a & !f & !g | a & !f & !h | a & !f & i | !b & !d & !g | !b & !d & !h | !b & !d & i | !b & e & !g | !b & e & !h | !b & e & i | !b & !f & !g | !b & !f & !h | !b & !f & i | !c & !d & !g | !c & !d & !h | !c & !d & i | !c & e & !g | !c & e & !h | !c & e & i | !c & !f & !g | !c & !f & !h | !c & !f & i" );
            }
        }
        catch( LogicalStatementParser::Logicalstatementexception &e )
        {
            std::cout << "Error caught: \"" << e.what() << "\"" << std::endl << "Tests FAILED in parsing" << std::endl << std::endl;
        }
    }

    if( true )
    {
        result &= test( " a & (b || c) | d", "a & b | a & c | d" );
        result &= test( "a && (b | c) & d ", "a & b & d | a & c & d" );
        result &= test( "a || (b && c) & d", "a | b & c & d" );
        result &= test( "a | (c & b) | d", "a | b & c | d" );
        result &= test( "(a | c) AND (b | d)", "a & b | a & d | b & c | c & d" );
        result &= test( "(a & c) OR (b & d)", "a & c | b & d" );
        result &= test( "!(a | b) | !( c & d ) | !(!a) | !!a", "!a & !b | a | !c | !d" );
        result &= test( "a & ( b | ( c & d ) )", "a & b | a & c & d" );
        result &= test( " a & ( b & ( c | d ) ) ", "a & b & c | a & b & d" );
        result &= test( "((a & b))", "a & b" );
    }

    if( true )
    {
        result &= test_error( "a |" );
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
        result &= test_error( "()" );
        result &= test_error( "" );
    }

    std::cout << std::endl << ( result? "All tests passed" : "Tests FAILED" ) << std::endl << "End testing" << std::endl;
    return 0;
}
