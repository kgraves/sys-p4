#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>

const std::string PATH = "table.log";

class Logger {
    public:
        static Logger *instance();
        void log(const std::string &str);
        void log(const char *str);
        void log(const int &i);
        void flush();

    private:
        static Logger *m_instance;

        std::fstream m_file;

        Logger();

        // disable these for singleton pattern
        Logger(const Logger &log);
        const Logger &operator=(const Logger &l);

        ~Logger();
};

#endif

