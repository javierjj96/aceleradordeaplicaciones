# Acelerador de Aplicaciones (Speed up App Dll)
Este Dll utiliza Detours para Acelerar/Desacelerar una aplicacion.
Diseñador para aplicaciones 32 bits.

#Proceso

Utilizacion de la libreria Detours para hacer hook a Kernel32.dll al metodo GetTickCount,GetTickCount64 para manipular la velocidad
del proceso de la aplicacion que llama a esta libreria.

Inicializa una consola dentro de la aplicacion una vez inyectada para mostrar el multiplicador de velocidad.

Compilar en 32 bits e inyectar en el proceso. Puede generar errores y generar un crash en la aplicacion.



