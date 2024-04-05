NAME := webserv
.DEFAULT_GOAL := multi
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
INC_DIRS := src
SRC_DIRS := src
LOG_DIR := logs

# Tell the Makefile where headers and source files are
vpath %.hpp $(INC_DIRS)
vpath %.cpp $(SRC_DIRS)

################################################################################
###############                  SOURCE FILES                     ##############
################################################################################

SRCS := $(wildcard $(SRC_DIRS)/*.cpp)
OBJ_SRCS := $(notdir $(SRCS))
OBJS := $(addprefix $(OBJ_DIR)/, $(OBJ_SRCS:%.cpp=%.o))

################################################################################
########                           FLAGS                        ################
################################################################################

CXXFLAGS ?= -Wextra -Wall -Werror -std=c++98 -MMD -MP $(addprefix -I, $(INC_DIRS))
LDFLAGS :=
LOG_LEVEL := $(LOG_LEVEL)

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

multi:
	@$(MAKE) -j12 all

all: $(NAME) | $(LOG_DIR) ## Default target for compiling webserv

$(NAME): $(OBJS) ## Rule for linking the object files to the webserv binary
	@$(LOG) "Linking object files to $@"
	@$(CXX) $^ $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR) ## Rule for compiling the object files
	@$(LOG) "Compiling $(notdir $@)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR): ## Create the object directory
	@$(LOG) "Creating object directory."
	@mkdir -p $@

$(LOG_DIR): ## Create the log directory (logs)
	@$(LOG) "Creating log directory."
	@mkdir -p $@

debug: CXXFLAGS += -g
debug: fclean all

clean: ## Cleans the object files from obj-cache
	@if [ -d "$(OBJ_DIR)" ]; then \
		$(LOG) "Cleaning $(notdir $(OBJ_DIR))"; \
		rm -rf $(OBJ_DIR); \
	else \
		$(LOG) "No objects to clean."; \
	fi

fclean: clean ## Cleans the object files and binaries
	@if [ -f "$(NAME)" ]; then \
		$(LOG) "Cleaning $(notdir $(NAME))"; \
		rm -f $(NAME); \
	else \
		$(LOG) "No library to clean."; \
	fi

re: fclean multi

-include $(OBJS:$(OBJ_DIR)/%.o=$(OBJ_DIR)/%.d)

.PHONY: all multi fclean clean re debug

################################################################################
########                        HELP TARGET                     ################
################################################################################

MAKEFILE_LIST := Makefile

display: ## Prints help for targets with comments
	@cat $(MAKEFILE_LIST) | grep -E '^[a-zA-Z_-]+:.*?## .*$$' | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

help: ## Display the help menu
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
