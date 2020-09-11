NAME=paper

all: $(NAME).pdf

$(NAME).pdf : $(NAME).md $(NAME).bib
	pandoc --bibliography $(NAME).bib --filter pandoc-citeproc $(NAME).md -o $(NAME).pdf

clean :
	rm $(NAME).pdf
