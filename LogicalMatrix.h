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
        class Operator
        {
            public:
                std::vector< bool > True, False;

                Operator( const size_t depth = 1 );
                Operator( const size_t depth, const bool condition );
                bool operator ==( const Operator &other ) const;
                bool operator <( const Operator &other ) const;
                Operator operator !() const;
        };

        std::map< std::string, Operator > AND_matrix;
        std::vector< std::vector< bool > > OR_matrix;

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
        std::string to_string() const;
        friend std::ostream &operator<<( std::ostream &output, const LogicalMatrix &object_arg );
        LogicalMatrix operator !() const;
        LogicalMatrix operator &( const LogicalMatrix &other ) const;
        LogicalMatrix operator &=( const LogicalMatrix &other );
        LogicalMatrix operator |( const LogicalMatrix &other ) const;
        LogicalMatrix operator |=( const LogicalMatrix &other );
        bool operator ==( const LogicalMatrix &other ) const;
        bool operator <( const LogicalMatrix &other ) const;
        bool empty() const;
        void clear();
        void trim();
        void debug_print() const;
};

#endif