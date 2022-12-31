#ifndef LOGGER_HPP
#define LOGGER_HPP

# include "Webserv.hpp"

#define RESET          "\x1B[0m"
#define RED            "\x1B[31m"
#define LIGHT_RED      "\x1B[91m"
#define WHITE          "\x1B[37m"
#define BLINK           "\x1b[5m"
#define YELLOW         "\x1B[33m"
#define LIGHT_BLUE     "\x1B[94m"
#define CYAN           "\x1B[36m"
#define DARK_GREY      "\x1B[90m"
#define LIGHTMAGENTA   "\x1B[95m"
#define GST             4;


/* Not used currently */
enum LogPrio{
    DEBUG,
    INFO,
    ERROR
};

enum L_State{
    ON,
    OFF
};


enum Mode{
    CONSOLE_OUTPUT,
    FILE_OUTPUT
};

class Logger{

    public:
        static std::string file_name;
        static LogPrio prio;
        static std::map<LogPrio, std::string> prio_str;
        static L_State state;

        static void         setFilenName(std::string);
        static void         setState(L_State);

        static void         logMsg(const char *, Mode, const char*, ...);
        static void         setPrio(LogPrio);
        static void         enableFileLog();
        static std::string  getCurrTime();
    private:
        static std::map<LogPrio, std::string> initMap();

};


#endif