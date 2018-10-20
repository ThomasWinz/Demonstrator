pscp.exe -pw openhabian ..\openhabkonfig\items\*.items openhabian@filderkoffer:/etc/openhab2/items

pscp.exe -pw openhabian ..\openhabkonfig\sitemaps\*.sitemap openhabian@filderkoffer:/etc/openhab2/sitemaps 

pscp.exe -pw openhabian ..\openhabkonfig\things\*.things openhabian@filderkoffer:/etc/openhab2/things 

pscp.exe -pw openhabian ..\openhabkonfig\rules\*.rules openhabian@filderkoffer:/etc/openhab2/rules

pscp.exe -pw openhabian ..\openhabkonfig\maps\*.map openhabian@filderkoffer:/etc/openhab2/transform

pscp.exe -pw openhabian ..\openhabkonfig\cfg\*.cfg openhabian@filderkoffer:/etc/openhab2/services
