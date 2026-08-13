// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "pch.h"

//----------------------------------------------------------------------------------
//	サンプルウィンドウクライアントプラグイン for AviUtl ExEdit2
//----------------------------------------------------------------------------------
#include <windows.h>
#include <commctrl.h>

#include <SDK/plugin2.h>
#include <SDK/logger2.h> // ログ出力 
#include <SDK/config2.h> // 設定関連

#include "MainWindow.h"

#define SampleWindowName L"SampleWindowClient"
#define IDC_BUTTON 1001
EDIT_HANDLE* edit_handle;
LOG_HANDLE* logger;
CONFIG_HANDLE* config;

//---------------------------------------------------------------------
//	汎用プラグイン構造体定義
//---------------------------------------------------------------------
COMMON_PLUGIN_TABLE common_plugin_table = {
	L"CustomToolbox",								// プラグインの名前
	L"CustomToolbox ver.0.1 by Kanata",		// プラグインの情報
};

//---------------------------------------------------------------------
//	必要とする本体バージョン番号取得関数 (未定義なら呼ばれません)
//---------------------------------------------------------------------
EXTERN_C __declspec(dllexport) DWORD RequiredVersion() {
	return 2003300;
}

//---------------------------------------------------------------------
//	ログ出力機能初期化関数 (未定義なら呼ばれません)
//---------------------------------------------------------------------
EXTERN_C __declspec(dllexport) void InitializeLogger(LOG_HANDLE* handle) {
	MainWindow::setLogger(handle);
}

//---------------------------------------------------------------------
//	設定関連初期化関数 (未定義なら呼ばれません)
//---------------------------------------------------------------------
EXTERN_C __declspec(dllexport) void InitializeConfig(CONFIG_HANDLE* handle) {
	MainWindow::setConfig(handle);
}

//---------------------------------------------------------------------
//	プラグインDLL初期化関数 (未定義なら呼ばれません)
//---------------------------------------------------------------------
EXTERN_C __declspec(dllexport) bool InitializePlugin(DWORD version) {
	return true;
}

//---------------------------------------------------------------------
//	プラグインDLL解放関数 (未定義なら呼ばれません)
//---------------------------------------------------------------------
EXTERN_C __declspec(dllexport) void UninitializePlugin() {
}

//---------------------------------------------------------------------
//	汎用プラグイン構造体のポインタを渡す関数
//---------------------------------------------------------------------
EXTERN_C __declspec(dllexport) COMMON_PLUGIN_TABLE* GetCommonPluginTable(void) {
	return &common_plugin_table;
}


//---------------------------------------------------------------------
//	プラグイン登録関数
//---------------------------------------------------------------------
EXTERN_C __declspec(dllexport) void RegisterPlugin(HOST_APP_TABLE* host) {
	MainWindow::registerPlugin(host);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

