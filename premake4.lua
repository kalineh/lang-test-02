
solution "kl"
   configurations { "Debug", "Release", }

   location "out/"
 
   project "kl"
      kind "ConsoleApp"
      language "C++"
      files
      {
         "**.h",
         "**.cpp",
      }

      includedirs
      {
      }

      libdirs
      {
      }

      links
      {
      }

      buildoptions
      {
        -- "/FI\"core.h\"",
        -- "/FI\"PCH.h\"",
        -- "/MP2",
      }

      -- debugdir ".."
      debugargs { }
      debugenvs { }

      configuration "Debug"
         targetname "kl.debug"
         defines { "DEBUG" }
         flags { "Symbols" }

      configuration "Release"
         targetname "kl.release"
         defines { "NDEBUG" }
         flags { "Optimize" }    
         buildoptions { "/d2Zi+", } --release mode debug info

   project "libs"
      kind "ConsoleApp"
      language "C++"
      files
      {
         -- convenient headers
      }
