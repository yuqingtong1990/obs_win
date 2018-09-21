#pragma once

//检查必要的路径是否已经创建(没有创建会创建, 创建失败的话返回false)
bool checkNecessaryclientPath();

//获取data文件夹
std::wstring getObsDataDirPath();
//获取plugins文件夹
std::wstring getObsPluginsPath();
