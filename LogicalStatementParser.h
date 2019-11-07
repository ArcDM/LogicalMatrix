// LogicalStatementParser.h

/** Header file for the LogicalStatementParser class.
 */

#ifndef __LogicalStatementParser_h_included__
#define __LogicalStatementParser_h_included__

#include <vector>
#include <string>
#include <iostream>
#include <set>

class LogicalStatementParser
{
    private:
        class Operator
        {
            public:
                std::string label;
                bool negation;

                Operator( const std::string input_string = "", const bool input_negation = false );
                bool operator ==( const Operator &other );
                Operator operator !();


        };

        // OR separated AND separated
        std::vector< std::vector< Operator > > operators;
        std::set< std::string > unique_identifiers;

        void separate_by_AND( const std::vector< std::string > OR_separated );


    public:
        LogicalStatementParser();
        LogicalStatementParser( const std::string &input_string );
        /*LogicalStatementParser( const LogicalStatementParser &other );
        LogicalStatementParser( LogicalStatementParser &&other ) noexcept;
        LogicalStatementParser& operator=( const LogicalStatementParser &other );
        LogicalStatementParser& operator=( LogicalStatementParser &&other ) noexcept;*/
        std::set< std::string > get_unique_identifiers();
        std::string to_string();
        friend std::ostream &operator<<( std::ostream &output, const LogicalStatementParser &object_arg );
        LogicalStatementParser operator !();
        LogicalStatementParser operator &( const LogicalStatementParser &other );
        LogicalStatementParser operator &=( const LogicalStatementParser &other );
        LogicalStatementParser operator |( const LogicalStatementParser &other );
        LogicalStatementParser operator |=( const LogicalStatementParser &other );
};

#endif
