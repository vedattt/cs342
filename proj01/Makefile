CFLAGS=-g -Wall

SRCDIR=./src
OBJDIR=./obj

PWORDSRCS=common.c pword.c
TWORDSRCS=common.c tword.c

PWORDOBJS=$(addprefix $(OBJDIR)/,$(subst .c,.o,$(PWORDSRCS)))
TWORDOBJS=$(addprefix $(OBJDIR)/,$(subst .c,.o,$(TWORDSRCS)))

all: pword tword

pword: $(PWORDOBJS)
	gcc $(CFLAGS) -o $@ $^

tword: $(TWORDOBJS)
	gcc $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	gcc $(CFLAGS) -o $@ -c $<

$(OBJDIR):
	mkdir -p $@

.PHONY: all clean

clean:
	rm -f $(PWORDOBJS) $(TWORDOBJS) ./pword ./tword
	rmdir $(OBJDIR)

