## 	**1. About program:**

Service *"SystemAnalyzer"* can analyze computer components and report user about received information.

***Limitations**:  Service works only on Windows OS, x64 architecture*

## **2. Functionality**

- ***Drive space analysis***  
	Provides information about capacity and free space on all drives on the computer. 

- ***Internet connection status***  
	Provides information about status of internet connection(online/offline)

- ***CPU load status***  
	Provides information about percentage of CPU load at the current moment.

## **3. Service installation:**
3.1.	Copy path to your .exe file: *`project_folder/bin/x64/Release/`*

3.2 Open command prompt(**as an administrator**)

3.3 Go to the directory of .exe file  

3.4 To install or to uninstall the service, use following commands:  
- in powershell:  *`./<path to your .exe file> /install`* *`/uninstall`* 

- in cmd: *`<path to your .exe file> /install`* *`/uninstall`*

3.5 Find SystemAnalyzer in windows services.  

3.6 Enjoy!

## **4. Principle of work:**
When the service launches, it reads configuration file and uses received program parameters.
From the moment of service start and to the moment of service stop "SystemAnalyzer" logs received information into log file. This information can be read by user during service work or after service stops.

## **5. Configuration file:**
Configuration file is located in folder "config" and has name "config.json". Configuration file contains parameters which service uses for work.
Currently, config.json contains following parameters:

- 	***"minimalDiskSpaceDelta"*** stands for minimal shift of free space on 		disk to log it(in megabytes). For example, if "minimalDiskSpaceDelta" = 10, 	service will log about space changes only when free space on disk is 			changed for >=10 megabytes.  
	***Limitations:*** this parameter must be written as integer value without quotes.  
	***Default value:*** 10

- 	***"log_path"*** stands for path where log file will be created and saved.  
	***Limitations:*** this parameter must be written in quotes and with 		correct path. If path doesn't exist on computer, log file won't be created. Example: "D:\Programs";   
	***Default value:*** "../../../"  (project folder)

-	***"log_name"*** stands for log file name.  
	***Limitations:*** this parameter must be written in quotes likely with '.txt' format  
	***Default value:*** "log.txt"

-	***"diskCriticalSpace"*** - if percent of free space on disk is lesser 			than this parameter, system will log message about it.  
	***Limitations:*** this parameter must be written as integer value without quotes   
	***Default value:*** 10

-	***"cpuCriticalLoad"*** - if percent of CPU load higher than this 				parameter, system will log message about it.  
	***Limitations:*** this parameter must be written as integer value without quotes   
	***Default value:*** 30

-	***"cpuLoadDelay"*** - pause(in seconds) after which service logs 			 information about cpu load.
	***Limitations:*** this parameter must be written as integer value without quotes    
	***Default value:*** 20

-	***"diskStatusDelay"*** - pause(in seconds) after which service logs 			 information about disk storage status.    
	***Limitations:*** this parameter must be written as integer value without quotes    
	***Default value:*** 60 

-	***"internetStatusDelay"*** - pause(in seconds) after which service logs 			 information about internet connection status.  
	***Limitations:*** this parameter must be written as integer value without quotes    
	***Default value:*** 60 

### **Important notes:** 
-	Configuration file must be located in "config" folder and must be named as "config.json"

-	If service can't find "config.json", it will use default values.

-	If service can't parse "config.json", it will use default values.

-	If values in "config.json" have incorrect format or type, service will use default values.

-	If user adds custom parameters to "config.json", service will ignore them.

-	When service starts, it logs info about success/fail of reading config file.


## **6. Log file:**
Log file contains info about service work. Service writes information about reading config files and system status. Log file can be deleted.



