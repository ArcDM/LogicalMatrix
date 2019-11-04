// LogicalStatementParser.h

/** Header file for the LogicalStatementParser class.
 */

#include <vector>
#include <string>

#ifndef __LogicalStatementParser_h_included__
#define __LogicalStatementParser_h_included__

class LogicalStatementParser
{
    private:

    public:
        LogicalStatementParser();
        LogicalStatementParser( const std::string &input_string );
        LogicalStatementParser( const LogicalStatementParser &object_arg );
        std::string to_string();
        void print();

};

#endif
