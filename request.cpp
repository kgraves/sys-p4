#include "request.h"

Request::Request() {}

Request::Request(int request, std::string type, std::string key)
    : m_request(request), m_type(type), m_key(key), m_number(0), m_values("")
{
}

Request::Request(int request, std::string type, std::string key, int number_values, std::string values)
    : m_request(request), m_type(type), m_key(key), m_number(number_values), m_values(values)
{
}

std::string Request::to_string() {
    std::ostringstream oss;

    oss << m_request << SEPARATOR;
    oss << m_type << SEPARATOR;
    oss << m_key << SEPARATOR;

    if (m_number > 0) {
        oss << m_number << SEPARATOR;
        oss << m_values << '\n';
    }
    else {
        oss << '\n';
    }

    return oss.str();
}

