#include <Python.h>
#include <string>
#include <fstream>
#include <streambuf>
#include "FileGlobBase.h"
#include "FileGlobList.h"
#include <map>
#include <set>
#include <functional>

#include <direct.h>
#ifdef WINDOWS
#include <windows.h>
#ifdef HAVE_JUNCTIONS
//#include "reparselib.h"


#define REPARSE_MOUNTPOINT_HEADER_SIZE   8

typedef struct {
  DWORD ReparseTag;
  DWORD ReparseDataLength;
  WORD Reserved;
  WORD ReparseTargetLength;
  WORD ReparseTargetMaximumLength;
  WORD Reserved1;
  WCHAR ReparseTarget[1];
} REPARSE_MOUNTPOINT_DATA_BUFFER, *PREPARSE_MOUNTPOINT_DATA_BUFFER;

static void CreateJunction(LPCSTR szJunction, LPCSTR szPath) {
  BYTE buf[sizeof(REPARSE_MOUNTPOINT_DATA_BUFFER) + MAX_PATH * sizeof(WCHAR)];
  REPARSE_MOUNTPOINT_DATA_BUFFER& ReparseBuffer = (REPARSE_MOUNTPOINT_DATA_BUFFER&)buf;
  char szTarget[MAX_PATH] = "\\??\\";

  strcat(szTarget, szPath);
  strcat(szTarget, "\\");

  if (!::CreateDirectory(szJunction, NULL)) throw ::GetLastError();

  // Obtain SE_RESTORE_NAME privilege (required for opening a directory)
  HANDLE hToken = NULL;
  TOKEN_PRIVILEGES tp;
  try {
    if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) throw ::GetLastError();
    if (!::LookupPrivilegeValue(NULL, SE_RESTORE_NAME, &tp.Privileges[0].Luid))  throw ::GetLastError();
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    if (!::AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))  throw ::GetLastError();
  }
  catch (DWORD) { }   // Ignore errors
  if (hToken) ::CloseHandle(hToken);

  HANDLE hDir = ::CreateFile(szJunction, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS, NULL);
  if (hDir == INVALID_HANDLE_VALUE) throw ::GetLastError();

  memset(buf, 0, sizeof(buf));
  ReparseBuffer.ReparseTag = IO_REPARSE_TAG_MOUNT_POINT;
  int len = ::MultiByteToWideChar(CP_ACP, 0, szTarget, -1, ReparseBuffer.ReparseTarget, MAX_PATH);
  ReparseBuffer.ReparseTargetMaximumLength = (len--) * sizeof(WCHAR);
  ReparseBuffer.ReparseTargetLength = len * sizeof(WCHAR);
  ReparseBuffer.ReparseDataLength = ReparseBuffer.ReparseTargetLength + 12;

  DWORD dwRet;
  if (!::DeviceIoControl(hDir, FSCTL_SET_REPARSE_POINT, &ReparseBuffer, ReparseBuffer.ReparseDataLength+REPARSE_MOUNTPOINT_HEADER_SIZE, NULL, 0, &dwRet, NULL)) {
    DWORD dr = ::GetLastError();
    ::CloseHandle(hDir);
    ::RemoveDirectory(szJunction);
    throw dr;
  }

  ::CloseHandle(hDir);
} // CreateJunction

#endif
#else //WINDOWS
#error
#endif

#ifdef HAVE_CURL
#include "curl/curl.h"
#endif 

std::string userdir = "";
std::string bindir = "";
std::string packagesdir = "";
std::string modulesdir = "";
std::string file = "BUILDENV";
PyObject* obj = 0;
PyObject *options = 0;
PyObject *targets = 0;
PyObject *platforms = 0;
PyObject *main_dict = 0;

bool _nolog = false;
bool _debug = false;
std::map<std::string, PyObject*>   _modules_loaded;
std::map<std::string, std::string> _options_desc;
std::map<std::string, std::string> _options_value;
std::map<std::string, std::string> _license_file;
std::map<std::string, std::string> _environments;
std::set<std::string> _targets;

#ifdef HAVE_CURL
CURL *curl;  
CURLcode result;


