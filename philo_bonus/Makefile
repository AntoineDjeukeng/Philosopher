# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: adjeuken <adjeuken@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/09/13 14:43:15 by adjeuken          #+#    #+#              #
#    Updated: 2025/09/30 09:59:26 by adjeuken         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


NAME        = philo
OBJ_DIR     = build

CC          = cc
THREAD_FLAGS= -pthread
CFLAGS      = -Wall -Wextra -Werror -MMD -MP $(THREAD_FLAGS) \
              -Iinclude
LDFLAGS     =
LDLIBS      =

RM          = rm -f

SRC_COMMON  = \
  src/time.c  src/params_init_help.c src/num.c src/io.c src/print.c \
  src/params_core.c src/forks.c src/params_init.c \
  src/philo_core.c src/philo_forks.c src/philo_lifecycle.c \
  src/logger.c src/logger_api.c src/mem.c src/sem_utils.c \
  src/start_threads.c

SRCS_MAIN   = src/main.c $(SRC_COMMON)

OBJS_MAIN   = $(addprefix $(OBJ_DIR)/,$(SRCS_MAIN:.c=.o))
DEPS_MAIN   = $(OBJS_MAIN:.o=.d)

all: $(NAME)

$(NAME): $(OBJS_MAIN)
	$(CC) $(CFLAGS) $(OBJS_MAIN) $(LDFLAGS) $(LDLIBS) -o $@


$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS_MAIN) $(DEPS_MAIN)
	rm -rf $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

-include $(DEPS_MAIN)

.PHONY: all clean fclean re memcheck helgrind drd

# ================= Diagnostics (Valgrind) =================
# Pass program arguments via ARGS="..."
# VALGRIND ?= valgrind

# memcheck: $(NAME)
# 	$(VALGRIND) --leak-check=full --show-leak-kinds=all \
# 	  --errors-for-leak-kinds=all --track-origins=yes  ./$(NAME) $(ARGS)

# helgrind: $(NAME)
# 	$(VALGRIND) --tool=helgrind  ./$(NAME) $(ARGS)

# drd: $(NAME)
# 	$(VALGRIND) --tool=drd  ./$(NAME) $(ARGS)
