project "slowMotionVideo"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")
    
    files
    {
        "**.h",
        "**.cpp",
    }

    includedirs
    {
        "%{IncludeDir.Camera_PIXELINK}",
    }

    libdirs
    {
        "%{wks.location}/NMROSCore/vendor/Camera/libs/PIXELINK/lib/x64",
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
