@echo off
setlocal

echo * This was created by myscript2.
for /F "tokens=*" %%a in ('more') do (
    echo %%a
)
 