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
endif

ifeq ($(VERBOSE), 1)
	CXXFLAGS += -D__verbose__
endif

################################################################################
########                         COMPILING                      ################
################################################################################

all: $(NAME)

$(NAME): $(OBJS)
	@$(LOG) "Linking object files to $@"
	@$(CXX) $^ $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	@$(LOG) "Compiling $(notdir $@)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

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