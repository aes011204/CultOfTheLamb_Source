// 명령어			옵션			원본 파일이 있는 위치				사본 파일을 저장할 위치
	
xcopy			/y			.\Engine\Bin\Engine.dll				.\Client\Bin\
xcopy			/y			.\Engine\Bin\Engine.lib				.\Reference\Library\

xcopy			/y			.\Engine\Header\*.*					.\Reference\Header\
xcopy 		/y 			.\EditorTool\Bin\EditorTool.lib 			.\Reference\Library\
xcopy 		/y 			.\EditorTool\Header\CImGuiManager.h 	.\Reference\Header\