// This is the writer call back function used by curl  
int writer2(char *data, size_t size, size_t nmemb, std::string *buffer)  
{  
  // What we will return  
  int result = 0;  
  
  // Is there anything in the buffer?  
  if (buffer != NULL)  
  {  
    // Append the data to the buffer  
    buffer->append(data, size * nmemb);   
    result = size * nmemb;  
  }  
	  
  return result;  
}

#endif //HAVE_CURL


static PyObject* emb_load(PyObject *self, PyObject *args)
{	
	std::string v = userdir;	
    const char *command;
    if (!PyArg_ParseTuple(args, "s", &command))
        return Py_BuildValue("");
  printf("loading... %s\n", command);
	
    if( strstr(command,"http"))
    {
		std::string name(strrchr(command,'/') + 1);
		v.append(name);
		const char *url = command;
		command = v.c_str();

		std::ifstream f(command);
    	if( f.good() == false || _debug == true )
    	{
    		f.close();

    		#ifdef HAVE_CURL    		
			std::string dbuffer3;
			curl_easy_setopt(curl, CURLOPT_URL, url);
	  		char errbuf[CURL_ERROR_SIZE];
	  		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	   		curl_easy_setopt(curl, CURLOPT_HEADER, 0);  
	   		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);  
	   		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer2);  
		   	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &dbuffer3);
	   		result = curl_easy_perform(curl);
	   		long http_code = 0;
			curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);

		    if (result == CURLE_OK && http_code == 200 )  
		    {  						
			    std::ofstream out(v,std::ofstream::binary);
			    out << dbuffer3;
			    out.close();

			    if( _debug == true ) {
					printf("Loading module packages %s\n", command);
				}
		    }
		    else
		    {
				if( _debug == true ) {
					printf("Error while loading module packages %s %s\n", command, errbuf);					
				}

		    	return Py_BuildValue("");
		    }
		    #else
		    return Py_BuildValue("");
		    if( _debug == true ) {
				printf("Loading module packages is disabled %s\n", command);
			}
		    #endif
    	}
    }


  std::string abspath, dir;
  struct stat buffer;   
  if( (stat(command, &buffer) == 0) ) 
  {
    char abspath2[4096];
    _fullpath(abspath2, command, 4096);
    dir = std::string(abspath2, strrchr(abspath2,'\\') > strrchr(abspath2, '/') ? strrchr(abspath2, '\\') : strrchr(abspath2, '/'));
    abspath = abspath2;
  }
  else
  {
    std::string temp = modulesdir;
    temp.append("/");
    temp.append(command);
    char abspath2[4096];
    _fullpath(abspath2, temp.c_str(), 4096);
    dir = std::string(abspath2, strrchr(abspath2,'\\') > strrchr(abspath2, '/') ? strrchr(abspath2, '\\') : strrchr(abspath2, '/'));
    if( (stat(abspath2, &buffer) == 0) )
    {
      abspath = abspath2;
    }
    else
    {
      switch (errno)
      {
         case ENOENT:
           printf("File %s not found.\n", abspath2);
           break;
         case EINVAL:
           printf("Invalid parameter to _stat.\n");
           break;
         default:
           /* Should never be reached. */
           printf("Unexpected error in _stat.\n");
      }      
      exit(-1);      
    }
  }


	//Lazy load the module...
	if( _modules_loaded.find(abspath) == _modules_loaded.end() ) {		
		if( _debug == true ) {
			printf("Loading new module %s\n", abspath);
		}

		std::ifstream t(abspath);
		std::string str((std::istreambuf_iterator<char>(t)),
						 std::istreambuf_iterator<char>());

		PyObject *pName, *pModule, *pArgs, *pValue, *pFunc;
		PyObject *pNewMod = PyModule_New(command);
		PyModule_AddStringConstant(pNewMod, "__file__", abspath.c_str());
		PyObject *pLocal = PyModule_GetDict(pNewMod);
		PyDict_SetItemString(pLocal, "__builtins__", PyEval_GetBuiltins());
		pValue = PyRun_String(str.c_str(), Py_file_input, pLocal, pLocal);
		if (pValue == NULL) {
		   PyErr_Print();
		}
		_modules_loaded[abspath] = pNewMod;
		return pNewMod;
	} else {
		if( _debug == true ) {
			printf("Loading existing module %s\n", abspath);
		}
		Py_INCREF(_modules_loaded.find(abspath)->second);
		return _modules_loaded.find(abspath)->second;
	}	
}

