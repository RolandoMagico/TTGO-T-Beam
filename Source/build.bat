@echo off
call %~dp0\..\..\esp-idf\export.bat
idf.py %*