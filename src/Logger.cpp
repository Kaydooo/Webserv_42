#include "../inc/Logger.hpp"


std::string Logger::file_name = "logfile.txt";
LogPrio Logger::prio = ERROR;
std::map<LogPrio, std::string> Logger::prio_str = initMap();



std::map<LogPrio, std::string> Logger::initMap()
{
    std::map<LogPrio, std::string> p_map;

    p_map[DEBUG] = "[DEBUG]   ";
    p_map[INFO] = "[INFO]    ";
    p_map[ERROR] = "[ERROR]   ";
    return p_map;
}
void    Logger::logMsg(LogPrio p, Mode m, const char* msg, ...)
{
    char        output[8192];
    va_list     args;
    int         n;

    va_start(args, msg);
    n = vsnprintf(output, 8192, msg, args);
    std::string date = getCurrTime();
    if (Logger::prio >= p)
    {
        if (m == FILE_OUTPUT)
        {
            if (mkdir("logs", 0777) < 0 && errno != EEXIST)
            {
                std::cerr << "mkdir() Error: " << strerror(errno) << std::endl;
                return ;
            }
            int fd = open(("logs/" + file_name).c_str(), O_CREAT | O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR);
            std::cout << "fd is " << fd << "And errno is :" << strerror(errno) << std::endl;
            write(fd, date.c_str(), date.length());
            write(fd, prio_str[p].c_str(), prio_str[p].length());
            write(fd, "   ", 3);
            write(fd, output, n);
            write(fd, "\n", 1);
            close(fd);
        }
        else if (m == CONSOLE_OUTPUT)
        {
            if (p == DEBUG)
                std::cout << LIGHTMAGENTA;
            else if (p == INFO)
                std::cout << CYAN;
            else if (p == ERROR)
                std::cout << RED;
            std::cout << getCurrTime() << prio_str[p] << output << RESET << std::endl;
        }      
    }
    va_end(args);
}

std::string Logger::getCurrTime()
{
    tzset();
    char date[1000];
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    tm.tm_hour = tm.tm_hour + GST;
    strftime(date, sizeof(date), "[%Y-%m-%d  %H:%M:%S]   ", &tm);
    return (std::string(date));
}

void Logger::setPrio(LogPrio p)
{
    Logger::prio = p;
}

void Logger::setFilenName(std::string name)
{
    Logger::file_name = name;
}