static PyObject* emb_info(PyObject *self, PyObject *args)
{
    const char *command;
    if (!PyArg_ParseTuple(args, "s", &command))
        return Py_BuildValue("");
	printf("%s\n", command);
    return Py_BuildValue("");
}

static PyObject* emb_license(PyObject *self, PyObject *args, PyObject *kwargs)
{
  const char *description, *license = "";
  static char *kwlist[] = {"description", "license", NULL};
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|s", kwlist, &description, &license))
    return Py_BuildValue("");
  char abspath2[4096];
  _fullpath(abspath2, license, 4096);
  _license_file[description] = abspath2;
  return Py_BuildValue("");
}

static PyObject* emb_environment(PyObject *self, PyObject *args, PyObject *kwargs)
{
  printf("parsing... environment information\n");
  const char *description, *license = "";
  static char *kwlist[] = {"name", "code", NULL};
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|s", kwlist, &description, &license))
    return Py_BuildValue("");
  _environments[description] = license;
  return Py_BuildValue("");
}

static PyObject* emb_option(PyObject *self, PyObject *args, PyObject *kwargs)
{
    const char *trigger, *value = "", *description = "";
	static char *kwlist[] = {"trigger", "value", "description", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|ss", kwlist, &trigger, &value, &description))
        return Py_BuildValue("");
    if(PyDict_Contains(options, Py_BuildValue("s", trigger)) == false ) {
    	PyDict_SetItemString(options, trigger, Py_BuildValue("s", value));	
    }
	
	_options_desc[trigger] = description;
	_options_value[trigger] = value;
    return Py_BuildValue("");
}

static PyObject* emb_target(PyObject *self, PyObject *args, PyObject *kwargs)
{
    const char *trigger;
	static char *kwlist[] = {"name", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s", kwlist, &trigger))
        return Py_BuildValue("");
		
	_targets.insert(std::string(trigger));
    return Py_BuildValue("");
}

static PyObject* emb_glob(PyObject *self, PyObject *args, PyObject *kwargs)
{
    PyObject* include = 0;
    PyObject* exclude = 0;
    PyObject* ignore = 0;
    static char *kwlist[] = {"include", "exclude", "ignore", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OO", kwlist, &include, &exclude, &ignore))
        return Py_BuildValue("");

    #ifdef WINDOWS
    FileGlobList glob;

    if( exclude )
    {
        for( int i = 0, c = PyList_Size(exclude); i < c; ++i ) {
            PyObject* item = PyList_GetItem(exclude, i);
            char* value = PyString_AsString(item);
            glob.AddExclusivePattern(value);
        }
    }

    if( ignore )
    {
        for( int i = 0, c = PyList_Size(ignore); i < c; ++i ) {
            PyObject* item = PyList_GetItem(ignore, i);
            char* value = PyString_AsString(item);
            glob.AddIgnorePattern(value);
        }
    }

    for( int i = 0, c = PyList_Size(include); i < c; ++i ) {
        PyObject* item = PyList_GetItem(include, i);
        char* value = PyString_AsString(item);
        glob.MatchPattern( value );
    }


    int i = 0;
    PyObject *rslt = PyList_New(glob.size());
	if( rslt !=0 )
	{
		for( FileGlobList::iterator itt = glob.begin(); itt != glob.end(); ++itt) 
		{
			PyObject* obj = Py_BuildValue("s", (*itt).c_str());
			if( obj ) 
			{
				PyList_SetItem(rslt, i, obj);
				++i;
			}
		}
	}

    return rslt;
    #else
    //TODO implement glob expressions for linux
    PyObject *rslt = PyList_New(0);
    return rslt;
    #endif
}


#ifdef HAVE_JUNCTIONS
static PyObject* emb_junction(PyObject *self, PyObject *args)
{
    const char *source, *destination;
    if (!PyArg_ParseTuple(args, "ss", &source, &destination))
        return Py_BuildValue("");
	
	char abspath1[4096], abspath2[4096];
	_fullpath(abspath1, source, 4096);
	_fullpath(abspath2, destination, 4096);

	::RemoveDirectory(source);
	//::CreateDirectory(source, NULL);

    LPSTR pszName;
	const char* dest = abspath2;
    char szPath[MAX_PATH], szLink[MAX_PATH];
    strcpy(szLink, dest);
    int len = strlen(szLink);
    if (szLink[len-1] == '\\') szLink[--len] = '\0';  // Remove trailing backslash
    if (!::GetFullPathName(abspath1, MAX_PATH, szPath, &pszName)) throw ::GetLastError();
    len = strlen(szPath);
	if (szPath[len-1] == '\\') szPath[--len] = '\0';  // Remove trailing backslash
	try {
		CreateJunction(szPath,szLink);
	} catch(...) {

	}	

    return Py_BuildValue("");
}
#endif


static PyObject* emb_guid(PyObject *self, PyObject *args)
{
	unsigned char *guidStr = 0x00;
	GUID *pguid = 0x00;
	pguid = new GUID;
	CoCreateGuid(pguid);
	UuidToString(pguid, &guidStr); 
	delete pguid;	
    return Py_BuildValue("s", guidStr);
}

static PyObject* emb_run(PyObject *self, PyObject *args)
{
    const char *command;
    if (!PyArg_ParseTuple(args, "s", &command))
        return Py_BuildValue("");

	char abspath[4096];
	_fullpath(abspath, command, 4096);
  printf("including... information %s\n", abspath);
	if( _modules_loaded.find(abspath) == _modules_loaded.end() ) 
	{
		std::string dir(abspath, strrchr(abspath,'\\') > strrchr(abspath, '/') ? strrchr(abspath, '\\') : strrchr(abspath, '/'));
		if( _debug == true ) {
			printf("Loading new build file %s\n", abspath);
		}		

		std::ifstream t(command);
		std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

		char workingDirectory[2048];
		_getcwd(workingDirectory, 2048);
		SetCurrentDirectoryA(dir.c_str());
    bool retval = RemoveDirectory("./.srp/");
    if( retval == false )
    {
      system("rmdir .\\.srp\\ /s /q");
      printf("The directory was not a junction\n");
    }

		#ifdef HAVE_JUNCTIONS
	    LPSTR pszName;
		const char* source = userdir.c_str();
		const char* dest = "./.srp/";
	    char szPath[MAX_PATH], szLink[MAX_PATH];
	    strcpy(szLink, dest);
	    int len = strlen(szLink);
	    if (szLink[len-1] == '\\') szLink[--len] = '\0';  // Remove trailing backslash
	    if (!::GetFullPathName(source, MAX_PATH, szPath, &pszName)) throw ::GetLastError();
	    len = strlen(szPath);
		if (szPath[len-1] == '\\') szPath[--len] = '\0';  // Remove trailing backslash
		try {
       if( _debug == true ) {
          printf("Creating junction %s %s\r\n", szLink, szPath);
       }
			 CreateJunction(szLink, szPath);
		} catch(...) {
       if( _debug == true ) {
          printf("Exception when trying to make junction %s %s\r\n", szLink, szPath);
       }
		}
		#else
    if( _debug == true ) {
      printf("Junctions are disabled in this version\r\n");
    }
    #endif

		PyObject* script = PyObject_GetAttrString(obj, "_SERPENT_SCRIPT");
		PyObject_SetAttrString(obj, "_SERPENT_SCRIPT", Py_BuildValue("s",command));
		PyObject* workingdir = PyObject_GetAttrString(obj, "_WORKING_DIR");
		PyObject_SetAttrString(obj, "_WORKING_DIR", Py_BuildValue("s",dir.c_str()));

		PyObject *pName, *pModule, *pArgs, *pValue, *pFunc;
		PyObject *pNewMod = PyModule_New(command);
		PyModule_AddStringConstant(pNewMod, "__file__", command);
		PyObject *pLocal = PyModule_GetDict(pNewMod);
		PyDict_SetItemString(pLocal, "serpent", obj);
		PyDict_SetItemString(pLocal, "__builtins__", PyEval_GetBuiltins());
		pValue = PyRun_String(str.c_str(), Py_file_input, pLocal, pLocal);
		if (pValue == NULL) {
		   PyErr_Print();
		}
		SetCurrentDirectoryA(workingDirectory);
		PyObject_SetAttrString(obj, "_WORKING_DIR", workingdir);
		PyObject_SetAttrString(obj, "_SERPENT_SCRIPT", script);
		_modules_loaded[abspath] = pNewMod;
    Py_INCREF(pNewMod);
		return pNewMod;
	} else {
		if( _debug == true ) {
			printf("Loading exiting build file %s\n", abspath);
		}
		return _modules_loaded.find(abspath)->second;
	}	
}


static PyObject* emb_repository_download(PyObject *self, PyObject *args)
{
	const char *id, *version;
    if (!PyArg_ParseTuple(args, "ss", &id, &version))
        return Py_BuildValue("");

	char userprofile[2048]= {0};
    if( GetEnvironmentVariable("SERPENTREPO", userprofile, 2048) != 0 ) 
    {
    	ExpandEnvironmentStrings("%SERPENTREPO%", userprofile, 2048);
    	printf("searching %s for %s %s\r\n", userprofile, id, version);

		#ifdef HAVE_CURL
    	std::string url;
    	url.append(userprofile);
    	url.append(id);
    	url.append(".");
    	url.append(version);
    	url.append(".tar.bz2");

    	std::string file;
    	file.append("./.srp/");
    	file.append(id);
    	file.append(".");
    	file.append(version);
    	file.append(".tar.bz2");    	

		std::string dbuffer3;
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  		char errbuf[CURL_ERROR_SIZE];
  		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
   		curl_easy_setopt(curl, CURLOPT_HEADER, 0);  
   		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);  
   		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer2);  
	   	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &dbuffer3);
   		result = curl_easy_perform(curl);
   		long http_code = 0;
		curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);

	    if (result == CURLE_OK && http_code == 200 )  
	    {  						
		    std::ofstream out(file.c_str(),std::ofstream::binary);
		    out << dbuffer3;
		    out.close();
	    }
	    else
	    {
	    	return Py_BuildValue("");
	    }
	    #endif

	    return Py_BuildValue("");
    }	
    else
    {
    	return Py_BuildValue("");
    }
}

