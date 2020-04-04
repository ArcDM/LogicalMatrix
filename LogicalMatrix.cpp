// LogicalMatrix.cpp

/** Implementation file for the LogicalMatrix class.
 *
 *  This implementation does not handle collapsing of statements such as
 *      A & B | A = A
 *      A | !A = TRUE
 *      A & !A = FALSE
 */

 #include "LogicalMatrix.h"
 #include <algorithm>
 #include <sstream>

/**Order of operations
 * ( )
 * ! NOT
 * && AND
 * || OR
 */


LogicalMatrix::Operator::Operator( const size_t depth )
{
    True = std::vector< bool >( depth, false );
    False = std::vector< bool >( depth, false );
}

LogicalMatrix::Operator::Operator( const size_t depth, const bool condition )
{
    True = std::vector< bool >( depth, false );
    False = std::vector< bool >( depth, false );

    if( condition )
    {
        True[ depth - 1 ] = true;
    }
    else
    {
        False[ depth - 1 ] = true;
    }
}

bool LogicalMatrix::Operator::operator ==( const Operator &other ) const
{
    return ( True == other.True ) && ( False == other.False );
}

bool LogicalMatrix::Operator::operator <( const Operator &other ) const
{
    if( True == other.True )
    {
        return False < other.False;
    }
    else
    {
        return True < other.True;
    }
}

// not used
LogicalMatrix::Operator LogicalMatrix::Operator::operator !() const
{
    Operator result( *this );
    result.True = False;
    result.False = True;
    return result;
}

bool LogicalMatrix::empty() const
{
    return AND_matrix.empty();
}

void LogicalMatrix::clear()
{
    AND_matrix.clear();
    OR_matrix.clear();
}

