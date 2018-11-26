/*
*
*
**/
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "configuration.hpp"


#define PREFIX                  0
#define EXTENSION               1
#define TIME_MS                 2
#define DIR_SEARCH              3
#define DIR_COPY                4


Configuration::Configuration(const std::string pathDir)
{
    std::ifstream configFile;
    std::string singleLine;
    std::string auxiliarLine[10];
    int lineIndex = 0;

    configFile.open(pathDir + "/parameters.conf");

    while (std::getline(configFile, singleLine))
    {
        if (singleLine[7] == '=')
        {
            auxiliarLine[lineIndex] = &singleLine[8];
        }
        else
        {
            auxiliarLine[lineIndex] = "";
        }
        
        switch(lineIndex)
        {
            case PREFIX:
            {
                prefix = auxiliarLine[PREFIX];
            }
            break;

            case EXTENSION:
            {
                file_type = auxiliarLine[EXTENSION];
            }
            break;

            case TIME_MS:
            {
                std::stringstream int_time_ms(auxiliarLine[TIME_MS]);
                int_time_ms >> time_ms;
            }
            break;

            case DIR_SEARCH:
            {
                dir_search = auxiliarLine[DIR_SEARCH];
            }
            break;

            case DIR_COPY:
            {
                dir_copy = auxiliarLine[DIR_COPY];
            }
            break;
        }

        ++lineIndex;
    }
}