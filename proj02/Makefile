CFLAGS=-g -Wall

SRCDIR=./src
OBJDIR=./obj

SRCS=cfs.c generator.c main.c process.c scheduler.c utility.c workload_params.c

OBJS=$(addprefix $(OBJDIR)/,$(subst .c,.o,$(SRCS)))

all: cfs

cfs: $(OBJS)
	gcc $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	gcc $(CFLAGS) -o $@ -c $<

$(OBJDIR):
	mkdir -p $@

.PHONY: all clean

clean:
	rm -f $(OBJS) ./cfs
	rm -rf $(OBJDIR)

