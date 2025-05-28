@echo off
set render=src\engine\render\render.c src\engine\render\render_init.c src\engine\render\render_util.c
set io=src\engine\io\io.c
set config=src\engine\config\config.c
set audio=src\engine\audio\audio.c
set input=src\engine\input\input.c
set time=src\engine\time\time.c
set physics=src\engine\physics\physics.c
set array_list=src\engine\array_list\array_list.c
set entity=src\engine\entity\enitity.c
set animation=src\engine\animation\animation.c
set files=src\glad.c src\main.c src\engine\global.c %render% %io% %config% %input% %time% %physics% %array_list% %entity% %animation% %audio%

REM Setează calea corectă către librăriile tale
set libs="C:\Users\Deborah\OneDrive\Desktop\project\lib\SDL2main.lib" "C:\Users\Deborah\OneDrive\Desktop\project\lib\SDL2.lib" "C:\Users\Deborah\OneDrive\Desktop\project\lib\freetype.lib" "C:\Users\Deborah\OneDrive\Desktop\project\lib\SDL2_ttf.lib" "C:\Users\Deborah\OneDrive\Desktop\project\lib\SDL2_mixer.lib" 

CL /Zi ^
/I ..\include ^
/I ..\include\SDL2 ^
/I "C:\Users\Deborah\OneDrive\Desktop\project\include" ^
/I src ^
/I src\engine ^
/I src\engine\render ^
/I deps\include ^
%files% /link %libs% /OUT:mygame.exe


