static PyMethodDef EmbMethods[] = {  
  {"environment", (PyCFunction)emb_environment, METH_VARARGS | METH_KEYWORDS},
  {"license", (PyCFunction)emb_license, METH_VARARGS | METH_KEYWORDS},
	{"option", (PyCFunction)emb_option, METH_VARARGS | METH_KEYWORDS},
	{"target", (PyCFunction)emb_target, METH_VARARGS | METH_KEYWORDS},	
	{"load", (PyCFunction)emb_load, METH_VARARGS},
  {"info", (PyCFunction)emb_info, METH_VARARGS},
	{"glob", (PyCFunction)emb_glob, METH_VARARGS | METH_KEYWORDS},
	{"include", (PyCFunction)emb_run, METH_VARARGS},
	{"guid", (PyCFunction)emb_guid, METH_VARARGS},
	{"repository_search", (PyCFunction)emb_repository_download, METH_VARARGS},	
	#ifdef HAVE_JUNCTIONS
	{"junction", (PyCFunction)emb_junction, METH_VARARGS},	
	#endif 	
    {NULL, NULL, 0, NULL}
};

#ifdef WINDOWS
LPSTR* CommandLineToArgvA(LPSTR lpCmdLine, INT *pNumArgs)
{
    int retval;
    retval = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, lpCmdLine, -1, NULL, 0);
    if (!SUCCEEDED(retval))
        return NULL;

    LPWSTR lpWideCharStr = (LPWSTR)malloc(retval * sizeof(WCHAR));
    if (lpWideCharStr == NULL)
        return NULL;

    retval = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, lpCmdLine, -1, lpWideCharStr, retval);
    if (!SUCCEEDED(retval))
    {
        free(lpWideCharStr);
        return NULL;
    }

    int numArgs;
    LPWSTR* args;
    args = CommandLineToArgvW(lpWideCharStr, &numArgs);
    free(lpWideCharStr);
    if (args == NULL)
        return NULL;

    int storage = numArgs * sizeof(LPSTR);
    for (int i = 0; i < numArgs; ++ i)
    {
        BOOL lpUsedDefaultChar = FALSE;
        retval = WideCharToMultiByte(CP_ACP, 0, args[i], -1, NULL, 0, NULL, &lpUsedDefaultChar);
        if (!SUCCEEDED(retval))
        {
            LocalFree(args);
            return NULL;
        }

        storage += retval;
    }

    LPSTR* result = (LPSTR*)LocalAlloc(LMEM_FIXED, storage);
    if (result == NULL)
    {
        LocalFree(args);
        return NULL;
    }

    int bufLen = storage - numArgs * sizeof(LPSTR);
    LPSTR buffer = ((LPSTR)result) + numArgs * sizeof(LPSTR);
    for (int i = 0; i < numArgs; ++ i)
    {
        assert(bufLen > 0);
        BOOL lpUsedDefaultChar = FALSE;
        retval = WideCharToMultiByte(CP_ACP, 0, args[i], -1, buffer, bufLen, NULL, &lpUsedDefaultChar);
        if (!SUCCEEDED(retval))
        {
            LocalFree(result);
            LocalFree(args);
            return NULL;
        }

        result[i] = buffer;
        buffer += retval;
        bufLen -= retval;
    }

    LocalFree(args);

    *pNumArgs = numArgs;
    return result;
}
#endif //WINDOWS

