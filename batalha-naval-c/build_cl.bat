@echo off
REM ============================================================
REM  Batalha Naval - Compilador Automático (Windows, cl.exe)
REM  Requisitos: Visual Studio Build Tools com MSVC (cl.exe)
REM ============================================================

echo.
echo === Compilando projeto Batalha Naval (C) com cl.exe ===
if not exist src (
  echo ERRO: Pasta 'src' nao encontrada!
  pause
  exit /b
)

REM Compilacao com o compilador da Microsoft
cl /nologo /EHsc /Fe:batalha_naval.exe src\*.c

if %errorlevel% neq 0 (
  echo.
  echo ❌ Erro de compilacao!
  pause
  exit /b
)

echo.
echo ✅ Compilacao concluida com sucesso!
echo Arquivo gerado: batalha_naval.exe

echo.
set /p RUN="Deseja executar agora? (S/N): "
if /I "%RUN%"=="S" (
  echo.
  batalha_naval.exe
)

pause
