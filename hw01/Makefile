CFLAGS=-g -Wall

SRCDIR=./src
OBJDIR=./obj

SRCS=tree.c
OBJS=$(subst .c,.o,$(SRCS))

FULLSRCS=$(addprefix $(SRCDIR)/,$(SRCS))
FULLOBJS=$(addprefix $(OBJDIR)/,$(OBJS))

all: $(FULLOBJS)
	gcc $(CFLAGS) -o tree $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	gcc $(CFLAGS) -o $@ -c $<

$(OBJDIR):
	mkdir -p $@

report-22002643.pdf: report-22002643.tex dump/man.txt dump/ls.txt dump/touch.txt src/tree.c Makefile
	latexmk -pdf

.PHONY: zip clean

clean:
	rm -f ./tree 22002643.tar.gz ./*.synctex.gz $(FULLOBJS)
	rm -fr ./obj ./22002643
	latexmk -c

zip: report-22002643.pdf
	mkdir 22002643
	cp report-22002643.pdf 22002643/
	tar cvf 22002643.tar 22002643
	gzip 22002643.tar

