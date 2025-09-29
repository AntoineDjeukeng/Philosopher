# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: adjeuken  <adjeuken@student.42.fr>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/09/13 14:43:15 by adjeuken          #+#    #+#              #
#    Updated: 2025/09/13 14:43:17 by adjeuken         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


NAME        = philo
BONUS_NAME  = philo_bonus

OBJ_DIR     = build

CC          = cc
THREAD_FLAGS= -pthread
CFLAGS      = -Wall -Wextra -Werror -MMD -MP $(THREAD_FLAGS) \
              -Iinclude
LDFLAGS     =
LDLIBS      =

RM          = rm -f

SRC_COMMON  = \
  src/time.c src/num.c src/io.c src/print.c \
  src/params_core.c src/forks.c src/params_init.c \
  src/philo_core.c src/philo_forks.c src/philo_lifecycle.c \
  src/logger.c src/logger_api.c src/mem.c src/start_threads.c

SRCS_MAIN   = src/main.c $(SRC_COMMON)
SRCS_BONUS  = src/main.c $(SRC_COMMON)

OBJS_MAIN   = $(addprefix $(OBJ_DIR)/,$(SRCS_MAIN:.c=.o))
OBJS_BONUS  = $(addprefix $(OBJ_DIR)/,$(SRCS_BONUS:.c=.o))
DEPS_MAIN   = $(OBJS_MAIN:.o=.d)
DEPS_BONUS  = $(OBJS_BONUS:.o=.d)

# Debug symbols when DEBUG=1, e.g., `make DEBUG=1 memcheck ARGS="..."`
DEBUG ?= 0
ifeq ($(DEBUG),1)
  CFLAGS += -g -O0
endif

all: $(NAME)

$(NAME): $(OBJS_MAIN)
	$(CC) $(CFLAGS) $(OBJS_MAIN) $(LDFLAGS) $(LDLIBS) -o $@

bonus: $(BONUS_NAME)

$(BONUS_NAME): $(OBJS_BONUS)
	$(CC) $(CFLAGS) $(OBJS_BONUS) $(LDFLAGS) $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS_MAIN) $(OBJS_BONUS) $(DEPS_MAIN) $(DEPS_BONUS)
	rm -rf $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME) $(BONUS_NAME)

re: fclean all

-include $(DEPS_MAIN) $(DEPS_BONUS)

.PHONY: all bonus clean fclean re memcheck helgrind drd

# ================= Diagnostics (Valgrind) =================
# Pass program arguments via ARGS="..."
VALGRIND ?= valgrind

memcheck: $(NAME)
	$(VALGRIND) --leak-check=full --show-leak-kinds=all \
	  --errors-for-leak-kinds=all --track-origins=yes -q ./$(NAME) $(ARGS)

helgrind: $(NAME)
	$(VALGRIND) --tool=helgrind -q ./$(NAME) $(ARGS)

drd: $(NAME)
	$(VALGRIND) --tool=drd -q ./$(NAME) $(ARGS)
