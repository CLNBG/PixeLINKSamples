project "AviCapture"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++17"

    targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
    objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")
    
    pchheader "stdafx.h"
    pchsource "%{wks.location}/NMROSCore/vendor/Camera/PixeLINKSamples/C and C++/avicapture/stdafx.cpp"
    
    characterset ("ASCII")
    
    defines
    {
      "_AFXDLL"
    }

    files
    {
        "**.h",
        "**.cpp",
        "**.rc",
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
        "NMROSCore"
    }
    
    filter "system:windows"
        systemversion "latest"
        
    filter "configurations:Debug"
        defines "NMROS_DEBUG"
        runtime "Debug"
        symbols "on"
        staticruntime "off"

    filter "configurations:Release"
        defines "NMROS_RELEASE"
        runtime "Release"
        optimize "on"
        staticruntime "on"
