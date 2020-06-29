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


LogicalMatrix::TruthTable::TruthTable( const size_t depth )
{
    True = std::vector< bool >( depth, false );
    False = std::vector< bool >( depth, false );
}

LogicalMatrix::TruthTable::TruthTable( const size_t depth, const bool condition )
{
    True = std::vector< bool >( depth, false );
    False = std::vector< bool >( depth, false );

    True[ depth - 1 ] = condition? true : false;
    False[ depth - 1 ] = condition? false : true;
}

bool LogicalMatrix::TruthTable::operator ==( const TruthTable &other ) const
{
    return ( True == other.True ) && ( False == other.False );
}

bool LogicalMatrix::TruthTable::operator <( const TruthTable &other ) const
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
LogicalMatrix::TruthTable LogicalMatrix::TruthTable::operator !() const
{
    TruthTable result( *this );
    result.True = False;
    result.False = True;
    return result;
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

template< typename Type >
static inline void extend_vector( std::vector< Type > &input_vector, const std::vector< Type > &additional_vector, const size_t newsize )
{
    size_t offset = input_vector.size() + additional_vector.size() - newsize;
    input_vector.reserve( newsize );

    std::copy( additional_vector.begin() + offset, additional_vector.end(), std::back_inserter( input_vector ) );
}

template< typename Type >
static inline std::vector< Type > stretch_vector( const std::vector< Type > &input_vector, const size_t &newsize, const size_t &stretch_factor )
{
    if( stretch_factor <= 1 )
    {
        return input_vector;
    }

    size_t count, index = 0;
    std::vector< Type > result_vector = std::vector< Type >( newsize );

    for( Type const& value : input_vector )
    {
        for( count = 0; count < stretch_factor; ++count )
        {
            result_vector[ index++ ] = value;
        }
    }

    return result_vector;
}

LogicalMatrix LogicalMatrix::build_inverse( const size_t &index ) const
{
    size_t depth = 0;
    LogicalMatrix temp_matrix;

    auto build_operator = [ &depth, &temp_matrix ]( const bool &value, const std::string &key, const bool &conditional )
    {
        if( value )
        {
            depth += 1;
            temp_matrix.AND_matrix[ key ] = TruthTable( depth, conditional );
        }
    };

    for( auto const& [ key, data ] : AND_matrix )
    {
        build_operator( data.True[ index ], key, false );
        build_operator( data.False[ index ], key, true );
    }

    std::vector< bool > temp_vector = std::vector< bool >( depth, false );
    temp_matrix.OR_matrix.push_back( std::vector< bool >( depth, true ) );

    for( auto& [ key, data ] : temp_matrix.AND_matrix )
    {
        extend_vector( data.True, temp_vector, depth );
        extend_vector( data.False, temp_vector, depth );
    }

    return temp_matrix;
}

void LogicalMatrix::extend_matrix( const LogicalMatrix &other )
{
    if( other.empty() )
    {
        return;
    }

    if( empty() )
    {
        *this = other;
        return;
    }

    size_t old_size = OR_matrix[ 0 ].size(),
        other_size = other.OR_matrix[ 0 ].size();
    size_t newsize = old_size + other_size;

    std::vector< bool > temp_vector( other_size, false );

    // extend keys found in this and not other with FALSE
    for( auto& [ key, data ] : AND_matrix )
    {
        if( other.AND_matrix.count( key ) == 0 )
        {
            extend_vector( data.True, temp_vector, newsize );
            extend_vector( data.False, temp_vector, newsize );
        }
    }

    for( auto const& [ key, data ] : other.AND_matrix )
    {
        // adds each TruthTable from other to this if needed
        if( AND_matrix.count( key ) == 0 )
        {
            AND_matrix[ key ] = TruthTable( old_size );
        }

        extend_vector( AND_matrix[ key ].True, data.True, newsize );
        extend_vector( AND_matrix[ key ].False, data.False, newsize );
    }
}

// This function consolidates duplicate AND sets
void LogicalMatrix::trim()
{
    if( AND_matrix.empty() || OR_matrix.empty() )
    {
        if( !AND_matrix.empty() || !OR_matrix.empty() )
        {
            clear();
        }

        return;
    }

    size_t index, inner_index, size = OR_matrix[ 0 ].size();
    bool temp_bool;

    std::vector< std::vector< bool > > is_subset;

    auto analyze_subsets = [ &size, &index, &inner_index, &is_subset ]( const std::vector< bool > &input_vector )
    {
        if( !input_vector[ index ] )
        {
            for( inner_index = 0; inner_index < size; ++inner_index )
            {
                is_subset[ inner_index ][ index ] = is_subset[ inner_index ][ index ] & !input_vector[ inner_index ];
            }
        }
    };

    auto remove_subset = [ &is_subset, &index ]( const size_t &remove_index )
    {
        is_subset.erase( is_subset.begin() + remove_index );

        for( std::vector< bool >& each_set : is_subset )
        {
            each_set.erase( each_set.begin() + remove_index );
        }

        index -= ( remove_index < index )? 1 : 0;
    };

    auto remove_ANDset = [ &temp_bool, &size, this ]( size_t &remove_index )
    {
        size_t sub_index;

        for( auto AND_iter = AND_matrix.begin(); AND_iter != AND_matrix.end(); )
        {
            temp_bool = false;

            if( AND_iter->second.True[ remove_index ] | AND_iter->second.False[ remove_index ] )
            { // check if remove_index is the only significant value for AND_iter
                for( sub_index = 0; sub_index < size; ++sub_index )
                {
                    if( sub_index != remove_index )
                    {
                        temp_bool |= AND_iter->second.True[ sub_index ] | AND_iter->second.False[ sub_index ];
                    }
                }

                temp_bool = !temp_bool;
            }

            if( temp_bool )
            { // has no other significant values
                AND_matrix.erase( AND_iter++ );
            }
            else
            {
                AND_iter->second.True.erase( AND_iter->second.True.begin() + remove_index );
                AND_iter->second.False.erase( AND_iter->second.False.begin() + remove_index );

                ++AND_iter;
            }
        }

        for( std::vector< bool >& statement : OR_matrix )
        {
            statement.erase( statement.begin() + remove_index );
        }

        --remove_index;
        --size;
    };

    for( index = 0; index < size; ++index )
    {
        temp_bool = false;

        for( std::vector< bool > const& statement : OR_matrix )
        {
            temp_bool |= statement[ index ];
        }

        if( !temp_bool ) // index is not used for any statement
        {
            remove_ANDset( index );
        }
    }

    is_subset = std::vector< std::vector< bool > >( size, std::vector< bool >( size, true ) );

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
            if( index != inner_index && is_subset[ index ][ inner_index ] )
            {
                if( is_subset[ inner_index ][ index ] )
                { // A is subset of B, B is subset of A, and A == B : combine A and B, remove B
                    for( std::vector< bool >& statement : OR_matrix )
                    {
                        statement[ index ] = statement[ index ] | statement[ inner_index ];
                    }

                    remove_subset( inner_index );
                    remove_ANDset( inner_index );
                }
                else
                { // A is subset of B and A != B
                    temp_bool = false;

                    for( std::vector< bool >& statement : OR_matrix )
                    { // if A then remove B
                        statement[ inner_index ] = statement[ index ]? false : statement[ inner_index ];
                        temp_bool |= statement[ inner_index ];
                    }

                    if( !temp_bool ) // B is empty
                    {
                        remove_subset( inner_index );
                        remove_ANDset( inner_index );
                    }
                }
            }
        }
    }
}

