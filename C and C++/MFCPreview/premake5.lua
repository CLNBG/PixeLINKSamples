project "MFCPreview"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++17"

    targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")
    
    files
    {
        "**.h",
        "**.cpp",
        "**.rc",
    }

    flags
    {
      "MFC"
    }
    characterset ("ASCII")
    defines {"_AFXDLL"}
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
