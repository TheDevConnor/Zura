# build.ps1
# Build script using MSVC

$src_files = Get-ChildItem -recurse ./src/*.cpp

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

        # Link
        $obj_files = Get-ChildItem ./bin/*.obj
        # CL.exe /D DEBUG /std:c++20 /Z7 /nologo /W3 /MDd /Od /EHsc /I.\inc\ /Fe:.\bin\ $obj_files /link /SUBSYSTEM:console
    }

    'release' {

        Write-Host "== Building Release =="
        # Compile
        CL.exe /D NDEBUG /std:c++20 /nologo /MD /O2 /EHsc /I.\inc\ /c /Fo:.\obj\release.obj .\src\main.cpp /link /SUBSYSTEM:console
        # Link
        CL.exe /D NDEBUG /std:c++20 /nologo /MD /O2 /EHsc /I.\inc\ /Fe:.\bin\release.exe .\obj\release.obj /link /SUBSYSTEM:console
    }

    'debug-gcc' {

        Write-Host "== Building Debug using GCC =="
        g++.exe -DDEBUG -std=c++17 $src_files -o ./bin/zura_gcc_debug.exe -Wall -Wextra -g -O0 

    }


    'edit' {
        gvim.exe $my_src_files
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
        
        $lib_headers = Get-ChildItem -recurse ./inc/*.h

        if( (Test-Path ./tags_inc) -eq $false){
            Write-Host "Library headers being tagged..."
            ctags.exe --totals=yes --c++-kinds=+p+l -f tags_inc $lib_headers
        }
        Copy-Item ./tags_inc ./tags
        ctags.exe -a --totals=yes --c++-kinds=+p+l $my_src_files
    }


    'search' { 
        Write-Host "== Searching Files =="
        $all_inc_files = Get-ChildItem -recurse .\inc\*.h
        $files_to_search = $all_inc_files + $my_src_files
        get-childItem $files_to_search | Select-String $args[1]
    }

    'count' {
        Write-Host "== Line Count of Source Files =="
        get-childitem $my_src_files | Get-Content | measure-object -line | Format-List Lines
    }

    'remedybg' {
        Write-Host "== Launching RemedyBG =="
        remedybg.exe debug_session.rdbg
    }

    'get_libs'{
	    .\build.ps1 get_sdl
	    .\build.ps1 get_cglm
    }


    'get_sdl'{

        # Build both Debug and Release configurations

        $sdl_version = "release-2.28.0"
        
        if( (Test-Path .\inc\SDL2) -eq $false){
            New-Item -type directory -path .\inc\SDL2
        }
        $sdl_inc_dir = Get-ItemProperty .\inc\SDL2

        Push-Location .\etc

        if( (Test-Path .\SDL) -eq $false){
            git clone https://github.com/libsdl-org/SDL.git
        }

        Push-Location .\SDL
        git checkout $sdl_version

        if( (Test-Path .\build) -eq $false){
            New-Item -type directory -path .\build
        }

        Push-Location ./build
        # Alternative compilers can be used when cmake is notifed with first command such as: 
        # cmake.exe -G "MingW Makefiles" -D CMAKE_C_COMPILER=gcc.exe -D CMAKE_CXX_COMPILER=g++.exe ..
        cmake.exe ..
        cmake.exe --build . -j 12 --config=Release
        cmake.exe --build . -j 12 --config=Debug
       
        
        # Debug config header is same as release header
        Copy-Item ".\include\SDL2\*.h" $sdl_inc_dir
        Copy-Item ".\include-config-release\SDL2\*.h" $sdl_inc_dir
        # Copy-Item ".\include-config-debug\*.h" $sdl_inc_dir
        Copy-Item ".\Release\*.lib" $lib_dir
        Copy-Item ".\Release\*.exp" $lib_dir
        Copy-Item ".\Release\*.dll" $bin_dir

        Copy-Item ".\Debug\*.lib" $lib_dir
        Copy-Item ".\Debug\*.exp" $lib_dir
        Copy-Item ".\Debug\*.dll" $bin_dir

        Pop-Location
        Pop-Location
        Pop-Location
    }


    'get_cglm'{

        # Build both Debug and Release configurations

        $cglm_version = "v0.9.0"
        
        Push-Location .\etc

        if( (Test-Path .\cglm) -eq $false){
            git clone https://github.com/recp/cglm.git
        }

        Push-Location .\cglm
        git checkout $cglm_version

        if( (Test-Path .\build) -eq $false){
            New-Item -type directory -path .\build
        }

        Push-Location ./build
        cmake.exe ..
        cmake.exe --build . -j 12 --config=Release

        cmake.exe --build . -j 12 --config=Debug

        # Append letter 'd' to signify debug build items
        Push-Location ./Debug
        Get-ChildItem * | Rename-Item -Newname { $_.BaseName + "d" + $_.Extension } 
        Pop-Location

        Copy-Item ".\Release\*.lib" $lib_dir
        Copy-Item ".\Release\*.exp" $lib_dir
        Copy-Item ".\Release\*.dll" $bin_dir

        Copy-Item ".\Debug\*.lib" $lib_dir
        Copy-Item ".\Debug\*.exp" $lib_dir
        Copy-Item ".\Debug\*.dll" $bin_dir
        
        Pop-Location
        Copy-Item -recurse ".\include\cglm" $inc_dir

        Pop-Location
        Pop-Location
    }


}
