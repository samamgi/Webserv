#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include "Utils.hpp"

#define LINELENGTH 100
#define PADDING 1

#define WEBSERV_BANNER                                                                             \
		"\n" \
        "           .:+##*.                 .@@@@@%#=.\n"                   \
        "        .-%@@#@@-                   .=@@*:=#@@+..\n"               \
        "      .:@@* :@@.                       .%@*. .%@+.\n"              \
        "     .+@#. .:#@@+.                    .@@%.   .-@%.\n"             \
        "    .#@=   .:#@%=.                     .-%@+.   -@%.\n"            \
        "    =@*    =@#:                          .+@+.  .*@+.\n"           \
        "    #@.   :@#.                            .*@:.  .@#.\n"           \
        "   .@%   .#@:                              :@+   .%@.\n"           \
        "   .@%   .#@:                              :@=   .%@.\n"           \
        "    #@.   :@#                             .%@.   .@#.\n"           \
        "    +@+    +@*.                          :%@-   .*@+\n"            \
        "    .%@-   .+@%---=*#@@@@@@@@@@@%*+---::#@%.   .+@#.\n"            \
        "    ..@@:    .#@@*-...............:=#@@@+..   :%@-.\n"             \
        "      .+@%-.                               .:#@*..\n"              \
        "        .=%@*                             .@@#..\n"                \
        "          :@+                              %@.\n"                  \
        "          -@-                              *@.\n"                  \
        "          =@-                              +@:\n"                  \
        "          =@-                       .      =@:\n"                  \
        "          =@-  .-#@@#=.          .#@@@#-.  =@:\n"                  \
        "          -@= .#@@@@@@@-.       -@@@@@@@#. +@:\n"                  \
        "          :@+ :@@@@@@@@%.       +@@@@@@@@+.%@.\n"                  \
        "          .@@..@@@@@@@@*.       =@@@@@@@@::@#.\n"                  \
        "           =@*..#@@@@@+.        .-%@@@@*..#@-\n"                   \
        "           .*@*. .....             ... ..*@=.\n"                   \
        "            .-@@-.                    .+@@:.\n"                    \
        "             ..=@@#+:...     .....:+*@@#-.\n"                      \
        "                ..-#%@@@@@@@@@@@@%##=..\n"                         
                                                                        
                                                                        
                                                                        

#define PROJ_TITLE "Webserv - 42"

#define TEAM_LIST "tsarrazi ssadi-ou"

#define TOP_LEFT GREY "\n╔" RESET
#define TOP_RIGHT GREY "╗" RESET
#define MID_LEFT GREY "╠" RESET
#define MID_RIGHT GREY "╣" RESET
#define BOT_LEFT GREY "╚" RESET
#define BOT_RIGHT GREY "╝\n" RESET
#define VERTICAL GREY "║" RESET
#define HORIZONTAL GREY "═" RESET
#define SYMBOL "   > "
#define ERROR_TAG RED "[ ERROR ] " RESET

void graBottomLine();
void graTopLine();
void graSeparator();
void graEmptyLine();
void graTextLine(std::string const &str);
void graTextHeader(std::string const &str);
void graTextElement(std::string const &str);
void graTextElement(int const &num);
void graTime(std::string const &str);
void graTime();
std::string graPad(std::string const &str);
void graError(std::string const &error);

#endif
