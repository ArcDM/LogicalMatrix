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

bool LogicalStatementParser::Operator::operator ==( const Operator &other ) const
{
    return ( negation == other.negation ) && ( label.compare( other.label ) == 0 );
}

bool LogicalStatementParser::Operator::operator <( const Operator &other ) const
{
    if( label.compare( other.label ) == 0 )
    {
        return ( other.negation )? false : negation;
    }
    else
    {
        return label < other.label;
    }
}

LogicalStatementParser::Operator LogicalStatementParser::Operator::operator !() const
{
    Operator result( *this );
    result.negation = !negation;
    return result;
}

bool LogicalStatementParser::empty() const
{
    return operators.empty();
}

void LogicalStatementParser::clear()
{
    operators.clear();
    unique_identifiers.clear();
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

std::vector< std::string > LogicalStatementParser::separate_by_OR( const std::string &input_string ) const
{
    std::vector< std::string > result;
    size_t index, depth, last = 0, length = input_string.size();

    auto add_identifier = [&]()
    {
        if( index - last > 0 )
        {
            std::string piece = input_string.substr( last, index - last );

            if( trim( piece ) )
            {
                result.push_back( piece );
                return;
            }
        }

        throw Logicalstatementexception();
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

                if( depth > 0 )
                {
                    throw Logicalstatementexception();
                }

                break;
            case ')':
                throw Logicalstatementexception();
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
    LogicalStatementParser temp_parser;
    LogicalStatementParser::operator_collection AND_set;
    size_t index, depth, last, length;
    bool negated;

    auto add_identifier = [&]( LogicalStatementParser::operator_collection &input_set, const std::string &input_string )
    {
        if( index - last > 0 )
        {
            std::string piece = input_string.substr( last, index - last );

            if( trim( piece ) )
            {
                input_set.insert( Operator( piece, negated ) );
                temp_parser.unique_identifiers.insert( piece );
                return;
            }
        }

        throw Logicalstatementexception();
    };

    for( const std::string &statement : OR_separated )
    {
        length = statement.size();
        last = 0;
        negated = false;

        for( index = 0; index < length; ++index )
        {
            switch( statement[ index ] )
            {
                case '(':
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

                    if( depth > 0 )
                    {
                        throw Logicalstatementexception();
                    }

                    // create new LogicalStatementParser with the string within the parenthesies
                    // remove string and parenthesies from statement
                    // add the new LogicalStatementParser to a set to be & with

                    break;
                case ')':
                    throw Logicalstatementexception();
                    break;
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
        temp_parser.operators.insert( AND_set );
        AND_set.clear();

        // temp_parser &= each paren_parser
        *this |= temp_parser;
        temp_parser.clear();
    }
}

// Construct from parsing a string
LogicalStatementParser::LogicalStatementParser( const std::string &input_string )
{
    separate_by_AND( separate_by_OR( input_string ) );
}

std::set< std::string > LogicalStatementParser::get_unique_identifiers() const
{
    std::set< std::string > result = unique_identifiers;
    return result;
}

LogicalStatementParser::statement_collection LogicalStatementParser::weave_operators( const statement_collection &other_operators ) const
{
    statement_collection result_operators;
    operator_collection temp_statement;

    for( auto &leftoperator : operators )
    {
        for( auto &rightoperator : other_operators )
        {
            temp_statement.insert( leftoperator.begin(), leftoperator.end() );
            temp_statement.insert( rightoperator.begin(), rightoperator.end() );
            result_operators.insert( temp_statement );
            temp_statement.clear();
        }
    }

    return result_operators;
}

LogicalStatementParser LogicalStatementParser::operator !() const
{ // !((a & !b) | (c & d)) = (!a | b) & (!c | !d) = (!a & !c | !a & !d) | (b & !c | b & !d)
    auto extract_operators = []( const LogicalStatementParser::operator_collection &input_set )
    {
        statement_collection result_operators;
        operator_collection temp_statement;

        for( auto &element : input_set )
        {
            temp_statement.insert( !element );
            result_operators.insert( temp_statement );
            temp_statement.clear();
        }

        return result_operators;
    };

    LogicalStatementParser new_parser;
    statement_collection temp_ops;
    LogicalStatementParser::statement_collection::iterator iter = operators.begin(), end = operators.end();

    new_parser.operators = extract_operators( *iter );

    for( ++iter; iter != end; ++iter )
    {
        temp_ops = extract_operators( *iter );
        new_parser.operators = new_parser.weave_operators( temp_ops );
        temp_ops.clear();
    }

    new_parser.unique_identifiers = unique_identifiers;

    return new_parser;
}

LogicalStatementParser LogicalStatementParser::operator &( const LogicalStatementParser &other ) const
{
    LogicalStatementParser new_parser;

    if( !other.empty() )
    {
        if( !empty() )
        {
            new_parser.operators = weave_operators( other.operators );
        }
        else
        {
            new_parser.operators = other.operators;
        }
    }
    else
    {
        new_parser.operators = operators;
    }


    new_parser.unique_identifiers = unique_identifiers;

    for( auto &identifier : other.unique_identifiers )
    {
        new_parser.unique_identifiers.insert( identifier );
    }

    return new_parser;
}

LogicalStatementParser LogicalStatementParser::operator &=( const LogicalStatementParser &other )
{ // ((a & b) | (c & d)) & ((e & f) | (g & h)) = (a & b & e & f) | (a & b & g & h) | (c & d & e & f) | (c & d & g & h)
    if( !other.empty() )
    {
        if( !empty() )
        {
            operators = weave_operators( other.operators );
        }
        else
        {
            operators = other.operators;
        }
    }

    for( auto &identifier : other.unique_identifiers )
    {
        unique_identifiers.insert( identifier );
    }

    return *this;
}

LogicalStatementParser LogicalStatementParser::operator |( const LogicalStatementParser &other ) const
{
    LogicalStatementParser new_parser( *this );
    new_parser |= other;
    return new_parser;
}

LogicalStatementParser LogicalStatementParser::operator |=( const LogicalStatementParser &other )
{
    if( &other == this )
    {
        return *this;
    }

    // add other.operators to operators
    for( auto &op : other.operators )
    {
        operators.insert( op );
    }

    // add other.unique_identifiers to unique_identifiers
    for( auto &identifier : other.unique_identifiers )
    {
        unique_identifiers.insert( identifier );
    }

    return *this;
}

bool LogicalStatementParser::operator ==( const LogicalStatementParser &other ) const
{
    return operators == other.operators;
}

bool LogicalStatementParser::operator <( const LogicalStatementParser &other ) const
{
    return operators < other.operators;
}

std::string LogicalStatementParser::to_string() const
{
    std::ostringstream stringstream;
    stringstream << *this;
    return stringstream.str();
}

std::ostream &operator<<( std::ostream &output, const LogicalStatementParser &object_arg )
{
    bool first = true;

    for( auto OR_separated : object_arg.operators )
    {
        if( !first )
        {
            output << " | ";
        }

        first = true;

        for( auto AND_separated : OR_separated )
        {
            if( first )
            {
                first = false;
            }
            else
            {
                output << " & ";
            }

            if( AND_separated.negation )
            {
                output << "!";
            }

            output << AND_separated.label;
        }
    }

    return output;
}
