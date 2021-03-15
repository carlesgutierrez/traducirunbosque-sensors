# traducirunbosque-sensors
Varios microcontroladores (Esp32, Arduino-MKR, SmartCitizen) envían datos sensores (espectrómetro, PH y humedad de la tierra, temperatura y humedad ambiente, cámara termal, Datos de smartCitizen ) a un software que mapea, muestra y reenvía todos los datos a terceros vía OSC.


# start all apps
create a bat one folder app that runs all apps. 
Example: 

@echo off
start /d "%~dp0traducirunbosque-sensors\myBosqueReceiverAllOSC\bin" myBosqueReceiverAllOSC.exe
start /d "%~dp0traducirUnBosque-Sonido" invernadero2.pd
echo Lets start "Sound and Sensors"
exit
