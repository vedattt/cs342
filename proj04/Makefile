CFLAGS=-g -Wall

SRCDIR=./src
OBJDIR=./obj

SRCS=main.c dir_entry_list.c

OBJS=$(addprefix $(OBJDIR)/,$(subst .c,.o,$(SRCS)))

all: diskprint

diskprint: $(OBJS)
	gcc $(CFLAGS) -o $@ $^ -lm

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	gcc $(CFLAGS) -o $@ -c $<

$(OBJDIR):
	mkdir -p $@

.PHONY: all clean

clean:
	rm -f $(OBJS) ./diskprint
	rm -rf $(OBJDIR)

