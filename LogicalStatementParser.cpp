// LogicalStatementParser.cpp

/** Implementation file for the LogicalStatementParser class.
 */

 #include "LogicalStatementParser.h"
 #include <algorithm>
 #include <sstream>

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

bool LogicalStatementParser::Operator::operator ==( const Operator &other )
{
    return ( negation == other.negation ) && ( label.compare( other.label ) == 0 );
}

LogicalStatementParser::Operator LogicalStatementParser::Operator::operator !()
{
    Operator result( *this );
    result.negation = !negation;
    return result;
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

/*// utility fuction to split a string by a delimiter
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
}*/

/*// utility function to trim whitespace from each element of a vector and removing any empty elements
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
}*/

/*// Debug print function
static inline void debugprint( std::vector< std::string > &string_vector )
{
    for( std::string &word : string_vector )
    {
        std::cerr << "DEBUG: #" << word << "#" << std::endl;
    }
}*/

/*// Debug print function
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
}*/

std::vector< std::string > separate_by_OR( const std::string &input_string )
{
    std::vector< std::string > result;
    std::string piece;
    size_t index, depth, last = 0, length = input_string.size();

    auto add_identifier = [&]()
    {
        if( index - last > 0 )
        {
            std::string piece = input_string.substr( last, index - last );

            if( trim( piece ) )
            {
                result.push_back( piece );
            }
        }
    };

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
                add_identifier();

                index += ( index + 1 < length && input_string[ index + 1 ] == '|' )? 1 : 0;
                last = index + 1;
                break;
            case 'O':
                if( index + 1 < length && input_string[ index + 1 ] == 'R' )
                {
                    add_identifier();

                    index += 1;
                    last = index + 1;
                }

                break;
        }
    }

    add_identifier();

    return result;
}

void LogicalStatementParser::separate_by_AND( const std::vector< std::string > OR_separated )
{
    bool negated;
    size_t index, depth, last, length;

    auto add_identifier = [&]( std::vector< LogicalStatementParser::Operator > &input_set, const std::string &input_string )
    {
        if( index - last > 0 )
        {
            std::string piece = input_string.substr( last, index - last );

            if( trim( piece ) )
            {
                input_set.push_back( Operator( piece, negated ) );
                unique_identifiers.insert( piece );
            }
        }
    };

    for( const std::string &statement : OR_separated )
    {
        std::vector< LogicalStatementParser::Operator > AND_set;
        length = statement.size();
        last = 0;
        negated = false;

        for( index = 0; index < length; ++index )
        {
            switch( statement[ index ] )
            {
                case '!':
                    negated = !negated;
                    last = index + 1;
                    break;
                case 'N':
                    if( index + 2 < length && statement[ index + 1 ] == 'O' && statement[ index + 2 ] == 'T' )
                    {
                        negated = !negated;
                        index += 2;
                        last = index + 1;
                    }

                    break;
                case '&':
                    add_identifier( AND_set, statement );

                    negated = false;
                    index += ( index + 1 < length && statement[ index + 1 ] == '&' )? 1 : 0;
                    last = index + 1;
                    break;
                case 'A':
                    if( index + 2 < length && statement[ index + 1 ] == 'N' && statement[ index + 2 ] == 'D' )
                    {
                        add_identifier( AND_set, statement );

                        negated = false;
                        index += 2;
                        last = index + 1;
                    }

                    break;
            }
        }

        add_identifier( AND_set, statement );
        operators.push_back( AND_set );
    }
}

// Construct from parsing a string
LogicalStatementParser::LogicalStatementParser( const std::string &input_string )
{
    separate_by_AND( separate_by_OR( input_string ) );
}

/*// Copy constructor
LogicalStatementParser::LogicalStatementParser( const LogicalStatementParser &other )
{
    std::vector< std::vector< Operator > > copied_operators( other.operators );
    operators = other.operators;

    std::set< std::string > copied_unique_identifiers( other.unique_identifiers );
    unique_identifiers = other.unique_identifiers;
}*/

std::set< std::string > LogicalStatementParser::get_unique_identifiers()
{
    std::set< std::string > result = unique_identifiers;
    return result;
}

LogicalStatementParser LogicalStatementParser::operator !()
{ // !((a & b) | (c & d)) = (!a | !b) & (!c | !d) = (!a & !c | !a & !d) | (!b & !c | !b & !d)
    LogicalStatementParser new_parser( *this );
    return new_parser;
}

LogicalStatementParser LogicalStatementParser::operator &( const LogicalStatementParser &other )
{
    LogicalStatementParser new_parser( *this );
    new_parser &= other;
    return new_parser;
}

LogicalStatementParser LogicalStatementParser::operator &=( const LogicalStatementParser &other )
{ // ((a & b) | (c & d)) & ((e & f) | (g & h)) = (a & b & e & f) | (a & b & g & h) | (c & d & e & f) | (c & d & g & h)
    // add other.operators to each of operators
    // add other.unique_identifiers to each unique_identifiers
    return *this;
}

LogicalStatementParser LogicalStatementParser::operator |( const LogicalStatementParser &other )
{
    LogicalStatementParser new_parser( *this );
    new_parser |= other;
    return new_parser;
}

LogicalStatementParser LogicalStatementParser::operator |=( const LogicalStatementParser &other )
{
    // add other.operators to operators
    // add other.unique_identifiers to unique_identifiers
    return *this;
}

std::string LogicalStatementParser::to_string()
{
    std::ostringstream stringstream;
    stringstream << *this;
    return stringstream.str();
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
