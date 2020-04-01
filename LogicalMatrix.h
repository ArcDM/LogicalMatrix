// LogicalMatrix.h

/** Header file for the LogicalMatrix class.
 */

#ifndef __LogicalMatrix_h_included__
#define __LogicalMatrix_h_included__

#include <exception>
#include <iostream>
#include <set>
#include <string>
#include <vector>

class LogicalMatrix
{
    private:
        class Operator
        {
            public:
                std::string label;
                bool negation;

                Operator( const std::string input_string = "", const bool input_negation = false );
                bool operator ==( const Operator &other ) const;
                bool operator <( const Operator &other ) const;
                Operator operator !() const;
        };

        typedef std::set< Operator > operator_collection; // AND separated
        typedef std::set< operator_collection > statement_collection; // OR separated AND separated

        statement_collection operators;
        std::set< std::string > unique_identifiers;

        statement_collection weave_operators( const statement_collection &other_operators ) const;

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
};

#endif
