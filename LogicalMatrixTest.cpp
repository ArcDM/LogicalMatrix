// LogicalMatrixTest.cpp

/** This file is used to test the correctness of the LogicalMatrix class
 */

#include <chrono>
#include <iostream>
#include <string>
#include "LogicalMatrix.h"
#include "LogicalMatrix.cpp"

// used to print unique identifiers from LogicalMatrix
std::ostream &operator<<( std::ostream &output, const std::set< std::string > &object_arg )
{
    for( std::string const& value : object_arg )
    {
        output << "\"" << value << "\" ";
    }

    return output;
}

// used to print a bool vector
std::ostream &operator<<( std::ostream &output, const std::vector< bool > &object_arg )
{
    for( bool const& value : object_arg )
    {
        output << ( value? "T" : "F" );
    }

    return output;
}

// used to print a string to bool map
std::ostream &operator<<( std::ostream &output, const std::map< std::string, bool > &object_arg )
{
    bool first = true;

    if( object_arg.empty() )
    {
        output << "nothing";
    }

    for( auto const& [ key, value ] : object_arg )
    {
        if( first )
        {
            first = false;
        }
        else
        {
            output << ", ";
        }

        output << key << " = " << ( value? "T" : "F" );
    }

    return output;
}

// Testing function for a given LogicalMatrix versus the expected result
bool test( const LogicalMatrix &test_parser, const std::string &expected, const bool &display = false )
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

        std::cout << "Expected: \"" << expected << "\"" << std::endl;
        test_parser.debug_print();

        std::cout << std::endl;
    }

    return result;
}

// Testing function for a given string versus the expected result
bool test( const std::string &tested, const std::string &expected, const bool &display = false )
{
    try
    {
        LogicalMatrix test_parser( tested );

        return test( test_parser, expected, display );
    }
    catch( LogicalMatrix::Logicalstatementexception &e )
    {
        std::cout << "Testing \"" << tested << "\" and \"" << expected << "\"" << std::endl
            << "Error caught for \"" << tested << "\": \"" << e.what() << "\"" << std::endl << "Test FAILED" << std::endl << std::endl;
    }

    return false;
}

bool test_evaluate( const std::string &tested, const std::map< std::string, bool > &test_map, const std::vector< bool > &expected, const bool &display = false )
{
    try
    {
        LogicalMatrix test_parser = LogicalMatrix( tested );
        std::vector< bool > result_vector = test_parser.evaluate( test_map );
        bool result = ( result_vector == expected );

        if( display || !result )
        {
            test_parser.debug_print();
            std::cout << "Testing \"" << tested << "\" with evaluate function\nusing " << test_map << std::endl
                << result_vector << std::endl << "Test "<< ( result? "passed" : "FAILED" ) << std::endl << std::endl;
        }

        return result;
    }
    catch( LogicalMatrix::Logicalstatementexception &e )
    {
        std::cout << "Testing \"" << tested << "\" with evaluate function" << std::endl
            << "Error caught for \"" << tested << "\": \"" << e.what() << "\"" << std::endl << "Test FAILED" << std::endl << std::endl;
    }

    return false;
}

bool test_evaluate_full( const std::string &tested, const bool &display = false )
{
    bool result = true;

    try
    {
        LogicalMatrix test_matrix = LogicalMatrix( tested );
        std::set< std::string > test_values = test_matrix.get_unique_identifiers();
        size_t counter, isolator, length = 1 << test_values.size();
        std::map< std::string, bool > test_map;
        std::vector< bool > result_vector;

        if( display )
        {
            std::cout << "Testing \"" << test_matrix << "\" with evaluate function" << std::endl;
        }

        for( counter = 0; counter < length; ++counter )
        {
            isolator = counter;

            for( std::string const& key : test_values )
            {
                test_map[ key ] = ( isolator & 1 );
                isolator >>= 1;
            }

            result_vector = test_matrix.evaluate( test_map );

            if( display )
            {
                std::cout << "Test " << counter << ": " << test_map << std::endl << result_vector << std::endl << std::endl;
            }
        }
    }
    catch( LogicalMatrix::Logicalstatementexception &e )
    {
        result = false;
        std::cout << "Error caught: \"" << e.what() << "\"" << std::endl << "Tests FAILED in parsing" << std::endl << std::endl;
    }

    return result;
}

