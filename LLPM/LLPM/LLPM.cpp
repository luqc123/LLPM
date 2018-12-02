// LLPM.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "..\..\Library\Basic.h"

using namespace ll;

int main()
{
	int boolean = 1;
	CHECK_ERROR(boolean == 0 || boolean == 1, L"boolean 参数必须是0或者1.");
	return 0;
}

