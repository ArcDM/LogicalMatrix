// LogicalStatementParser.cpp

/** Implementation file for the LogicalStatementParser class.
 */

 #include "LogicalStatementParser.h"
 #include <algorithm>
 #include <iostream>

/**Order of operations
 * ( )
 * ! NOT
 * && AND
 * || OR
 */

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

static inline void replace( std::vector< std::string > &string_vector, std::vector< std::string > &replace, std::string &substitute  )
{
}

// Construct from parsing a string
LogicalStatementParser::LogicalStatementParser( const std::string &input_string )
{
    std::vector< std::string > logical_statements = split_string( input_string, "," );

    //replace( logical_statements, { "||", "OR" }, "|" );
    //replace( logical_statements, { "&&", "AND" }, "&" );

    debugprint( logical_statements );
    // input_strings = number of unique identifiers

    // reformat each logical_statements to remove ( )

    // separate each logical_statement by each OR


}

// Copy constructor
LogicalStatementParser::LogicalStatementParser( const LogicalStatementParser &object_arg )
{
}

std::string LogicalStatementParser::to_string()
{
    return "";
}

void LogicalStatementParser::print()
{
}