bool parse_responsefile(const char* responseFile);

bool parse_argv(char** argv, int argc)
{
	for( int i = 0; i < argc; i++ ) {
		if( argv[i][0] == '/' && argv[i][1] == 't' && argv[i][2] == ':' ) {
			PyList_Append(targets,Py_BuildValue("s",argv[i] + 3));
		}
		else if( argv[i][0] == '/' && argv[i][1] == 'p' && argv[i][2] == ':' ) {
			PyList_Append(platforms,Py_BuildValue("s",argv[i] + 3));
		}
		else if( argv[i][0] == '/' && argv[i][1] == 'f' && argv[i][2] == ':' ) {
			file = argv[i] + 3;
		}
    else if( argv[i][0] == '/' && argv[i][1] == 'n' && argv[i][2] == 'o' && argv[i][3] == 'l' && argv[i][4] == 'o' && argv[i][5] == 'g' ) {
      _nolog = true;
    }
		else if( argv[i][0] == '-' && argv[i][1] == '-' && argv[i][2] == 't' && argv[i][3] == 0 ) {
			_debug = true;
		}
		else if( argv[i][0] == '-' && argv[i][1] == '-' && strchr(&argv[i][2], '=') != 0 ) {
			std::string name(&argv[i][2], strchr(&argv[i][2], '='));
			std::string value(strchr(&argv[i][2], '=') + 1);
			PyDict_SetItemString(options, name.c_str(), Py_BuildValue("s",value.c_str()));
		}
		else if( argv[i][0] == '@') 
		{
			const char* response = argv[i] + 1;
			bool retval = parse_responsefile(response);
			if( retval == false ) 
			{
				printf("Invalid response file %s specified\n", response );
				return 1;
			}
		}
	}
}

