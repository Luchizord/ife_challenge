#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

/*
*
*
*
**/
class Configuration
{
    private:
        std::string prefix;
        std::string file_type;
        unsigned long int time_ms;
        std::string dir_search;
        std::string dir_copy;

    public:
        Configuration(const std::string pathDir);

        std::string getPrefix(void)
        {
            return prefix;
        };

        std::string getFileType(void)
        {
            return file_type;
        };

        unsigned long int getTimeMs(void)
        {
            return time_ms;
        };

        std::string getDirSearch(void)
        {
            return dir_search;
        };

        std::string getDirCopy(void)
        {
            return dir_copy;
        };
};

#endif