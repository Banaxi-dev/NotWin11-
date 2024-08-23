@echo off
setlocal

set /p drive=Geben Sie den Laufwerksbuchstaben ein (z.B. C): 

if not exist %drive%:\ (
    echo Das Laufwerk %drive% existiert nicht.
    exit /b 1
)

%drive%:
cd \ 

if not exist setup.exe (
    echo Die Datei setup.exe wurde nicht gefunden.
    exit /b 1
)

setup.exe /product server

endlocal
pause