// Testing function for Logicalstatementexception during parsing
bool test_error( const std::string &tested, const bool &display = false )
{
    std::string result;

    try
    {
        LogicalMatrix test_parser( tested );

        if( test_parser.empty() )
        {
            result = "is empty";
        }
        else
        {
            result = test_parser.to_string();
        }
    }
    catch( LogicalMatrix::Logicalstatementexception &e )
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
    std::chrono::high_resolution_clock::time_point time_start = std::chrono::high_resolution_clock::now();
    bool result = true;

    if( true )
    {
        result &= test( " d & c & b & a", "a & b & c & d" );
        result &= test( " a & b & c & space space", "a & b & c & space space" );
        result &= test( "NOTaANDb&cORd", "!a & b & c | d" );
        result &= test( "a && !b || c & d ", "a & !b | c & d" );
        result &= test( "a &  b | !c | space     space", "a & b | !c | space     space" );
        result &= test( "a  |  b & c & !       d", "a | b & c & !d" );
        result &= test( "!a|b&c|!d", "!a | b & c | !d" );
        result &= test( "a | !b | ! !  !c AND d", "a | !b | !c & d" );
        result &= test( "!a | !!b OR !!!c | !!!!d", "!a | b | !c | d" );
    }

    if( true )
    {
        try
        {
            if( true )
            {
                LogicalMatrix left_value = LogicalMatrix( "a & b" );
                LogicalMatrix right_values[] = {
                    LogicalMatrix(),
                    LogicalMatrix( "c & d" ),
                    LogicalMatrix( "a & b" ),
                    LogicalMatrix( "a | a & b | a & c" )
                };

                std::string OR_expected[] = { "a & b", "a & b | c & d", "a & b", "a" };
                std::string AND_expected[] = { "a & b", "a & b & c & d", "a & b", "a & b" };

                for( short index = 0; index < 4; ++index )
                {
                    result &= test( left_value | right_values[ index ], OR_expected[ index ] )
                            & test( left_value & right_values[ index ], AND_expected[ index ] );
                }
            }

            if( true )
            {
                result &= test( LogicalMatrix( "!a & b & c" ) | LogicalMatrix( "d & !e & f" ) | LogicalMatrix( "g & h & !i" ),
                    "!a & b & c | d & !e & f | g & h & !i" );

                result &= test( LogicalMatrix( "!a | b | c" ) & LogicalMatrix( "d | !e | f" ) & LogicalMatrix( "g | h | !i" ),
                    "!a & d & g | !a & d & h | !a & d & !i | !a & !e & g | !a & !e & h | !a & !e & !i | !a & f & g | !a & f & h | !a & f & !i | b & d & g | b & d & h | b & d & !i | b & !e & g | b & !e & h | b & !e & !i | b & f & g | b & f & h | b & f & !i | c & d & g | c & d & h | c & d & !i | c & !e & g | c & !e & h | c & !e & !i | c & f & g | c & f & h | c & f & !i" );
            }

            if( true )
            {
                result &= test( !LogicalMatrix( "a & b" ), "!a | !b");
                result &= test( !LogicalMatrix( "!a | !b" ), "a & b");
                result &= test( !LogicalMatrix( "a & !b | c & d" ), "!a & !c | !a & !d | b & !c | b & !d" );
                result &= test( !LogicalMatrix( "!a & b & c | d & !e & f | g & h & !i" ),
                    "a & !d & !g | a & !d & !h | a & !d & i | a & e & !g | a & e & !h | a & e & i | a & !f & !g | a & !f & !h | a & !f & i | !b & !d & !g | !b & !d & !h | !b & !d & i | !b & e & !g | !b & e & !h | !b & e & i | !b & !f & !g | !b & !f & !h | !b & !f & i | !c & !d & !g | !c & !d & !h | !c & !d & i | !c & e & !g | !c & e & !h | !c & e & i | !c & !f & !g | !c & !f & !h | !c & !f & i" );
            }
        }
        catch( LogicalMatrix::Logicalstatementexception &e )
        {
            result = false;
            std::cout << "Error caught: \"" << e.what() << "\"" << std::endl << "Tests FAILED in parsing" << std::endl << std::endl;
        }
    }

    if( true )
    {
        result &= test( "( a | b ) & c", "a & c | b & c" );
        result &= test( " a & (b || c) | d", "a & b | a & c | d" );
        result &= test( "a && (b | c) & d ", "a & b & d | a & c & d" );
        result &= test( "a || (b && c) & d", "a | b & c & d" );
        result &= test( "a | (c & b) | d", "a | b & c | d" );
        result &= test( "(a | c) AND (b | d)", "a & b | a & d | b & c | c & d" );
        result &= test( "(a & c) OR (b & d)", "a & c | b & d" );
        result &= test( "!(a | b) | !( c & d ) | !(!a) | !!a", "!a & !b | !c | !d | a" );
        result &= test( "a & ( b | ( c & d ) )", "a & b | a & c & d" );
        result &= test( " a & ( b & ( c | d ) ) ", "a & b & c | a & b & d" );
        result &= test( "((a & b))", "a & b" );
    }

    if( true )
    {
        result &= test( "a & b, c | d", "a & b, c | d" );
        result &= test( "a & b | e & f, c | d | e & f", "a & b | e & f, e & f | c | d" );
        result &= test( "a & b, a & b | d & e, a & b | c | d, c | d", "a & b, a & b | d & e, a & b | c | d, c | d" );
        result &= test( "a | b, a & b | c & d, a & b | e, c | d | a | b, c | d, e | c | d", "a | b, a & b | c & d, a & b | e, a | b | c | d, c | d, e | c | d" );
    }

    if( true )
    {
        result &= test( "( a & b, c | d ) | e & f", "a & b | e & f, c | d | e & f" );
        result &= test( "e & f & ( a & b, c | d )", "a & b & e & f, c & e & f | d & e & f" );
        result &= test( "( a & b, c | d ) , e & f", "a & b, c | d, e & f" );
        result &= test( "( a & b, c | d ) | ( a & b, d & e )", "a & b, a & b | d & e, a & b | c | d, c | d" );
        result &= test( "( a & b, c | d ) & ( a & b, d & e )", "a & b, a & b & d & e, a & b & c | a & b & d, d & e" );
        result &= test( "( a & b, c | d ) , ( a & b, d & e )", "a & b, c | d, a & b, d & e" );
        result &= test( "( a & b, c | d ) | ( a | b, c & d, e )", "a | b, a & b | c & d, a & b | e, c | d | a | b, c | d, c | d | e" );
        result &= test( "( a & b, c | d ) & ( a | b, c & d, e )", "a & b, a & b & c & d, a & b & e, a & c | b & c | a & d | b & d, c & d, c & e | d & e" );
        result &= test( "( a & b, c | d ) , ( a | b, c & d, e )", "a & b, c | d, a | b, c & d, e" );
    }

    if( true )
    {
        try
        {
            if( true )
            {
                LogicalMatrix left_value = LogicalMatrix( "a & b" );
                LogicalMatrix right_values[] = {
                    LogicalMatrix(),
                    LogicalMatrix( "c & d" ),
                    LogicalMatrix( "a & b" ),
                    LogicalMatrix( "a & c" ),
                    LogicalMatrix( "a | a & b | a & c" )
                };

                std::string ADD_expected[] = { "a & b", "a & b, c & d", "a & b, a & b", "a & b, a & c", "a & b, a" };

                for( short index = 0; index < 5; ++index )
                {
                    result &= test( left_value + right_values[ index ], ADD_expected[ index ] );
                }
            }

            if( true )
            {
                result &= test( LogicalMatrix( "!a & b & c" ) + LogicalMatrix( "d & !e & f" ) + LogicalMatrix( "g & h & !i" ), "!a & b & c, d & !e & f, g & h & !i" );
                result &= test( LogicalMatrix( "!a & b & c" ) + LogicalMatrix( "a & !b & c" ) + LogicalMatrix( "a & b & !c" ), "!a & b & c, a & !b & c, a & b & !c" );
            }

            if( true )
            {
                LogicalMatrix left_value = LogicalMatrix( "a & b, c | d" );
                LogicalMatrix right_values[] = {
                    LogicalMatrix(),
                    LogicalMatrix( "e & f" ),
                    LogicalMatrix( "a & b, d & e" ),
                    LogicalMatrix( "a | b, c & d, e" )
                };

                std::string OR_expected[] = { "a & b, c | d", "a & b | e & f, c | d | e & f", "a & b, a & b | d & e, a & b | c | d, c | d", "a | b, a & b | c & d, a & b | e, c | d | a | b, c | d, c | d | e" };
                std::string AND_expected[] = { "a & b, c | d", "a & b & e & f, c & e & f | d & e & f", "a & b, a & b & d & e, a & b & c | a & b & d, d & e", "a & b, a & b & c & d, a & b & e, a & c | b & c | a & d | b & d, c & d, c & e | d & e" };
                std::string ADD_expected[] = { "a & b, c | d", "a & b, c | d, e & f", "a & b, c | d, a & b, d & e", "a & b, c | d, a | b, c & d, e" };

                for( short index = 0; index < 4; ++index )
                {
                    result &= test( left_value | right_values[ index ], OR_expected[ index ] )
                            & test( left_value & right_values[ index ], AND_expected[ index ] )
                            & test( left_value + right_values[ index ], ADD_expected[ index ] );
                }
            }

            if( true )
            {
                result &= test( !LogicalMatrix( "a & b, !a | !b, a & !b | c & d, !a & b & c | d & !e & f | g & h & !i" ),
                    "!a | !b, a & b, !a & !c | !a & !d | b & !c | b & !d, a & !d & !g | a & !d & !h | a & !d & i | a & e & !g | a & e & !h | a & e & i | a & !f & !g | a & !f & !h | a & !f & i | !b & !d & !g | !b & !d & !h | !b & !d & i | !b & e & !g | !b & e & !h | !b & e & i | !b & !f & !g | !b & !f & !h | !b & !f & i | !c & !d & !g | !c & !d & !h | !c & !d & i | !c & e & !g | !c & e & !h | !c & e & i | !c & !f & !g | !c & !f & !h | !c & !f & i" );
            }
        }
        catch( LogicalMatrix::Logicalstatementexception &e )
        {
            result = false;
            std::cout << "Error caught: \"" << e.what() << "\"" << std::endl << "Tests FAILED in parsing" << std::endl << std::endl;
        }
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
        result &= test_error( "! & a" );
        result &= test_error( "()" );
        result &= test_error( "" );
        result &= test_error( "," );
        result &= test_error( "a ," );
        result &= test_error( ", b" );
        result &= test_error( "a , , c & d" );
        result &= test_error( "a & , c & d" );
        result &= test_error( "a & b , | d" );
        result &= test_error( "( a & b ) ," );
        result &= test_error( "( , ) | b" );
        result &= test_error( "!, a" );
    }

    if( true )
    {
        std::map< std::string, bool > test_map = { { "a", false }, { "b", true }, { "c", true }, { "d", false }, { "e", false } };

        result &= test_evaluate( "a & b, c | d", test_map, { 0, 1 } );
        result &= test_evaluate( "a & b | e & f, c | d | e & f", test_map, { 0, 1 } );
        result &= test_evaluate( "a & b, a & b | d & e, a & b | c | d, c | d", test_map, { 0, 0, 1, 1 } );
        result &= test_evaluate( "a | b, a & b | c & d, a & b | e, c | d | a | b, c | d, e | c | d", test_map, { 1, 0, 0, 1, 1, 1 } );

        result &= test_evaluate( "a & !a", { { "a", false } }, { 0 } );
        result &= test_evaluate( "a & !a", { { "a", true } }, { 0 } );
        result &= test_evaluate( "a | !a", { { "a", false } }, { 1 } );
        result &= test_evaluate( "a | !a", { { "a", true } }, { 1 } );
        result &= test_evaluate( "a | !a", { }, { 0 } );
    }

    if( true )
    {
        result &= test_evaluate_full( "( a & b, c | d ) & ( a | b, c & d, e )", true );
    }

    std::cout << std::endl << ( result? "All tests passed" : "Tests FAILED" ) << std::endl << "\tTime elapsed: " <<
        std::chrono::duration_cast< std::chrono::duration< double > >( std::chrono::high_resolution_clock::now() - time_start ).count()
        << " seconds" << std::endl << "End testing" << std::endl;
    return 0;
}
