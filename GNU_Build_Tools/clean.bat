@echo off
if exist Makefile mingw32-make DIR_DIV=\\ RM=del -f ./Makefile clean
