

xcopy /y/s ..\Public\"*.h" ..\..\Reference\Inc\
xcopy /y/s ..\Bin\"*.lib" ..\..\Reference\bin\
xcopy /y/s ..\Bin\"*.dll" ..\..\Client\Bin\


xcopy /y/s ..\ThirdPartyLib\"*.lib" ..\..\Reference\bin\
xcopy /y/s ..\ThirdPartyLib\"*.dll" ..\..\Client\Bin\