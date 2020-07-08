// El siguiente bloque ifdef muestra la forma estándar de crear macros que hacen la exportación
// de un DLL más sencillo. Todos los archivos de este archivo DLL se compilan con DLL2_EXPORTS
// símbolo definido en la línea de comandos. Este símbolo no debe definirse en ningún proyecto
// que use este archivo DLL. De este modo, otros proyectos cuyos archivos de código fuente incluyan el archivo verán
// interpretan que las funciones DLL2_API se importan de un archivo DLL, mientras que este archivo DLL interpreta los símbolos
// definidos en esta macro como si fueran exportados.
#ifdef DLL2_EXPORTS
#define DLL2_API __declspec(dllexport)
#else
#define DLL2_API __declspec(dllimport)
#endif

// Clase exportada del DLL
class DLL2_API CDll2 {
public:
	CDll2(void);
	// TODO: agregar métodos aquí.
};

extern DLL2_API int nDll2;

DLL2_API int fnDll2(void);
