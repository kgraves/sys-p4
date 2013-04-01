#include "parser.h"

Parser::Parser(const std::string &in_file)
{
    // try to open files
    m_in_path = in_file;
    //m_out_path = out_file;

    if (open_file(m_in_fd, m_in_path, O_RDONLY) == -1)
        perror("open in_file error");
}

// main parse function for reading a file by chunks
bool Parser::read_parse(const int chunk_size)
{
    read_file(m_in_fd, chunk_size);
    close_file(m_in_fd);
    return (m_requests.size());
    //return (write(m_out_fd, output.c_str(), output.size()) != -1);
}

// main parse function for mmap-ing a file
bool Parser::mmap_parse()
{
    size_t size = file_size(m_in_fd);
    char* buffer = (char*)mmap(0, size, PROT_READ, MAP_SHARED, m_in_fd, 0);

    mmap_file(buffer);
    close_file(m_in_fd);

    //write(m_out_fd, output.c_str(), output.size());
    return (munmap(buffer, size) != -1 && m_requests.size());
}

bool Parser::open_file(int &fd, const std::string &file_name, const int flags, const int modes)
{
    if (modes)
        fd = open(file_name.c_str(), flags, modes);
    else
        fd = open(file_name.c_str(), flags);

    return fd != -1;
}

bool Parser::close_file(const int fd)
{
    return (close(fd) != -1);
}

size_t Parser::file_size(const int fd)
{
    struct stat fst;
    if (fstat(fd, &fst) == -1)
    {
        perror("fstat error");
        return -1;
    }

    return fst.st_size;
}

bool Parser::parse(char** buffer, std::string &result, char delim, int &bytes_read)
{
    result.clear();
    bytes_read = 0;

    while (**buffer && **buffer != delim)
    {
        result += **buffer;
        ++(*buffer);
        ++bytes_read;
    }

    if (**buffer != delim)
    {
        return false;
    }
    else
    {
        assert(**buffer == delim);
        ++(*buffer); //advance past the current delim
        ++bytes_read;
        return true;
    }
}

void Parser::read_file(int in_file, const int buffer_size)
{
    int bytes_read = 0;
    char *buffer = new char[buffer_size+1];
    char *buffer_head = buffer;
    char *line_buff, *line_head;
    std::string request_code, type, key, num_data, data;
    //std::string output;

    //read chunk from file
    while ((bytes_read = read(in_file, buffer, buffer_size)) != 0)
    {
        //append null terminator after read()
        buffer[bytes_read] = '\0';
        std::string result;

        //pick off request that ends in a newline
        while (parse(&buffer, result, LINE_DELIM, bytes_read))
        {
            std::string request_code, type, key, num_data, data;

            //copy the string obj into a char*
            line_buff = new char[result.size()+1];
            strcpy(line_buff, result.c_str());
            line_head = line_buff;

            parse(&line_buff, request_code, PIECE_DELIM, bytes_read);
            parse(&line_buff, type, PIECE_DELIM, bytes_read);
            parse(&line_buff, key, PIECE_DELIM, bytes_read);

            int req = atoi(request_code.c_str());
            if (req == PUT)
            {
                parse(&line_buff, num_data, PIECE_DELIM, bytes_read);

                int n = atoi(num_data.c_str());
                std::string datum;
                while(n)
                {
                    parse(&line_buff, datum, WORD_DELIM, bytes_read);
                    data += datum + " ";
                    --n;
                }

                Request r(PUT, type, key, n, data);
                m_requests.push_back(r);
            }
            else if (req == GET)
            {
                Request r(GET, type, key);
                m_requests.push_back(r);
            }
            else //req == DELETE
            {
                assert(req == DELETE);
                Request r(DELETE, type, key);
                m_requests.push_back(r);
            }

            //delete mem where line_buff used to point
            delete line_head;
        }
        
        //lseek back parse_length bytes
        if (lseek(in_file, (-1 * bytes_read), SEEK_CUR) == -1)
        {
            perror("seek error");
            return;
        }

        buffer = buffer_head;
    }

    delete buffer_head;
    buffer = NULL;

    line_buff = NULL;
    line_head = NULL;
    
    //return output;
}

void Parser::mmap_file(char *buffer)
{
    //char *buffer = new char[buffer_size+1];
    //char *buffer_head = buffer;
    //std::string output;
    int bytes_read = 0;
    char *line_buff, *line_head;
    std::string request_code, type, key, num_data, data;

    std::string result;

    //pick off request that ends in a newline
    while (parse(&buffer, result, LINE_DELIM, bytes_read))
    {
        std::string request_code, type, key, num_data, data;

        //copy the string obj into a char*
        line_buff = new char[result.size()+1];
        strcpy(line_buff, result.c_str());
        line_head = line_buff;

        parse(&line_buff, request_code, PIECE_DELIM, bytes_read);
        parse(&line_buff, type, PIECE_DELIM, bytes_read);
        parse(&line_buff, key, PIECE_DELIM, bytes_read);

        int req = atoi(request_code.c_str());
        if (req == PUT)
        {
            parse(&line_buff, num_data, PIECE_DELIM, bytes_read);

            int n = atoi(num_data.c_str()), num = n;
            std::string datum;
            while(n)
            {
                parse(&line_buff, datum, WORD_DELIM, bytes_read);
                data += datum + " ";
                --n;
            }

            Request r(PUT, type, key, num, data);
            m_requests.push_back(r);
        }
        else if (req == GET)
        {
            Request r(GET, type, key);
            m_requests.push_back(r);
        }
        else //req == DELETE
        {
            assert(req == DELETE);
            Request r(DELETE, type, key);
            m_requests.push_back(r);
        }

        //delete mem where line_buff used to point
        delete [] line_head;
        line_buff = NULL;
    }

    buffer -= bytes_read;

    //return output;
}
