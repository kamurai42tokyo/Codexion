# ============================ Variables =============================
#
#  NAME   : Name of the executable to build
#  CC     : Compiler
#  CFLAGS : Compiler flags
#  SRCS   : Source files (.c)
#  OBJS   : Object files (SRCS with ".c" replaced by ".o")
#
# ===================================================================

NAME	= codexion

CC		= cc
CFLAGS	= -Wall -Wextra -Werror -pthread

SRCS	= main.c \
		  parse.c \
		  init.c \
		  simulation.c \
		  free.c \
		  threads/actions.c \
		  threads/coder.c \
		  threads/dongle.c \
		  threads/monitor.c \
		  threads/print_log.c \
		  threads/request.c \
		  threads/stop.c \
		  threads/time.c \
		  threads/turn.c

OBJS	= $(SRCS:.c=.o)



# ============================== Rules ===============================
#
#  make all     : Default target when make is run without arguments
#  make $(NAME) : Compiles the sources and builds the executable
#  make xxx.o   : Compiles only xxx.c into xxx.o
#  make clean   : Removes the object files
#  make fclean  : Removes the object files and the executable
#  make re      : Runs fclean, then builds the executable again
#
#  .PHONY       : Always runs these rules, even if a file with
#                 the same name exists in the folder
#
# ===================================================================

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c codexion.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
