NAME=sish
SRC=tokenize.c execute.c freearray.c
OBJ=$(SRC:.c=.o)
RM = rm -f
IPATH = .
CFLAGS += -I$(IPATH)
LDFLAGS += -Llib
LDLIBS += -lsish
all: $(NAME) fclean
$(NAME):$(OBJ)
	ar -rc ./libsish.a $(OBJ)
	ranlib ./libsish.a
	cc -Wall -L$(IPATH) -I$(IPATH) main.c $(LDLIBS) -o $(NAME)
clean:
	$(RM) *~
	$(RM) *.o
	$(RM) *.core
	$(RM) *.swp
	$(RM) \#*
fclean: clean
	$(RM) ./libsish.a
re: fclean all
