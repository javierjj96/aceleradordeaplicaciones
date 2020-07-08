// Dll2.cpp : Define las funciones exportadas del archivo DLL.
//

#include "pch.h"
#include "framework.h"
#include "Dll2.h"


// Ejemplo de variable exportada
DLL2_API int nDll2=0;

// Ejemplo de función exportada.
DLL2_API int fnDll2(void)
{
    return 0;
}

// Constructor de clase exportada.
CDll2::CDll2()
{
    return;
}