// credit to https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
// trim from start (in place)
static inline void ltrim_string(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// credit to https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
// trim from end (in place)
static inline void rtrim_string(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// credit to https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
// trim from both ends (in place) and returns if the string is still not empty
static inline bool trim_string(std::string &s) {
    ltrim_string(s);
    rtrim_string(s);
    return !s.empty();
}

// Construct from parsing a string
// This is where the class parses the string
// Recursion is possible depending on the input_string
LogicalMatrix::LogicalMatrix( const std::string &input_string )
{
    size_t index, depth, last = 0, length = input_string.size();
    LogicalMatrix temp_matrix, cumulative_AND;
    bool negated = false, recursive_LSP = false, not_empty = false;

    auto PAREN_identifier = [&]()
    {
        if( index - last > 0 )
        {
            std::string piece = input_string.substr( last, index - last );

            if( trim_string( piece ) )
            {
                temp_matrix = LogicalMatrix( piece );

                if( negated )
                {
                    temp_matrix = !temp_matrix;
                    negated = false;
                }

                recursive_LSP = true;
                return;
            }
        }

        throw Logicalstatementexception();
    };

    auto AND_identifier = [&]()
    {
        if( index - last > 0 )
        {
            std::string piece = input_string.substr( last, index - last );

            not_empty = trim_string( piece );

            if( not_empty )
            {
                temp_matrix.AND_matrix[ piece ] = Operator( 1, !negated );
                temp_matrix.OR_matrix = {{ true }};
                negated = false;
            }
        }

        if( not_empty ^ recursive_LSP )
        {
            recursive_LSP = not_empty = false;
            cumulative_AND &= temp_matrix;
            temp_matrix.clear();
            return;
        }

        throw Logicalstatementexception();
    };

    auto OR_identifier = [&]()
    {
        AND_identifier();

        *this |= cumulative_AND;
        cumulative_AND.clear();
    };

    for( index = 0; index < length; ++index )
    {
        switch( input_string[ index ] )
        {
            case ',':
                // work in progress
                break;
            case '(':
                depth = 1;
                last = ++index;

                for( ; index < length; ++index )
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

                PAREN_identifier();
                last = index + 1;

                break;
            case ')':
                throw Logicalstatementexception();
                break;
            case '|': // |, ||
                OR_identifier();

                index += ( index + 1 < length && input_string[ index + 1 ] == '|' )? 1 : 0;
                last = index + 1;
                break;
            case 'O': // OR
                if( index + 1 < length && input_string[ index + 1 ] == 'R' )
                {
                    OR_identifier();

                    index += 1;
                    last = index + 1;
                }

                break;
            case '!':
                negated = !negated;
                last = index + 1;
                break;
            case 'N': // NOT
                if( index + 2 < length && input_string[ index + 1 ] == 'O' && input_string[ index + 2 ] == 'T' )
                {
                    negated = !negated;
                    index += 2;
                    last = index + 1;
                }

                break;
            case '&':
                AND_identifier();

                index += ( index + 1 < length && input_string[ index + 1 ] == '&' )? 1 : 0;
                last = index + 1;
                break;
            case 'A': // AND
                if( index + 2 < length && input_string[ index + 1 ] == 'N' && input_string[ index + 2 ] == 'D' )
                {
                    AND_identifier();

                    index += 2;
                    last = index + 1;
                }

                break;
        }
    }

    OR_identifier();
}

std::set< std::string > LogicalMatrix::get_unique_identifiers() const
{
    std::set< std::string > result;

    for( auto const& [ key, data ] : AND_matrix )
    {
        result.insert( key );
    }

    return result;
}

// Negation
// !((a & !b) | (c & d)) = (!a | b) & (!c | !d) = !a & !c | !a & !d | b & !c | b & !d
LogicalMatrix LogicalMatrix::operator !() const
{
    size_t old_size = OR_matrix[ 0 ].size();
    size_t index, depths[ old_size ] = { 0 };
    LogicalMatrix new_matrix[ old_size ];
    std::vector< bool > temp_vector;

    auto extend_vector = [&]( std::vector< bool > &input_vector, const std::vector< bool > &additional_vector, const size_t depth )
    {
        input_vector.reserve( depth );

        std::copy( additional_vector.begin() + input_vector.size(), additional_vector.end(), std::back_inserter( input_vector ) );
    };

    for( auto const& [ key, data ] : AND_matrix )
    {
        for( index = 0; index < old_size; ++index )
        {
            if( data.True[ index ] )
            {
                depths[ index ] += 1;
                new_matrix[ index ].AND_matrix[ key ] = Operator( depths[ index ], false );
            }

            if( data.False[ index ] )
            {
                depths[ index ] += 1;
                new_matrix[ index ].AND_matrix[ key ] = Operator( depths[ index ], true );
            }
        }
    }

    new_matrix[ 0 ].OR_matrix.push_back( std::vector< bool >( depths[ 0 ], true ) );
    temp_vector = std::vector< bool >( depths[ 0 ], false );

    for( auto& [ key, data ] : new_matrix[ 0 ].AND_matrix )
    {
        extend_vector( data.True, temp_vector, depths[ 0 ] );
        extend_vector( data.False, temp_vector, depths[ 0 ] );
    }

    for( index = 1; index < old_size; ++index )
    {
        new_matrix[ index ].OR_matrix.push_back( std::vector< bool >( depths[ index ], true ) );
        temp_vector = std::vector< bool >( depths[ index ], false );

        for( auto& [ key, data ] : new_matrix[ index ].AND_matrix )
        {
            extend_vector( data.True, temp_vector, depths[ index ] );
            extend_vector( data.False, temp_vector, depths[ index ] );
        }

        new_matrix[ 0 ] &= new_matrix[ index ];
        new_matrix[ index ].clear();
    }

    return new_matrix[ 0 ];
}

// AND yealding a new object
LogicalMatrix LogicalMatrix::operator &( const LogicalMatrix &other ) const
{
    if( other.empty() )
    {
        return *this;
    }

    if( empty() )
    {
        return other;
    }

    LogicalMatrix new_matrix( *this );
    new_matrix &= other;
    return new_matrix;
}

// AND assignment
// ((a & b) | (c & d)) & ((e & f) | (g & h)) = a & b & e & f | a & b & g & h | c & d & e & f | c & d & g & h
LogicalMatrix LogicalMatrix::operator &=( const LogicalMatrix &other )
{
    if( other.empty() )
    {
        return *this;
    }

    if( empty() )
    {
        *this = other;
        return *this;
    }

    size_t old_size = OR_matrix[ 0 ].size(),
        other_size = other.OR_matrix[ 0 ].size();
    size_t newsize = old_size * other_size,
        index, other_index, count;

    auto stretch_vector = [&]( std::vector< bool > &input_vector )
    {
        std::vector< bool > vector_copy( input_vector );
        input_vector = std::vector< bool >( newsize );
        index = 0;

        for( bool value : vector_copy )
        {
            for( size_t count_ = 0; count_ < other_size; ++count_ )
            {
                input_vector[ index++ ] = value;
            }
        }
    };

    for( auto& [ key, data ] : AND_matrix )
    {
        stretch_vector( data.True );
        stretch_vector( data.False );
    }

    for( auto const& [ key, data ] : other.AND_matrix )
    {
        if( AND_matrix.count( key ) == 0 )
        {
            AND_matrix[ key ] = Operator( newsize );
        }

        index = 0;

        for( count = 0; count < old_size; ++count )
        {
            for( other_index = 0; other_index < other_size ; ++other_index )
            {
                AND_matrix[ key ].True[ index ] = AND_matrix[ key ].True[ index ] | data.True[ other_index ];
                AND_matrix[ key ].False[ index ] = AND_matrix[ key ].False[ index ] | data.False[ other_index ];
                ++index;
            }
        }
    }

    stretch_vector( OR_matrix[ 0 ] );

    return *this;
}

// OR yealding a new object
LogicalMatrix LogicalMatrix::operator |( const LogicalMatrix &other ) const
{
    LogicalMatrix new_matrix( *this );
    new_matrix |= other;
    return new_matrix;
}

// OR assignment
LogicalMatrix LogicalMatrix::operator |=( const LogicalMatrix &other )
{
    if( other.empty() )
    {
        return *this;
    }

    if( empty() )
    {
        *this = other;
        return *this;
    }

    size_t old_size = OR_matrix[ 0 ].size(),
        other_size = other.OR_matrix[ 0 ].size();
    size_t newsize = old_size + other_size;
    bool temp_bool;

    std::vector< bool > extended_vector( other_size, false );

    auto extend_vector = [&]( std::vector< bool > &input_vector, const std::vector< bool > &additional_vector )
    {
        input_vector.reserve( newsize );

        std::copy( additional_vector.begin(), additional_vector.end(), std::back_inserter( input_vector ) );
    };

    // extend keys found in this and not other with FALSE
    for( auto& [ key, data ] : AND_matrix )
    {
        if( other.AND_matrix.count( key ) == 0 )
        {
            extend_vector( data.True, extended_vector );
            extend_vector( data.False, extended_vector );
        }
    }

    for( auto const& [ key, data ] : other.AND_matrix )
    {
        if( AND_matrix.count( key ) == 0 )
        {
            AND_matrix[ key ] = Operator( old_size );
        }

        extend_vector( AND_matrix[ key ].True, data.True );
        extend_vector( AND_matrix[ key ].False, data.False );
    }

    // need to account for multiple statements
    extend_vector( OR_matrix[ 0 ], other.OR_matrix[ 0 ] );

    trim();

    return *this;
}

// This function consolidates duplicate AND sets
void LogicalMatrix::trim()
{
    size_t index, inner_index, size = OR_matrix[ 0 ].size();
    bool temp_bool;

    std::vector< std::vector< bool > > has_subset( size, std::vector< bool >( size, true ) );

    auto analyze_subsets = [&]( const std::vector< bool > &input_vector )
    {
        if( !input_vector[ index ] )
        {
            for( inner_index = 0; inner_index < size; ++inner_index )
            {
                has_subset[ inner_index ][ index ] = has_subset[ inner_index ][ index ] & !input_vector[ inner_index ];
            }
        }
    };

    for( auto const& [ key, data ] : AND_matrix )
    {
        for( index = 0; index < size; ++index )
        {
            analyze_subsets( data.True );
            analyze_subsets( data.False );
        }
    }

    for( index = 0; index < size; ++index )
    {
        for( inner_index = 0; inner_index < size; ++inner_index )
        {
            if( index != inner_index && has_subset[ index ][ inner_index ] )
            {
                temp_bool = true;

                for( auto& statement : OR_matrix )
                { // check if each statement uses these indexes equivalently
                    temp_bool &= ( statement[ index ] == statement[ inner_index ] );
                }

                if( temp_bool )
                { // remove index from *this and has_subset
                    for( auto AND_iter = AND_matrix.begin(); AND_iter != AND_matrix.end(); )
                    {
                        AND_iter->second.True.erase( AND_iter->second.True.begin() + inner_index );
                        AND_iter->second.False.erase( AND_iter->second.False.begin() + inner_index );

                        temp_bool = false;

                        for( auto const& value : AND_iter->second.True )
                        {
                            temp_bool |= value;
                        }

                        for( auto const& value : AND_iter->second.False )
                        {
                            temp_bool |= value;
                        }

                        if( temp_bool )
                        {
                            ++AND_iter;
                        }
                        else
                        { // has no values
                            AND_matrix.erase( AND_iter++ );
                        }
                    }

                    for( auto& statement : OR_matrix )
                    {
                        statement.erase( statement.begin() + inner_index );
                    }

                    has_subset.erase( has_subset.begin() + inner_index );

                    for( auto& each_set : has_subset )
                    {
                        each_set.erase( each_set.begin() + inner_index );
                    }

                    index -= ( inner_index < index )? 1 : 0;
                    --size;
                }
            }
        }
    }
}

bool LogicalMatrix::operator ==( const LogicalMatrix &other ) const
{
    return ( AND_matrix == other.AND_matrix ) && ( OR_matrix == other.OR_matrix );
}

bool LogicalMatrix::operator <( const LogicalMatrix &other ) const
{
    if( AND_matrix == other.AND_matrix )
    {
        return OR_matrix < other.OR_matrix;
    }
    else
    {
        return AND_matrix < other.AND_matrix;
    }
}

std::string LogicalMatrix::to_string() const
{
    std::ostringstream stringstream;
    stringstream << *this;
    return stringstream.str();
}

std::ostream &operator<<( std::ostream &output, const LogicalMatrix &object_arg )
{
    size_t size = object_arg.OR_matrix[0].size();
    std::ostringstream AND_streams[ size ];
    std::vector< bool > AND_empty( size, true );
    bool OR_empty, output_empty = true;

    for( auto const& [ key, data ] : object_arg.AND_matrix )
    {
        for( size_t index = 0; index < size; ++index )
        {
            if( data.True[ index ] ^ data.False[ index ] )
            {
                if( AND_empty[ index ] )
                {
                    AND_empty[ index ] = false;
                }
                else
                {
                    AND_streams[ index ] << " & ";
                }

                if( data.False[ index ] )
                {
                    AND_streams[ index ] << "!";
                }

                AND_streams[ index ] << key;
            }
        }
    }

    for( auto statement : object_arg.OR_matrix )
    {
        OR_empty = true;

        if( output_empty )
        {
            output_empty = false;
        }
        else
        {
            output << ", ";
        }

        for( size_t index = 0; index < size; ++index )
        {
            if( statement[ index ] && !AND_empty[ index ] )
            {
                if( OR_empty )
                {
                    OR_empty = false;
                }
                else
                {
                    output << " | ";
                }

                //output << AND_streams[ index ].rdbuf();
                output << AND_streams[ index ].str();
            }
        }
    }

    return output;
}

void LogicalMatrix::debug_print() const
{
    auto print_vector = [&]( const std::string &label, const std::vector< bool > &input_vector )
    {
        std::cout << label << ": ";

        for( auto const& value : input_vector )
        {
            std::cout << value;
        }

        std::cout << std::endl;
    };

    std::cout << "AND_matrix" << std::endl;

    for( auto const& [ key, data ] : AND_matrix )
    {
        std::cout << "Value: " << key << std::endl;
        print_vector( "T", data.True );
        print_vector( "F", data.False );
    }

    std::cout << "OR_matrix" << std::endl;

    for( auto index = 0; index < OR_matrix.size(); ++index )
    {
        print_vector( std::to_string( index + 1 ), OR_matrix[ index ] );
    }
}
