# build.ps1
# Build script using MSVC

$src_files = Get-ChildItem -recurse *.cpp
$inc_files = Get-ChildItem -recurse *.hpp,*.h
$all_files = $inc_files + $src_files 

Switch ($args[0])
{
    $null {
        .\build.ps1 debug
    }
    

    'debug' {

        Write-Host "== Building Debug =="
        # Compile
        # /MDd gives linker warnings if you don't build dependancies with it.
        CL.exe /D _CRT_SECURE_NO_WARNINGS /std:c++20 /Z7 /nologo /W3 /MDd /Od /EHsc /I.\inc\ /Fe:.\bin\zura_debug.exe $src_files /link /SUBSYSTEM:console

        # Don't separate Linking step yet
        # Link
        # $obj_files = Get-ChildItem ./bin/*.obj
        # CL.exe /D DEBUG /std:c++20 /Z7 /nologo /W3 /MDd /Od /EHsc /I.\inc\ /Fe:.\bin\ $obj_files /link /SUBSYSTEM:console
    }

    'release' {

        Write-Host "== Building Release =="
        # Compile
        # CL.exe /D NDEBUG /std:c++20 /nologo /MD /O2 /EHsc /I.\inc\ /c /Fo:.\obj\release.obj .\src\main.cpp /link /SUBSYSTEM:console
        # Link
        # CL.exe /D NDEBUG /std:c++20 /nologo /MD /O2 /EHsc /I.\inc\ /Fe:.\bin\release.exe .\obj\release.obj /link /SUBSYSTEM:console
    }

    'debug-gcc' {

        Write-Host "== Building Debug using GCC =="
        g++.exe -DDEBUG -std=c++17 $src_files -o ./bin/zura_gcc_debug.exe -Wall -Wextra -g -O0 

    }


    'edit' {
        gvim.exe $src_files
    }

    'format'{
        Write-Host "== Formatting Project =="
        clang-format.exe -i --style=file $my_src_files
    }

    'clean' {
        Write-Host "== Cleaning Up Files =="
        $remove_items = Get-ChildItem -exclude inc,etc,lib | get-childitem -recurse -depth 1 -include *.obj,.\vk_layer_log.txt,.\tags,.\tags_inc, *.idb, *.ilk, *.pdb, .\*.obj, *.exe, *~
        $num_items = ($remove_items | Measure-Object).count
        Write-Host "Removing " $num_items " file(s)."
        if( $remove_items -ne $null){ Remove-Item $remove_items }
    }

    'tags' {
        Write-Host "== Tagging Files =="
        ctags.exe -a --totals=yes --c++-kinds=+p+l $all_files
    }


    'search' { 
        Write-Host "== Searching Files =="
        get-childItem $all_files | Select-String $args[1]
    }

    'remedybg' {
        Write-Host "== Launching RemedyBG =="
        remedybg.exe .\bin\zura_debug.exe    
    }

}