// Construct from parsing a string
// This is where the class parses the string
// Recursion is possible depending on the input_string
LogicalMatrix::LogicalMatrix( const std::string &input_string )
{
    size_t index, depth, last = 0, length = input_string.size();
    LogicalMatrix temp_matrix, cumulative_OR, cumulative_AND;
    bool negated = false, recursive_LSP = false, not_empty = false;

    auto PAREN_identifier = [ & ]()
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

    auto AND_identifier = [ & ]()
    {
        if( index - last > 0 )
        {
            std::string piece = input_string.substr( last, index - last );

            not_empty = trim_string( piece );

            if( not_empty )
            {
                temp_matrix.AND_matrix[ piece ] = TruthTable( 1, !negated );
                temp_matrix.OR_matrix.push_back( { true } );
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

    auto OR_identifier = [ & ]()
    {
        AND_identifier();

        cumulative_OR |= cumulative_AND;
        cumulative_AND.clear();
    };

    auto new_statement = [ & ]()
    {
        OR_identifier();

        *this += cumulative_OR;
        cumulative_OR.clear();
    };

    for( index = 0; index < length; ++index )
    {
        switch( input_string[ index ] )
        {
            case '\n':
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

// Negation
// !((a & !b) | (c & d)) = (!a | b) & (!c | !d) = !a & !c | !a & !d | b & !c | b & !d
LogicalMatrix LogicalMatrix::operator !() const
{
    if( empty() )
    {
        return LogicalMatrix();
    }

    size_t index, old_size = OR_matrix[ 0 ].size();
    LogicalMatrix result_matrix, cumulative_AND, temp_matrix[ old_size ];

    for( index = 0; index < old_size; ++index )
    {
        temp_matrix[ index ] = build_inverse( index );
    }

    for( std::vector< bool > const& statement : OR_matrix )
    {
        for( index = 0; index < old_size; ++index )
        {
            if( statement[ index ] )
            {
                cumulative_AND &= temp_matrix[ index ];
            }
        }

        result_matrix += cumulative_AND;
        cumulative_AND.clear();
    }

    return result_matrix;
}

LogicalMatrix LogicalMatrix::NOT()
{
    *this = !(*this);
    return *this;
}

// Negate specific statement
LogicalMatrix LogicalMatrix::NOT( const size_t &statement_index )
{
    if( statement_index >= statement_count() )
    {
        return *this;
    }

    size_t index, old_size = OR_matrix[ 0 ].size();
    LogicalMatrix cumulative_AND;

    for( index = 0; index < old_size; ++index )
    {
        if( OR_matrix[ statement_index ][ index ] )
        {
            OR_matrix[ statement_index ][ index ] = false;

            cumulative_AND &= build_inverse( index );
        }
    }

    OR_matrix.erase( OR_matrix.begin() + statement_index );

    return ADD( cumulative_AND, statement_index );
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
    std::vector< std::vector< bool > > temp_OR_vector = std::vector< std::vector< bool > >( other.statement_count() * statement_count(), std::vector< bool >( newsize ) );

    // ensures each TruthTable has the correct length of data
    for( auto& [ key, data ] : AND_matrix )
    {
        data.True = stretch_vector( data.True, newsize, other_size );
        data.False = stretch_vector( data.False, newsize, other_size );
    }

    for( auto const& [ key, data ] : other.AND_matrix )
    {
        // adds each TruthTable from other to this if needed
        if( AND_matrix.count( key ) == 0 )
        {
            AND_matrix[ key ] = TruthTable( newsize );
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

    // multiplys OR_matrix with other.OR_matrix
    for( std::vector< bool > const& statement : OR_matrix )
    {
        for( std::vector< bool > const& other_statement : other.OR_matrix )
        {
            other_index = 0;

            for( bool const& value : statement )
            {
                for( bool const& other_value : other_statement )
                {
                    temp_OR_vector[ index ][ other_index++ ] = value & other_value;
                }
            }

            ++index;
        }
    }

    OR_matrix = temp_OR_vector;

    trim();

    return *this;
}

LogicalMatrix LogicalMatrix::AND( const LogicalMatrix &other )
{
    *this &= other;
    return *this;
}

LogicalMatrix LogicalMatrix::AND( const LogicalMatrix &other, const size_t &statement_index )
{
    if( other.empty() | statement_index >= statement_count() )
    {
        return *this;
    }

    LogicalMatrix temp_matrix = isolate_statement( statement_index ) & other;

    OR_matrix.erase( OR_matrix.begin() + statement_index );

    return ADD( temp_matrix, statement_index );
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

    extend_matrix( other );

    size_t index = 0, newsize = OR_matrix[ 0 ].size() + other.OR_matrix[ 0 ].size();
    std::vector< std::vector< bool > > temp_OR_vector = std::vector< std::vector< bool > >( other.statement_count() * statement_count() );

    for( std::vector< bool > const& statement : OR_matrix )
    {
        for( std::vector< bool > const& other_statement : other.OR_matrix )
        {
            temp_OR_vector[ index ] = statement;
            extend_vector( temp_OR_vector[ index ], other_statement, newsize );
            ++index;
        }
    }

    OR_matrix = temp_OR_vector;

    trim();

    return *this;
}

LogicalMatrix LogicalMatrix::OR( const LogicalMatrix &other )
{
    *this |= other;
    return *this;
}

LogicalMatrix LogicalMatrix::OR( const LogicalMatrix &other, const size_t &statement_index )
{
    if( other.empty() | statement_index >= statement_count() )
    {
        return *this;
    }

    LogicalMatrix temp_matrix = isolate_statement( statement_index ) | other;

    OR_matrix.erase( OR_matrix.begin() + statement_index );

    return ADD( temp_matrix, statement_index );
}

LogicalMatrix LogicalMatrix::operator +( const LogicalMatrix &other ) const
{
    LogicalMatrix new_matrix( *this );
    return new_matrix.ADD( other );
}

LogicalMatrix LogicalMatrix::operator +=( const LogicalMatrix &other )
{
    return ADD( other );
}

LogicalMatrix LogicalMatrix::ADD( const LogicalMatrix &other, const size_t &statement_index )
{
    if( other.empty() )
    {
        return *this;
    }

    if( OR_matrix.empty() )
    {
        *this = other;
        return *this;
    }

    extend_matrix( other );

    size_t depth = statement_count(),
        old_size = OR_matrix[ 0 ].size(),
        other_size = other.OR_matrix[ 0 ].size();
    size_t index, newsize = old_size + other_size;

    std::vector< bool > temp_vector( other_size, false );

    OR_matrix.reserve( statement_count() + other.statement_count() );

    for( std::vector< bool >& statement : OR_matrix )
    {
        extend_vector( statement, temp_vector, newsize );
    }

    index = ( statement_index < depth )? statement_index : depth;

    for( auto const& statement : other.OR_matrix )
    {
        OR_matrix.insert( OR_matrix.begin() + index, std::vector< bool >( old_size, false ) );
        extend_vector( OR_matrix[ index++ ], statement, newsize );
    }

    trim();

    return *this;
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

size_t LogicalMatrix::identifier_count() const
{
    return AND_matrix.size();
}

size_t LogicalMatrix::statement_count() const
{
    return OR_matrix.size();
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

std::vector< bool > LogicalMatrix::evaluate( std::map< std::string, bool > identifiers ) const
{
    if( empty() )
    {
        return {};
    }

    size_t index, inner_index, size = OR_matrix[ 0 ].size(), depth = statement_count();
    std::vector< bool > truth_table( size, true );
    std::vector< bool > result( depth, false );

    for( auto const& [ key, data ] : AND_matrix )
    {
        if( identifiers.count( key ) == 0 )
        { // keys found in this and not identifiers with are evaluated as FALSE regardless of data
            for( index = 0; index < size; ++index )
            {
                truth_table[ index ] = truth_table[ index ] & !( data.True[ index ] || data.False[ index ] );
            }
        }
        else
        { // keys found in both this and identifiers are evaluated based on their value in identifiers
            if( identifiers[ key ] )
            {
                for( index = 0; index < size; ++index )
                {
                    truth_table[ index ] = truth_table[ index ] & !data.False[ index ];
                }
            }
            else
            {
                for( index = 0; index < size; ++index )
                {
                    truth_table[ index ] = truth_table[ index ] & !data.True[ index ];
                }
            }
        }
    }

    for( index = 0; index < depth; ++index )
    {
        for( inner_index = 0; inner_index < size; ++inner_index )
        {
            if( truth_table[ inner_index ] && OR_matrix[ index ][ inner_index ] )
            {
                result[ index ] = true;
                break;
            }
        }
    }

    return result;
}

bool LogicalMatrix::remove_statement( const size_t &remove_index )
{
    if( remove_index < statement_count() )
    {
        OR_matrix.erase( OR_matrix.begin() + remove_index );
        trim();

        return true;
    }

    return false;
}

LogicalMatrix LogicalMatrix::isolate_statement( const size_t &statement_index ) const
{
    LogicalMatrix result;

    if( statement_index < statement_count() )
    {
        result.AND_matrix = AND_matrix;
        result.OR_matrix.push_back( OR_matrix[ statement_index ] );
        result.trim();
    }

    return result;
}

std::vector< LogicalMatrix > LogicalMatrix::split_statements() const
{
    size_t index, depth = statement_count();
    std::vector< LogicalMatrix > result = std::vector< LogicalMatrix >( depth );

    for( index = 0; index < depth; ++index )
    {
        result[ index ] = isolate_statement( index );
    }

    return result;
}

void LogicalMatrix::combine_statements()
{
    if( statement_count() > 1 )
    {
        std::vector< LogicalMatrix > split_vector = split_statements();

        clear();

        for( auto const& individual_LM : split_vector )
        {
            *this &= individual_LM;
        }
    }
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

std::string LogicalMatrix::to_string() const
{
    std::ostringstream stringstream;
    stringstream << *this;
    return stringstream.str();
}

std::ostream &operator<<( std::ostream &output, const LogicalMatrix &object_arg )
{
    if( object_arg.empty() )
    {
        return output;
    }

    size_t size = object_arg.OR_matrix[ 0 ].size();
    std::ostringstream AND_streams[ size ];
    std::vector< bool > AND_empty( size, true );
    bool OR_empty, output_empty = true;

    for( auto const& [ key, data ] : object_arg.AND_matrix )
    {
        for( size_t index = 0; index < size; ++index )
        {
            if( data.True[ index ] | data.False[ index ] )
            {
                if( AND_empty[ index ] )
                {
                    AND_empty[ index ] = false;
                }
                else
                {
                    AND_streams[ index ] << " & ";
                }

                if( data.True[ index ] & data.False[ index ] )
                {
                    AND_streams[ index ] << key << " & ";
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
        print_vector( statement, std::to_string( index++ ) );
    }

    std::cout << output.str();
}
