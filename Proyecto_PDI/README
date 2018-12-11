# Optimización de Operadores Morfológicos con Instrucciones SIMD


#### Descripción de la Aplicación

Se cuenta con 5 implementaciones de los algoritmos morfológicos de dilatación y erosión:
* Serial: Implementación Naive
* LTI-Lib2: Implementación utilizando las funciones provistas en la biblioteca LTI-Lib2
* OpenCV: Implementación utilizando las funciones provistas en la biblioteca OpenCV
* Paper: Implementación propuesta por Dokládal-Dokládalová
* Neon-Vectorial: Implementación que utiliza las intrínsecas de NEON para procesamiento vectorial

### Prerequisitos

La máquina donde se desea ejecutar las versiones descritas anteriormente, debe contar con: 
* Sistema Operativo Linux
* Biblioteca LTI-Lib-2
* Biblioteca OpenCV 2.4 o superior
* Procesador ARM con soporte para ARMv8

Cada versión contiene un script denominado **clearCache.sh* que podría requerir permisis de ejecución para funcionar correctamente:
```
chmod +x clearCache.sh
```

Cada versión ejecutará el filtro de mínimos primero, seguido del filtro de máximos.

### Instrucciones de Uso

##### Compilación
Dentro de cada carpeta (versión) se encuentra un archivo denominado *Makefile* junto a los demás archivos de código fuente. Basta con abrir una terminal desde la carpeta y ejecutar:
```
make
```
##### Ejecución
Si la compilación finalizó correctamente se generará un archivo ejecutable con el mismo nombre de la carpeta. 

###### * Imagen a Utilizar: 
El *path* de la imagen a utilizar debe colocarse después del nombre del ejecutable de acuerdo a la versión del programa:
```
./[Nombre_Carpeta] <path_imagen>
```
Por ejemplo:
```
./LTI-Lib2 ../images/lenna1.png
```

###### * Resultados: 
Todas las versiones generan un archivo denominado *data.dat* con los resultados de tiempos obtenidos. Además, durante su ejecución se imprimen los valores de la varianza obtenida mediante las mediciones obtenidas para determinar cada valor de tiempo.

###### * Habilitar Visualización:
Por defecto, los resultados no son visibles. Para habilitar la visualización (imágenes) del algoritmo en tiempo de ejecución, basta con ingresar al código fuente (.cpp) de la versión que se desea ejecutar y des-comentar el siguiente macro en las líneas iniciales de dicho código:
```
//#define DISPLAY 1

```

En caso de que esta línea no se encuentre comentada, se mostrará la imagen de entrada en escala de grises, posteriormente se mostrará el resultado de aplicar la erosión (filtro de mínimos) y por último el resultado de la dilatación (filtro de máximos).