bool parse_responsefile(const char* responseFile)
{
	std::ifstream t(responseFile);
	if( t.bad() == false )
	{
		std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

		#ifdef WINDOWS
		int argc = 0;
		char** argv = CommandLineToArgvA((char*)str.c_str(), &argc);
		parse_argv(argv, argc);
		return true;
		#endif 
	}

	return false;
}

std::string get_environment_var()
{
	char userprofile[2048]= {0};
	GetEnvironmentVariableA("USERPROFILE", userprofile, 2048);
	return std::string(userprofile);
}

void load_serpent_home_dir()
{
	char userprofile[2048]= {0};
    if( GetEnvironmentVariable("SERPENTHOME", userprofile, 2048) == 0 ) {
    	Py_SetPythonHome(0);
    } else {
    	ExpandEnvironmentStrings("%SERPENTHOME%", userprofile, 2048);
    	Py_SetPythonHome(userprofile);
    }
}

int main(int argc, char** argv)
{
  //Create the directory
  userdir = get_environment_var();
  userdir.append("/.srp/");
  CreateDirectory(userdir.c_str(), false);
  bindir = std::string(userdir);
  bindir.append(".bin");
  CreateDirectory(bindir.c_str(), false);  
  packagesdir = std::string(userdir);
  packagesdir.append("packages");
  CreateDirectory(packagesdir.c_str(), false); 
  modulesdir = std::string(userdir);
  modulesdir.append("modules");
  CreateDirectory(modulesdir.c_str(), false);   

  std::string root = "";
  int lastKnownOption = 1;  
  for( int i = 0; i < argc; ++i ) 
  {
     if( argv[i] != nullptr && strcmp(argv[i], "-w") == 0 && argv[i + 1] != nullptr)
     {
        root = argv[i + 1];
        SetCurrentDirectoryA(root.c_str());
        lastKnownOption=i + 2;
     }
  }


  std::function<void(int,char**,int)> parse_buildenv = [](int argc, char** argv, int lastKnownOption){
      #ifdef HAVE_CURL  
      curl = curl_easy_init();
      #endif  

      //freopen("NUL:", "a", stdout);
      //freopen("NUL:", "a", stderr);
      char workingDirectory[2048];
      _getcwd(workingDirectory, 2048);

        Py_Initialize();
        load_serpent_home_dir();  



      options = PyDict_New();
      targets = PyList_New(0);
      platforms = PyList_New(0);
      parse_argv(argv + lastKnownOption + 1, argc - (lastKnownOption + 1));
      
      obj = Py_InitModule("serpent", EmbMethods);
      PyObject_SetAttrString(obj, "content", Py_BuildValue("i", false));
      PyObject_SetAttrString(obj, "action", Py_BuildValue("s", argv[lastKnownOption]));
      PyObject_SetAttrString(obj, "triggers", options);
      PyObject_SetAttrString(obj, "targets", targets);
      PyObject_SetAttrString(obj, "platforms", platforms);
      PyObject_SetAttrString(obj, "_SERPENT_COMMAND", Py_BuildValue("s",argv[0]));
      PyObject_SetAttrString(obj, "_SERPENT_VERSION", Py_BuildValue("s","0.0.98"));
      PyObject_SetAttrString(obj, "_SERPENT_SCRIPT", Py_BuildValue("s",file.c_str()));
      PyObject_SetAttrString(obj, "_WORKING_DIR", Py_BuildValue("s",workingDirectory));
      PyObject_SetAttrString(obj, "_WORKING_ROOT", Py_BuildValue("s",workingDirectory));
      PyObject_SetAttrString(obj, "_SERPENT_LOG", _nolog == true ? Py_False : Py_True);

      
      PyObject* serpent_dictionary = PyModule_GetDict(obj);
      PyDict_SetItemString(serpent_dictionary, "__builtins__", PyEval_GetBuiltins());

      extern std::string data;  
      PyObject* pValue = PyRun_String(data.c_str(), Py_file_input, serpent_dictionary, serpent_dictionary);
      if (pValue == NULL) {
         PyErr_Print();
      }
      
      printf("Run serpent on %s\n", file.c_str());
      PyObject* tuple = PyTuple_New(1);
      PyTuple_SetItem(tuple, 0, Py_BuildValue("s", file.c_str()));
      emb_run(0, tuple);

      PyObject* pFunc = PyObject_GetAttrString(obj, (char*)"build");
      if( pFunc )
      {
        if (PyCallable_Check(pFunc)) {
          PyObject_CallObject(pFunc,0);
          PyErr_Print();    
        } else {
          PyErr_Print();
        }
      } 

      /*
      std::ifstream t(file.c_str());
      std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
      PyObject* main_module = PyImport_AddModule("__main__");
      //PyModule_AddStringConstant(main_module, "__file__", "BUILDENV");

      main_dict = PyModule_GetDict(main_module);
      PyObject* local_dict = PyDict_New();
      PyDict_SetItemString(local_dict, "serpent", obj);
      PyObject* runned = PyRun_String(str.c_str(), Py_file_input, main_dict, local_dict);
      if (runned == NULL) {
           PyErr_Print();
      } else { 

        PyObject* pFunc = PyObject_GetAttrString(obj, (char*)"build");
        if( pFunc )
        {
          if (PyCallable_Check(pFunc)) {
            PyObject_CallObject(pFunc,0);
            PyErr_Print();    
          } else {
            PyErr_Print();
          }
        }   
      }
      */

      Py_Finalize();
      //freopen("CONIN$", "r", stdin); 
      //freopen("CONOUT$", "w", stdout); 
      //freopen("CONOUT$", "w", stderr);   

      #ifdef HAVE_CURL
      curl_easy_cleanup(curl);
      #endif    
  };

	if( argv[lastKnownOption] != 0 && strcmp(argv[lastKnownOption], "help") == 0)
	{
    parse_buildenv(argc,argv,lastKnownOption);
		printf("env file action [option1] [option1]\r\n");
		printf("\n");
		printf("\tSupported Triggers:\n");
		for( std::map<std::string, std::string>::iterator itt = _options_desc.begin(); itt != _options_desc.end(); ++itt ) {
			printf("\t--%s=%s %s\n", itt->first.c_str(), _options_value[itt->first].c_str(), itt->second.c_str());
		}

		printf("\n");
		printf("\tSupported Targets:\n");
		for( std::set<std::string>::iterator itt = _targets.begin(); itt != _targets.end(); ++itt ) {
			printf("\t/t:%s\n", (*itt).c_str());
		}

		printf("\n");
		printf("\tSupported Actions:\n");
		printf("\t%s\n", "Clean");
		printf("\t%s\n", "Build");
		printf("\t%s\n", "Rebuild");
		printf("\t%s\n", "Workspace");
	}
  else if( argv[lastKnownOption] != 0 && strcmp(argv[lastKnownOption], "license") == 0)
  {
    parse_buildenv(argc,argv,lastKnownOption);
    printf("env file action [option1] [option1]\r\n");
    printf("\n");
    printf("\tLicenses:\n");
    for( std::map<std::string, std::string>::iterator itt = _license_file.begin(); itt != _license_file.end(); ++itt ) {
      std::string file;
      file.append("license/license-");
      file.append(itt->first.c_str());
      file.append(".txt");

      printf("copy from %s to %s\n", itt->second.c_str(), file.c_str());
      CopyFile(itt->second.c_str(), file.c_str(), false);      
    }
  }  
  else if( argv[lastKnownOption] != 0 && strcmp(argv[lastKnownOption], "env") == 0)
  {
    parse_buildenv(argc,argv,lastKnownOption);
    for( std::map<std::string, std::string>::iterator itt = _environments.begin(); itt != _environments.end(); ++itt ) {
      std::string environmentName;
      environmentName.append(bindir);
      environmentName.append("/activate-");
      environmentName.append(itt->first.c_str());
      environmentName.append(".bat");
      std::ofstream myfile;
      myfile.open(environmentName.c_str());
      myfile << itt->second.c_str();
      myfile.close();
    }

    std::string environmentName;
    environmentName.append(bindir);
    environmentName.append("/activate-");
    environmentName.append(argv[lastKnownOption + 1]);
    environmentName.append(".bat");


    struct stat buffer;   
    if( (stat(environmentName.c_str(), &buffer) == 0) ) 
    {
      std::string environment;
      environment.append("cmd /k \"call ");
      environment.append(environmentName.c_str());
      environment.append("\"");
      system("cls");
      printf("activating environment %s\n", environmentName.c_str());      
      system(environment.c_str());
    }
    else
    {
      printf("environment does not exists %s\n", environmentName.c_str());
    }
  }    
  else if( argv[lastKnownOption] != 0 && strcmp(argv[lastKnownOption], "--h") == 0)
  {
    printf("env file action [option1] [option1]\r\n");
    printf("\n");
    printf("\tSupported Actions:\n");
    printf("\t%s\n", "Clean");
    printf("\t%s\n", "Build");
    printf("\t%s\n", "Rebuild");
    printf("\t%s\n", "Workspace");
  }  
	else if( argv[lastKnownOption] != 0 && strcmp(argv[lastKnownOption], "configure") == 0)
	{
		std::ofstream file("BUILD_RESP");
		for( int i = 2; i < argc; ++i ) {			
			file << argv[i] << std::endl;			
		}
		file.close();
	}
  else
  {
    parse_buildenv(argc,argv,lastKnownOption);
  }

	return 0;
}
