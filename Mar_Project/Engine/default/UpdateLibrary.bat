

xcopy /y ..\Public\"*.h" ..\..\Reference\Inc\
xcopy /y ..\Bin\"*.lib" ..\..\Reference\bin\
xcopy /y ..\Bin\"*.dll" ..\..\Client\Bin\


xcopy /y ..\ThirdPartyLib\"*.lib" ..\..\Reference\bin\
xcopy /y ..\ThirdPartyLib\"*.dll" ..\..\Client\Bin\