// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件: 
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO:  在此处引用程序需要的其他头文件
#include "resource.h"

//stl
#include <string>
#include <vector>
#include <list>
#include <queue>
#include <memory>
#include <functional>
#include <unordered_map>

#include "CqtUtil.h"
#include "ObserContainer.hpp"
#include "threadTaskPool.h"
// 包含libobs
#include "obs.h"
#include "obs.hpp"
#include "util.hpp"
#include "platform.h"

#include "obs_define.h"
#include "obs-logging.h"