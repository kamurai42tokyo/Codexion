# ============================ 変数の定義 =============================
# 
#  NAME   ： 生成する実行ファイルの名前
#  CC     ： コンパイラ
#  CFLAGS ： コンパイラのフラグ
#  SRCS   ： ソースファイル（.c）
#  OBJS   ： オブジェクトファイル（ ".c" を ".o" に置き換えたもの）
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



# =========================== ルールの定義 ============================
#
#  make all     ： 引数なしの make コマンドが実施する処理
#  make $(NAME) ： ソースファイルをコンパイルし、実行ファイルを生成
#  make xxx.o   ： xxx.c を１つだけコンパイルし、xxx.o を作成 
#  make clean   ： コンパイル過程で生成されたオブジェクトファイルを削除
#  make fclean  ： 生成された実行ファイルを削除
#  make re      ： 生成された実行ファイルを削除し、再度実行ファイルを生成
# 
#  .PHONY       ： ここに記載している名前のファイルがフォルダにあっても、
#               　 そのルールを必ず実行させる
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
