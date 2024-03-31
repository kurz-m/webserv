NAME := webserv
.DEFAULT_GOAL := all
CXX := c++

################################################################################
###############                 PRINT OPTIONS                     ##############
################################################################################

G := \033[32m
M := \033[31m
X := \033[0m
BO := $(shell tput bold)
LOG := printf "[$(BO)$(G)ⓘ INFO$(X)] %s\n"
TEST := printf "[$(BO)$(M)ⓘ TEST$(X)] %s\n"

################################################################################
###############                  DIRECTORIES                      ##############
################################################################################

OBJ_DIR := obj-cache
INC_DIRS := .
SRC_DIRS := .
LOG_LEVEL := $(LOG_LEVEL)

# Tell the Makefile where headers and source files are
vpath %.hpp $(INC_DIRS)
vpath %.cpp $(SRC_DIRS)

################################################################################
###############                  SOURCE FILES                     ##############
################################################################################

SRCS := $(wildcard *.cpp)
OBJS := $(addprefix $(OBJ_DIR)/, $(SRCS:%.cpp=%.o))

################################################################################
########                           FLAGS                        ################
################################################################################

CXXFLAGS ?= -Wextra -Wall -Werror -std=c++98 -MMD -MP $(addprefix -I, $(INC_DIRS))
LDFLAGS :=

ifeq ($(DEBUG), 1)
	CXXFLAGS += -g
else
	CXXFLAGS += -O3
endif

ifeq ($(LOG_LEVEL),)
	CXXFLAGS += -D__LOG_LEVEL__=4
else ifeq ($(LOG_LEVEL), 0)
	CXXFLAGS += -D__LOG_LEVEL__=0
else ifeq ($(LOG_LEVEL), 1)
	CXXFLAGS += -D__LOG_LEVEL__=1
else ifeq ($(LOG_LEVEL), 2)
	CXXFLAGS += -D__LOG_LEVEL__=2
else ifeq ($(LOG_LEVEL), 3)
	CXXFLAGS += -D__LOG_LEVEL__=3
endif

# ifdef $(LOG_LEVEL)
# 	CXXFLAGS += -D__LOG_LEVEL__=$(LOG_LEVEL)
# endif

################################################################################
########                         COMPILING                      ################
################################################################################

all: $(NAME)

$(NAME): $(OBJS)
	@$(LOG) "Linking object files to $@"
	@$(CXX) $^ $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	@$(LOG) "Compiling $(notdir $@)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	@$(LOG) "Creating object directory."
	@mkdir -p $@

debug: CXXFLAGS += -g
debug: fclean all

clean:
	@if [ -d "$(OBJ_DIR)" ]; then \
		$(LOG) "Cleaning $(notdir $(OBJ_DIR))"; \
		rm -rf $(OBJ_DIR); \
	else \
		$(LOG) "No objects to clean."; \
	fi

fclean: clean
	@if [ -f "$(NAME)" ]; then \
		$(LOG) "Cleaning $(notdir $(NAME))"; \
		rm -f $(NAME); \
	else \
		$(LOG) "No library to clean."; \
	fi

re: fclean all

-include $(OBJS:$(OBJ_DIR)/%.o=$(OBJ_DIR)/%.d)

.PHONY: all fclean clean re debug

test:
	$(CXX) $(CXXFLAGS) -g Token.cpp Lexer.cpp test_lexer.cpp -o lexer.test

test-parser:
	$(CXX) $(CXXFLAGS) -g Token.cpp Settings.cpp EventLogger.cpp Lexer.cpp Parser.cpp HTTP.cpp test_lexer.cpp -o parser.test

help: ## Display this help menu
	@echo "Webserv Project Manual\n"
	@echo "Compilation Options:"
	@echo "make [DEBUG=X] [LOG_LEVEL=x]"
	@echo "  DEBUG=0:  Compile without debug symbols (default)"
	@echo "  DEBUG=1:  Compile with debug symbols"
	@echo "  LOG_LEVEL=0 (DEBUG):   Log all messages"
	@echo "  LOG_LEVEL=1 (INFO):    Log errors, warnings, and info"
	@echo "  LOG_LEVEL=2 (WARNING): Log errors and warnings"
	@echo "  LOG_LEVEL=3 (ERROR):   Log only errors"
	@echo "  LOG_LEVEL=4 (NONE):    Log no message (default)"
	@echo "\nExecution Options:"
	@echo "Usage: ./webserv [config_file]"
	@echo "  config_file: Optional path to configuration file. Defaults to 'config/default.conf'\n"
