CC = gcc
CXXSTD = c++11
CXXLIB = stdc++
FLAG = -std=$(CXXSTD) -l$(CXXLIB) -lm
TEX = xelatex
all: program report

program: PA02.cpp
	@echo "[*] Programming Assignment 02 : Compile..."
	@$(CC) PA01-2.cpp -o pa01-2 $(FLAG)

report: PA02-Report.tex 
	@echo "[*] Generating Report"
	@$(TEX) PA02-Report.tex 

clean:
	@rm -rf *.exe *.log *.out pa02 PA01-Report.pdf
