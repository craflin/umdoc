@echo off
setlocal

if "%1" == "html" (
    echo ^<pre^>
    for /F "tokens=*" %%a in ('more') do (
      echo %%a
    )
    echo ^</pre^>
) else (
    echo \begin{verbatim}
    for /F "tokens=*" %%a in ('more') do (
      echo %%a
    )
    echo \end{verbatim}
)