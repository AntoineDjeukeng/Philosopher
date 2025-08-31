# **************************************************************************** #
#                                   CONFIG                                      #
# **************************************************************************** #

NAME        := philo
BONUS       := philo_bonus

CC          := cc
THREADS     := -pthread
CFLAGS      := -Wall -Wextra -Werror -O2 -MMD -MP $(THREADS)
LDFLAGS     := $(THREADS)
INCLUDE     := -Iinclude

# ------------------------------------------------------------------------------
# Mandatory sources
# ------------------------------------------------------------------------------
SRC_DIR     := src
SRC_FILES   := ft_calloc.c app.c  destroy.c  init.c  logger.c  main.c  monitor.c  parse.c  philo.c\
				 run.c  time.c  logger_utils.c  philo_utils.c ft_strlcpy.c  ft_strcmp.c\
				 
SRCS        := $(addprefix $(SRC_DIR)/,$(SRC_FILES))

OBJ_DIR     := obj
OBJS        := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS        := $(OBJS:.o=.d)

# ------------------------------------------------------------------------------
# Bonus sources
# ------------------------------------------------------------------------------
SRCB_DIR    := src_bonus
SRCB_FILES  := 
SRCB        := $(addprefix $(SRCB_DIR)/,$(SRCB_FILES))

OBJB_DIR    := obj_bonus
OBJSB       := $(SRCB:$(SRCB_DIR)/%.c=$(OBJB_DIR)/%.o)
DEPSB       := $(OBJSB:.o=.d)

# **************************************************************************** #
#                                   RULES                                       #
# **************************************************************************** #

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) $(OBJS) -o $@ $(LDFLAGS)

bonus: $(BONUS)

$(BONUS): $(OBJSB)
	$(CC) $(CFLAGS) $(INCLUDE) $(OBJSB) -o $@ $(LDFLAGS)

# ------------------------------------------------------------------------------
# Object rules with auto dir creation + dependency generation
# ------------------------------------------------------------------------------
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(OBJB_DIR)/%.o: $(SRCB_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

# ------------------------------------------------------------------------------
# Housekeeping
# ------------------------------------------------------------------------------
clean:
	rm -f $(OBJS) $(OBJSB) $(DEPS) $(DEPSB)
	@rmdir --ignore-fail-on-non-empty $(OBJ_DIR) $(OBJB_DIR) 2>/dev/null || true

fclean: clean
	rm -f $(NAME) $(BONUS)

re: fclean all

.PHONY: all bonus clean fclean re

# Include auto-generated dependency files (safe if missing)
-include $(DEPS)
-include $(DEPSB)
