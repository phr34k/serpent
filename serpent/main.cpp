#include <Python.h>
#include <string>
#include <fstream>
#include <streambuf>
#include "FileGlobBase.h"
#include "FileGlobList.h"
#include <map>
#include <set>

#include <direct.h>
#ifdef WINDOWS
#include <windows.h>
#else //WINDOWS
#error
#endif

std::string file = "BUILDENV";
PyObject* obj = 0;
PyObject *options = 0;
PyObject *targets = 0;
PyObject *platforms = 0;
PyObject *main_dict = 0;

std::map<std::string, PyObject*>   _modules_loaded;
std::map<std::string, std::string> _options_desc;
std::map<std::string, std::string> _options_value;
std::set<std::string> _targets;


static PyObject* emb_load(PyObject *self, PyObject *args)
{
    const char *command;
    if (!PyArg_ParseTuple(args, "s", &command))
        return Py_BuildValue("");

	char abspath[4096];
	_fullpath(abspath, command, 4096);
	std::string dir(abspath, strrchr(abspath,'\\') > strrchr(abspath, '/') ? strrchr(abspath, '\\') : strrchr(abspath, '/'));
	printf("%s\n", abspath);    

	//Lazy load the module...
	if( _modules_loaded.find(abspath) == _modules_loaded.end() ) {		
		printf("Loading new module %s\r\n", abspath);
		std::ifstream t(abspath);
		std::string str((std::istreambuf_iterator<char>(t)),
						 std::istreambuf_iterator<char>());

		PyObject *pName, *pModule, *pArgs, *pValue, *pFunc;
		PyObject *pNewMod = PyModule_New("");
		PyModule_AddStringConstant(pNewMod, "__file__", command);
		PyObject *pLocal = PyModule_GetDict(pNewMod);
		PyDict_SetItemString(pLocal, "__builtins__", PyEval_GetBuiltins());
		pValue = PyRun_String(str.c_str(), Py_file_input, pLocal, pLocal);
		if (pValue == NULL) {
		   PyErr_Print();
		}
		_modules_loaded[abspath] = pNewMod;
		return pNewMod;
	} else {
		printf("Loading existing module %s\r\n", abspath);
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






static PyObject* emb_run(PyObject *self, PyObject *args)
{
    const char *command;
    if (!PyArg_ParseTuple(args, "s", &command))
        return Py_BuildValue("");

	//Lazy load the module...
	if( _modules_loaded.find(command) == _modules_loaded.end() ) {

		char abspath[4096];
		_fullpath(abspath, command, 4096);
		std::string dir(abspath, strrchr(abspath,'\\') > strrchr(abspath, '/') ? strrchr(abspath, '\\') : strrchr(abspath, '/'));

		std::ifstream t(command);
		std::string str((std::istreambuf_iterator<char>(t)),
						 std::istreambuf_iterator<char>());

		char workingDirectory[2048];
		_getcwd(workingDirectory, 2048);		
		SetCurrentDirectoryA(dir.c_str());

		PyObject* script = PyObject_GetAttrString(obj, "_SERPENT_SCRIPT");
		PyObject_SetAttrString(obj, "_SERPENT_SCRIPT", Py_BuildValue("s",command));
		PyObject* workingdir = PyObject_GetAttrString(obj, "_WORKING_DIR");
		PyObject_SetAttrString(obj, "_WORKING_DIR", Py_BuildValue("s",abspath));

		PyObject *pName, *pModule, *pArgs, *pValue, *pFunc;
		PyObject *pNewMod = PyModule_New("");
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
		return pNewMod;
	} else {
		return _modules_loaded.find(command)->second;
	}	
}

static PyMethodDef EmbMethods[] = {
	
	{"option", (PyCFunction)emb_option, METH_VARARGS | METH_KEYWORDS},
	{"target", (PyCFunction)emb_target, METH_VARARGS | METH_KEYWORDS},	
	{"load", (PyCFunction)emb_load, METH_VARARGS},
    {"info", (PyCFunction)emb_info, METH_VARARGS},
	{"glob", (PyCFunction)emb_glob, METH_VARARGS | METH_KEYWORDS},
	{"include", (PyCFunction)emb_run, METH_VARARGS},	
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

int main(int argc, char** argv)
{
	char workingDirectory[2048];
	_getcwd(workingDirectory, 2048);

    Py_Initialize();

	options = PyDict_New();
	targets = PyList_New(0);
	platforms = PyList_New(0);
	parse_argv(argv + 2, argc - 2);
	
    obj = Py_InitModule("serpent", EmbMethods);
    PyObject_SetAttrString(obj, "content", Py_BuildValue("i", false));
	PyObject_SetAttrString(obj, "action", Py_BuildValue("s", argv[1]));
	PyObject_SetAttrString(obj, "triggers", options);
	PyObject_SetAttrString(obj, "targets", targets);
	PyObject_SetAttrString(obj, "platforms", platforms);
	PyObject_SetAttrString(obj, "_SERPENT_COMMAND", Py_BuildValue("s",argv[0]));
	PyObject_SetAttrString(obj, "_SERPENT_VERSION", Py_BuildValue("s","0.0.98"));
	PyObject_SetAttrString(obj, "_SERPENT_SCRIPT", Py_BuildValue("s",file.c_str()));
	PyObject_SetAttrString(obj, "_WORKING_DIR", Py_BuildValue("s",workingDirectory));
	PyObject* serpent_dictionary = PyModule_GetDict(obj);
	PyDict_SetItemString(serpent_dictionary, "__builtins__", PyEval_GetBuiltins());



	extern std::string data;
	
	PyObject* pValue = PyRun_String(data.c_str(), Py_file_input, serpent_dictionary, serpent_dictionary);
	if (pValue == NULL) {
	   PyErr_Print();
	}
	
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
	Py_Finalize();

	if( argv[1] != 0 && strcmp(argv[1], "help") == 0)
	{
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
	else if( argv[1] != 0 && strcmp(argv[1], "configure") == 0)
	{
		std::ofstream file("BUILD_RESP");
		for( int i = 2; i < argc; ++i ) {			
			file << argv[i] << std::endl;			
		}
		file.close();
	}

	return 0;
}
