// LogicalStatementParser.cpp

/** Implementation file for the LogicalStatementParser class.
 */

 #include "LogicalStatementParser.h"
 #include <algorithm>

/**Order of operations
 * ( )
 * ! NOT
 * && AND
 * || OR
 */


LogicalStatementParser::Operator::Operator( const std::string input_string, const bool input_negation )
{
    label = input_string;
    negation = input_negation;
}

LogicalStatementParser::Operator::Operator( const Operator &object_arg )
{
    label = std::string( object_arg.label );
    negation = object_arg.negation;
}

// Default constructor
LogicalStatementParser::LogicalStatementParser()
{
}

// credit to https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// credit to https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// credit to https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
// trim from both ends (in place) and returns if the string is still not empty
static inline bool trim(std::string &s) {
    ltrim(s);
    rtrim(s);
    return !s.empty();
}

// utility fuction to split a string by a delimiter
std::vector< std::string > split_string( const std::string &input_string, const std::string &delimiter )
{
    std::vector< std::string > result;
    std::string piece;
    size_t last = 0, length = delimiter.length();

    for( std::string::size_type found = input_string.find( delimiter ); // 2 line for statement
        found != std::string::npos; found = input_string.find( delimiter, last ) )
    {
        if( found - last > 0 )
        {
            piece = input_string.substr( last, found - last );

            if( trim( piece ) )
            {
                result.push_back( piece );
            }
        }

        last = found + length;
    }

    if( input_string.length() - last > 0 )
    {
        piece = input_string.substr( last );

        if( trim( piece ) )
        {
            result.push_back( piece );
        }
    }

    return result;
}

// utility function to trim whitespace from each element of a vector and removing any empty elements
static inline void trim( std::vector< std::string > &string_vector )
{
    std::vector< std::string >::iterator iter = string_vector.begin();

    while( iter != string_vector.end() )
    {
        trim( *iter );

        if( iter->empty() )
        {
            string_vector.erase( iter );
            continue;
        }

        ++iter;
    }
}

// Debug print function
static inline void debugprint( std::vector< std::string > &string_vector )
{
    for( std::string &word : string_vector )
    {
        std::cerr << "DEBUG: #" << word << "#" << std::endl;
    }
}

// Debug print function
static inline void debugprint( std::vector< std::vector< std::string > > &string_vector )
{
    size_t index, length;

    for( auto &statement : string_vector )
    {
        length = statement.size();

        std::cerr << "DEBUG: " << statement[ 0 ];

        for( index = 1; index < length; ++index )
        {
            std::cerr << " | " << statement[ index ];
        }

        std::cerr << std::endl;
    }
}

std::vector< std::string > separate_by_OR( const std::string &input_string )
{
    std::vector< std::string > result;
    std::string piece;
    size_t index, depth, last = 0, length = input_string.size();

    for( index = 0; index < length; ++index )
    {
        switch( input_string[ index ] )
        {
            case '(':
                depth = 1;

                for( ++index; index < length; ++index )
                {
                    if( input_string[ index ] == '(' )
                    {
                        ++depth;
                    }
                    else if( input_string[ index ] == ')' )
                    {
                        if( --depth == 0 )
                        {
                            break;
                        }
                    }
                }

                break;
            case '|':
                if( index - last > 0 )
                {
                    piece = input_string.substr( last, index - last );

                    if( trim( piece ) )
                    {
                        result.push_back( piece );
                    }
                }

                index += ( index + 1 < length && input_string[ index + 1 ] == '|' )? 2 : 1;
                last = index;
                break;
            case 'O':
                if( index + 1 < length && input_string[ index + 1 ] == 'R' )
                {
                    if( index - last > 0 )
                    {
                        piece = input_string.substr( last, index - last );

                        if( trim( piece ) )
                        {
                            result.push_back( piece );
                        }
                    }

                    index += 2;
                    last = index;
                }

                break;
        }
    }

    if( index - last > 0 )
    {
        piece = input_string.substr( last );

        if( trim( piece ) )
        {
            result.push_back( piece );
        }
    }

    return result;
}

void LogicalStatementParser::separate_by_AND( const std::vector< std::string > OR_separated )
{
    bool negated;
    std::string piece;
    size_t index, depth, last, length;

    for( const std::string &statement : OR_separated )
    {
        std::vector< LogicalStatementParser::Operator > AND_set;
        length = statement.size();
        last = 0;

        for( index = 0; index < length; ++index )
        {
            switch( statement[ index ] )
            {
                /*case '(':
                    depth = 1;

                    for( ++index; index < length; ++index )
                    {
                        if( statement[ index ] == '(' )
                        {
                            ++depth;
                        }
                        else if( statement[ index ] == ')' )
                        {
                            if( --depth == 0 )
                            {
                                break;
                            }
                        }
                    }

                    break;*/
                case '!':
                    negated = !negated;
                    break;
                case 'N':
                    if( index + 2 < length && statement[ index + 1 ] == 'O' && statement[ index + 2 ] == 'T' )
                    {
                        negated = !negated;
                    }

                    break;
                case '&':
                    if( index - last > 0 )
                    {
                        piece = statement.substr( last, index - last );

                        if( trim( piece ) )
                        {
                            AND_set.push_back( Operator( piece, negated ) );
                        }
                    }

                    negated = false;
                    index += ( index + 1 < length && statement[ index + 1 ] == '&' )? 2 : 1;
                    last = index;
                    break;
                case 'A':
                    if( index + 2 < length && statement[ index + 1 ] == 'N' && statement[ index + 2 ] == 'D' )
                    {
                        if( index - last > 0 )
                        {
                            piece = statement.substr( last, index - last );

                            if( trim( piece ) )
                            {
                                AND_set.push_back( Operator( piece, negated ) );
                            }
                        }

                        negated = false;
                        index += 3;
                        last = index;
                    }

                    break;
            }
        }

        if( index - last > 0 )
        {
            piece = statement.substr( last );

            if( trim( piece ) )
            {
                AND_set.push_back( Operator( piece, negated ) );
            }
        }

        operators.push_back( AND_set );
    }
}

// Construct from parsing a string
LogicalStatementParser::LogicalStatementParser( const std::string &input_string )
{
    separate_by_AND( separate_by_OR( input_string ) );
    /*std::cerr << input_string << std::endl;

    for( auto &value : separate_by_OR( input_string ) )
    {
        std::cerr << "#" << value;
    }

    std::cerr << "#" << std::endl;*/

    // separate each logical_statement by each OR


}

// Copy constructor
LogicalStatementParser::LogicalStatementParser( const LogicalStatementParser &object_arg )
{
}

void LogicalStatementParser::negate()
{

}

std::vector< std::string > unique_identifiers()
{
    std::vector< std::string > result;
    return result;
}

std::string LogicalStatementParser::to_string()
{
    return "";
}

std::ostream &operator<<( std::ostream &output, const LogicalStatementParser &object_arg )
{
    size_t AND_index, AND_length, OR_index, OR_length = object_arg.operators.size();

    for( OR_index = 0; OR_index < OR_length; ++OR_index )
    {
        if( OR_index > 0 )
        {
            output << " | ";
        }

        AND_length = object_arg.operators[ OR_index ].size();

        for( AND_index = 0; AND_index < AND_length; ++AND_index )
        {
            if( AND_index > 0 )
            {
                output << " & ";
            }

            if( object_arg.operators[ OR_index ][ AND_index ].negation )
            {
                output << "!";
            }

            output << object_arg.operators[ OR_index ][ AND_index ].label;
        }
    }

    return output;
}
