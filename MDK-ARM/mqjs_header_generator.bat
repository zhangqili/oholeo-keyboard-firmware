@echo off
cd /d %~dp0

gcc -o mqjs_header_generator.exe ../libamp/src/mquickjs/mqjs_libamp_stdlib.c ../libamp/lib/mquickjs/mquickjs_build.c -I../libamp/lib/mquickjs -I../libamp_user -DNDEBUG

mqjs_header_generator.exe -a -m32 > .\mquickjs_atom.h
mqjs_header_generator.exe -m32 > .\mqjs_stdlib.h

if not exist .\sys (
    mkdir .\sys
)

if not exist .\sys\time.h (
    echo /* Dummy time.h for Keil compatibility */ > .\sys\time.h
) else (
    echo sys\time.h already exists.
)

