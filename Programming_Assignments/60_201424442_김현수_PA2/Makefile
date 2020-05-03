CC = g++
CXXSTD = c++11
FLAG = -O2 -std=$(CXXSTD)
TEX = xelatex
all: program report

program: PA02.cpp
	@echo "[*] Programming Assignment 02 : Compile..."
	@$(CC) $(FLAG) PA02.cpp -o pa02

report: PA02-Report.tex 
	@echo "[*] Generating Report"
	@$(TEX) PA02-Report.tex 

clean:
	@rm -rf *.aux *.exe *.log *.out pa02 PA02-Report.pdf
