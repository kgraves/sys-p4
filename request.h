#ifndef REQUEST_H 
#define REQUEST_H

#include <string>
#include <sstream>

const std::string STRING = "string";
const std::string INTEGER = "integer";
const char SEPARATOR = ' ';

class Request
{
    public:
        Request();
        Request(int request, std::string type, std::string key);
        Request(int request, std::string type, std::string key, int number_values, std::string values);

        int request() {return m_request;};
        std::string type() {return m_type;};
        std::string key() {return m_key;};
        int number() {return m_number;};
        std::string values() {return m_values;};

        std::string to_string();

        int m_request;
        std::string m_type;
        std::string m_key;
        int m_number;
        std::string m_values;

    private:
};

#endif

