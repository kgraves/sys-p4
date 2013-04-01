#include "logger.h"

/* static */ Logger *Logger::m_instance = NULL;

Logger::Logger() : m_file(PATH.c_str(), std::ios_base::out|std::ios_base::app) {
    if (!m_file.is_open())
        m_file.open(PATH.c_str());
}

/*static*/ Logger* Logger::instance() {
    if (m_instance == NULL)
        m_instance = new Logger();

    return m_instance;
}

void Logger::log(const std::string &str) {
    m_file << str << std::endl;
}

void Logger::log(const char *str) {
    m_file << str << std::endl;
}

void Logger::log(const int &i) {
    m_file << i << std::endl;
}

void Logger::flush() {
    m_file.close();
}

Logger::~Logger() {
    m_file.close();
    
    if (m_instance)
        delete m_instance;
}

