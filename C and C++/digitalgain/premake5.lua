project "digitalgain"
    kind "ConsoleApp"
    language "C"
    cdialect "C17"

    targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")
    
    files
    {
        "**.h",
        "**.c",
    }

    includedirs
    {
        "%{IncludeDir.Camera_PIXELINK}",
    }

    libdirs
    {
        "%{wks.location}/NMROSCore/vendor/Camera/libs/PIXELINK/lib/Win64_x64",
    }
      
    links
    {
        "PxLAPI40.lib",
    }

    filter "system:windows"
        systemversion "latest"
        
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"
        staticruntime "off"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
        staticruntime "on"
