CFLAGS=-g -Wall

SRCDIR=./src
OBJDIR=./obj

SRCS=main.c utility.c params.c paging.c

OBJS=$(addprefix $(OBJDIR)/,$(subst .c,.o,$(SRCS)))

all: pagesim

pagesim: $(OBJS)
	gcc $(CFLAGS) -o $@ $^ -lm

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	gcc $(CFLAGS) -o $@ -c $<

$(OBJDIR):
	mkdir -p $@

.PHONY: all clean

clean:
	rm -f $(OBJS) ./pagesim
	rm -rf $(OBJDIR)

