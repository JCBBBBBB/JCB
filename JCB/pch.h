#pragma once

#define WIN32_LEAN_AND_MEAN
#define _HAS_STD_BYTE 0  // ← 이게 맨 위에 있어야 함
//#include "Types.h"
//#include "Values.h"
//#include "Struct.h"



#include "Structs.h"
#include "Types.h"
#include "Values.h"

// STL
#include <vector>
#include <list>
#include <map>
#include <string>
#include <unordered_map>
using namespace std;

//WINAPI
#include <Windows.h>
#include <assert.h>
#include <memory>

//DirectX
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>
using namespace DirectX;
using namespace Microsoft::WRL;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex\\DirectXTex_Debug.lib")
#else
#pragma comment(lib, "DirectXTex\\DirectXTex.lib")
#endif