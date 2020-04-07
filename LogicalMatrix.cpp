// LogicalMatrix.cpp

/** Implementation file for the LogicalMatrix class.
 *
 *  This implementation does not handle tautologies nor contradictions such as
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
    LogicalMatrix temp_matrix, cumulative_OR, cumulative_AND;
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

        cumulative_OR |= cumulative_AND;
        cumulative_AND.clear();
    };

    auto new_statement = [&]()
    {
        OR_identifier();

        *this += cumulative_OR;
        cumulative_OR.clear();
    };

    for( index = 0; index < length; ++index )
    {
        switch( input_string[ index ] )
        {
            case ',':
                new_statement();

                last = index + 1;
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

    new_statement();
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
    size_t index, depth, old_size = OR_matrix[ 0 ].size();
    LogicalMatrix result_matrix, cumulative_AND, temp_matrix;
    std::vector< bool > temp_vector;

    auto build_operator = [ &depth, &temp_matrix ]( const bool &value, const std::string &key, const bool &conditional )
    {
        if( value )
        {
            depth += 1;
            temp_matrix.AND_matrix[ key ] = Operator( depth, conditional );
        }
    };

    auto extend_vector = [ &depth, &temp_vector ]( std::vector< bool > &input_vector )
    {
        input_vector.reserve( depth );
        std::copy( temp_vector.begin() + input_vector.size(), temp_vector.end(), std::back_inserter( input_vector ) );
    };

    for( std::vector< bool > const& statement : OR_matrix )
    {
        for( index = 0; index < old_size; ++index )
        {
            if( statement[ index ] )
            {
                depth = 0;

                for( auto const& [ key, data ] : AND_matrix )
                {
                    build_operator( data.True[ index ], key, false );
                    build_operator( data.False[ index ], key, true );
                }

                temp_vector = std::vector< bool >( depth, false );
                temp_matrix.OR_matrix.push_back( std::vector< bool >( depth, true ) );

                for( auto& [ key, data ] : temp_matrix.AND_matrix )
                {
                    extend_vector( data.True );
                    extend_vector( data.False );
                }

                cumulative_AND &= temp_matrix;
                temp_vector.clear();
                temp_matrix.clear();
            }
        }

        result_matrix += cumulative_AND;
        cumulative_AND.clear();
    }

    return result_matrix;
}

// AND yealding a new object
LogicalMatrix LogicalMatrix::operator &( const LogicalMatrix &other ) const
{
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
    std::vector< std::vector< bool > > temp_OR_vector = std::vector< std::vector< bool > >( other.OR_matrix.size() * OR_matrix.size() );

    auto stretch_vector = [ &index, &count ]( std::vector< bool > &input_vector, const int &newsize, const int &stretch_factor )
    {
        if( stretch_factor > 1 )
        {
            index = 0;
            std::vector< bool > result_vector = std::vector< bool >( newsize );

            for( bool const& value : input_vector )
            {
                for( count = 0; count < stretch_factor; ++count )
                {
                    result_vector[ index++ ] = value;
                }
            }

            input_vector = result_vector;
        }
    };

    // ensures each Operator has the correct length of data
    for( auto& [ key, data ] : AND_matrix )
    {
        stretch_vector( data.True, newsize, other_size );
        stretch_vector( data.False, newsize, other_size );
    }


    for( auto const& [ key, data ] : other.AND_matrix )
    {
        // adds each Operator from other to this if needed
        if( AND_matrix.count( key ) == 0 )
        {
            AND_matrix[ key ] = Operator( newsize );
        }

        index = 0;

        // multiplys AND_matrix with other.AND_matrix for the current key
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

    index = 0;
    count = 0;

    // multiplys OR_matrix with other.OR_matrix
    for( std::vector< bool > const& statement : OR_matrix )
    {
        for( std::vector< bool > const& other_statement : other.OR_matrix )
        {
            temp_OR_vector[ count ] = std::vector< bool >( newsize );

            for( bool const& value : statement )
            {
                for( bool const& other_value : other_statement )
                {
                    temp_OR_vector[ count ][ index++ ] = value & other_value;
                }
            }

            ++count;
        }
    }

    OR_matrix = temp_OR_vector;

    trim();

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
    return OR_helper( other, false );
}

LogicalMatrix LogicalMatrix::OR_helper( const LogicalMatrix &other, const bool &add )
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
    size_t index, count, newsize = old_size + other_size;
    bool temp_bool;

    std::vector< bool > temp_vector( other_size, false );

    auto extend_vector = [ &newsize ]( std::vector< bool > &input_vector, const std::vector< bool > &additional_vector )
    {
        input_vector.reserve( newsize );

        std::copy( additional_vector.begin(), additional_vector.end(), std::back_inserter( input_vector ) );
    };

    // extend keys found in this and not other with FALSE
    for( auto& [ key, data ] : AND_matrix )
    {
        if( other.AND_matrix.count( key ) == 0 )
        {
            extend_vector( data.True, temp_vector );
            extend_vector( data.False, temp_vector );
        }
    }

    for( auto const& [ key, data ] : other.AND_matrix )
    {
        // adds each Operator from other to this if needed
        if( AND_matrix.count( key ) == 0 )
        {
            AND_matrix[ key ] = Operator( old_size );
        }

        extend_vector( AND_matrix[ key ].True, data.True );
        extend_vector( AND_matrix[ key ].False, data.False );
    }

    if( add )
    {
        for( std::vector< bool >& statement : OR_matrix )
        {
            extend_vector( statement, temp_vector );
        }

        for( std::vector< bool > const& statement : other.OR_matrix )
        {
            OR_matrix.push_back( std::vector< bool >( old_size, false ) );
            extend_vector( OR_matrix.back(), statement );
        }
    }
    else
    {
        index = 0;
        std::vector< std::vector< bool > > temp_OR_vector = std::vector< std::vector< bool > >( other.OR_matrix.size() * OR_matrix.size() );

        for( std::vector< bool > const& statement : OR_matrix )
        {
            for( std::vector< bool > const& other_statement : other.OR_matrix )
            {
                temp_OR_vector[ index ] = statement;
                extend_vector( temp_OR_vector[ index ], other_statement );
                ++index;
            }
        }

        OR_matrix = temp_OR_vector;
    }

    trim();

    return *this;
}

LogicalMatrix LogicalMatrix::operator +( const LogicalMatrix &other ) const
{
    LogicalMatrix new_matrix( *this );
    new_matrix += other;
    return new_matrix;
}

LogicalMatrix LogicalMatrix::operator +=( const LogicalMatrix &other )
{
    return OR_helper( other, true );
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

    auto remove_subset = [&]( const int &remove_index )
    {
        for( auto& [ key, data ] : AND_matrix )
        {
            data.True.erase( data.True.begin() + remove_index );
            data.False.erase( data.False.begin() + remove_index );
        }

        for( std::vector< bool >& statement : OR_matrix )
        {
            statement.erase( statement.begin() + remove_index );
        }

        has_subset.erase( has_subset.begin() + remove_index );

        for( std::vector< bool >& each_set : has_subset )
        {
            each_set.erase( each_set.begin() + remove_index );
        }

        index -= ( remove_index < index )? 1 : 0;
        --size;
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
                if( has_subset[ index ][ inner_index ] == has_subset[ inner_index ][ index ] )
                { // B is subset of A, A is subset of B, and B == A : combine A and B, remove B
                    for( std::vector< bool >& statement : OR_matrix )
                    {
                        statement[ index ] = statement[ index ] | statement[ inner_index ];
                    }

                    remove_subset( inner_index );
                }
                else
                {
                    temp_bool = true;

                    for( std::vector< bool >& statement : OR_matrix )
                    { // check if each statement uses these indexes equivalently
                        temp_bool &= ( statement[ index ] == statement[ inner_index ] );
                    }

                    if( temp_bool ) // B is subset of A and B != A : remove B
                    {
                        remove_subset( inner_index );

                        // check to see if each Operator is still relevent
                        for( auto AND_iter = AND_matrix.begin(); AND_iter != AND_matrix.end(); )
                        {
                            temp_bool = false;

                            for( bool const& value : AND_iter->second.True )
                            {
                                temp_bool |= value;
                            }

                            for( bool const& value : AND_iter->second.False )
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
                    }
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

    for( std::vector< bool > const& statement : object_arg.OR_matrix )
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

                output << AND_streams[ index ].str();
            }
        }
    }

    return output;
}

void LogicalMatrix::debug_print() const
{
    size_t index = 0;
    std::ostringstream output;

    auto print_vector = [ &output ]( const std::vector< bool > &input_vector, const std::string &label )
    {
        output << label << ": ";

        for( bool const& value : input_vector )
        {
            output << value;
        }

        output << std::endl;
    };

    output << "AND_matrix" << std::endl;

    for( auto const& [ key, data ] : AND_matrix )
    {
        output << "Value: " << key << std::endl;
        print_vector( data.True, "T" );
        print_vector( data.False, "F" );
    }

    output << "OR_matrix" << std::endl;

    for( std::vector< bool > const& statement : OR_matrix )
    {
        print_vector( statement, std::to_string( ++index ) );
    }

    std::cout << output.str();
}
