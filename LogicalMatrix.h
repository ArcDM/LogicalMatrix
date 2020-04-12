// LogicalMatrix.h

/** Header file for the LogicalMatrix class.
 */

#ifndef __LogicalMatrix_h_included__
#define __LogicalMatrix_h_included__

#include <exception>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

class LogicalMatrix
{
    private:
        class TruthTable
        {
            public:
                std::vector< bool > True, False;

                TruthTable( const size_t depth = 1 );
                TruthTable( const size_t depth, const bool condition );
                bool operator ==( const TruthTable &other ) const;
                bool operator <( const TruthTable &other ) const;
                TruthTable operator !() const;
        };

        std::map< std::string, TruthTable > AND_matrix;
        std::vector< std::vector< bool > > OR_matrix;

        LogicalMatrix build_inverse( const size_t &index ) const;
        void extend_matrix( const LogicalMatrix &other );
        void trim();

    public:
        class Logicalstatementexception: public std::exception
        {
            public:
                virtual const char* what() const throw()
                {
                    return "Malformed logical statement";
                }
        };

        LogicalMatrix() {}
        LogicalMatrix( const std::string &input_string );

        LogicalMatrix operator !() const;
        LogicalMatrix NOT();
        LogicalMatrix NOT( const size_t &statement_index );

        LogicalMatrix operator &( const LogicalMatrix &other ) const;
        LogicalMatrix operator &=( const LogicalMatrix &other );
        LogicalMatrix AND( const LogicalMatrix &other );
        LogicalMatrix AND( const LogicalMatrix &other, const size_t &statement_index );

        LogicalMatrix operator |( const LogicalMatrix &other ) const;
        LogicalMatrix operator |=( const LogicalMatrix &other );
        LogicalMatrix OR( const LogicalMatrix &other );
        LogicalMatrix OR( const LogicalMatrix &other, const size_t &statement_index );

        LogicalMatrix operator +( const LogicalMatrix &other ) const;
        LogicalMatrix operator +=( const LogicalMatrix &other );
        LogicalMatrix ADD( const LogicalMatrix &other, const size_t &statement_index = -1 );

        bool operator ==( const LogicalMatrix &other ) const;
        bool operator <( const LogicalMatrix &other ) const;
        bool empty() const;
        void clear();

        std::vector< bool > evaluate( std::map< std::string, bool > identifiers ) const;
        bool remove_statement( const size_t &remove_index );
        LogicalMatrix isolate_statement( const size_t &statement_index ) const;
        std::vector< LogicalMatrix > split_statements() const;
        void combine_statements();

        std::set< std::string > get_unique_identifiers() const;
        std::string to_string() const;
        friend std::ostream &operator<<( std::ostream &output, const LogicalMatrix &object_arg );
        void debug_print() const;
};

#endif
