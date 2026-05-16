NAME		= webserv

CXX			= c++ 
CXXFLAGS		= -Wall -Wextra -Werror -std=c++98 -g
DEP_FLAG	= -MMD -MP
INCLUDE		= -I$(INC_DIR)

RMF			= rm -f
RMD			= rm -rf
MKDIR		= mkdir -p

INC_DIR		= inc/
SRC_DIR		= src/
OBJ_DIR		= obj/
REQ_DIR		= request/
RSP_DIR		= response/
PRT_DIR		= printlog/
CONF_DIR	= Parsing_configuration/

SRC_FILES	= HttpParserTester main  ServerManager Utils \

REQ_FILES	= HttpRequest HttpParser RequestLine Uri Headers
RSP_FILES	= Response HeadRsp Script
PRT_FILES	= PrintLog Graphics
CONF_FILES	= ABlockBase LocationConf ParsingConf ServerData

ALL_FILES	+= $(SRC_FILES)
ALL_FILES	+= $(addprefix $(REQ_DIR), $(REQ_FILES))
ALL_FILES	+= $(addprefix $(RSP_DIR), $(RSP_FILES))
ALL_FILES	+= $(addprefix $(PRT_DIR), $(PRT_FILES))
ALL_FILES	+= $(addprefix $(CONF_DIR), $(CONF_FILES))

SRCS		= $(addprefix $(SRC_DIR), $(addsuffix .cpp, $(ALL_FILES)))
OBJS		= $(addprefix $(OBJ_DIR), $(addsuffix .o, $(ALL_FILES)))
DEPS		= $(addprefix $(OBJ_DIR), $(addsuffix .d, $(ALL_FILES)))

GRE         = \033[32m
RED         = \033[31m
RES         = \033[0m

all:	$(NAME)

$(NAME): $(OBJS)
		@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
		@echo -e "$(GRE)Compilation $(NAME) [OK]$(RES)"

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp Makefile
		@$(MKDIR) $(dir $@)
		@$(CXX) $(CXXFLAGS) $(INCLUDE) $(DEP_FLAG) -c $< -o $@
		@echo -e "$(BLA)loading : $(RES)$< $(GRE)[OK]$(RES)"

clean:
	@$(RMD) $(OBJ_DIR)
	@echo -e "$(RED)FILE '*.o' for $(NAME) deleted$(RES)"

fclean: clean
	@$(RMF) $(NAME)
	@echo -e "$(RED)Project $(NAME) deleted$(RES)"

re: fclean all

-include $(DEPS)
.PHONY:	all clean fclean re
