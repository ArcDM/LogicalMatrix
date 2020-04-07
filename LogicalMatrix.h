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

        LogicalMatrix OR_helper( const LogicalMatrix &other, const bool &add );
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
        std::set< std::string > get_unique_identifiers() const;
        std::vector< bool > evaluate( std::map< std::string, bool > identifiers ) const;
        std::string to_string() const;
        friend std::ostream &operator<<( std::ostream &output, const LogicalMatrix &object_arg );
        LogicalMatrix operator !() const;
        LogicalMatrix operator &( const LogicalMatrix &other ) const;
        LogicalMatrix operator &=( const LogicalMatrix &other );
        LogicalMatrix operator |( const LogicalMatrix &other ) const;
        LogicalMatrix operator |=( const LogicalMatrix &other );
        LogicalMatrix operator +( const LogicalMatrix &other ) const;
        LogicalMatrix operator +=( const LogicalMatrix &other );
        bool operator ==( const LogicalMatrix &other ) const;
        bool operator <( const LogicalMatrix &other ) const;
        bool empty() const;
        void clear();
        void debug_print() const;
};

#endif
