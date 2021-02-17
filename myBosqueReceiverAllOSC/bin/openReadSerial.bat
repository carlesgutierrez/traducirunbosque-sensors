@echo off
taskkill /f /im javaw.exe
start /d "%~dp0readSerial" readSerial.exe
echo Lets start "%~dp0readSerial/readSerial.exe"
exit