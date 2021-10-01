//  exports.h
//
//	Simple header to instruct the linker to forward function exports to another library.
//

#pragma comment(linker,"/export:GetFileVersionInfoA=VERSION_orig.GetFileVersionInfoA,@1")
#pragma comment(linker,"/export:GetFileVersionInfoByHandle=VERSION_orig.GetFileVersionInfoByHandle,@2")
#pragma comment(linker,"/export:GetFileVersionInfoExA=VERSION_orig.GetFileVersionInfoExA,@3")
#pragma comment(linker,"/export:GetFileVersionInfoExW=VERSION_orig.GetFileVersionInfoExW,@4")
#pragma comment(linker,"/export:GetFileVersionInfoSizeA=VERSION_orig.GetFileVersionInfoSizeA,@5")
#pragma comment(linker,"/export:GetFileVersionInfoSizeExA=VERSION_orig.GetFileVersionInfoSizeExA,@6")
#pragma comment(linker,"/export:GetFileVersionInfoSizeExW=VERSION_orig.GetFileVersionInfoSizeExW,@7")
#pragma comment(linker,"/export:GetFileVersionInfoSizeW=VERSION_orig.GetFileVersionInfoSizeW,@8")
#pragma comment(linker,"/export:GetFileVersionInfoW=VERSION_orig.GetFileVersionInfoW,@9")
#pragma comment(linker,"/export:VerFindFileA=VERSION_orig.VerFindFileA,@10")
#pragma comment(linker,"/export:VerFindFileW=VERSION_orig.VerFindFileW,@11")
#pragma comment(linker,"/export:VerInstallFileA=VERSION_orig.VerInstallFileA,@12")
#pragma comment(linker,"/export:VerInstallFileW=VERSION_orig.VerInstallFileW,@13")
#pragma comment(linker,"/export:VerLanguageNameA=VERSION_orig.VerLanguageNameA,@14")
#pragma comment(linker,"/export:VerLanguageNameW=VERSION_orig.VerLanguageNameW,@15")
#pragma comment(linker,"/export:VerQueryValueA=VERSION_orig.VerQueryValueA,@16")
#pragma comment(linker,"/export:VerQueryValueW=VERSION_orig.VerQueryValueW,@17")
