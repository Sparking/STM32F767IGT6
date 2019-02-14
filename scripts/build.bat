@echo off
if exist scripts\update_objects.py python scripts\update_objects.py
if exist Makefile mingw32-make -j8 all
