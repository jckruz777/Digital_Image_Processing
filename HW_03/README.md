# Manejo de Pixeles: Trazo de Líneas utilizando el Algoritmo de Bresenham

Se han codificado dos implementaciones de este algoritmo mediante el uso de las bibliotecas:
* OpenCV
* LTI-Lib-2

## Descripción de la Aplicación

La aplicación permite al usuario especificar la imagen sobre la cual desea dibujar las líneas. También le permite seleccionar entre 9 colores posibles a saber:
* Negro
* Blanco
* Rojo
* Verde
* Azul
* Amarillo
* Naranja
* Cyan
* Magenta

El usuario también puede especificar las coordenadas que definen el punto inicial y final de la línea a dibujar sobre la imagen de entrada. La aplicación permite modificar dichos puntos en tiempo real, mediante operaciones algebraicas definidas por el usuario al pulsar ciertas teclas del teclado. La aplicación provee información al usuario sobre las coordenadas actualizadas de los puntos que definen inicio y final de la línea, así como de la distancia que existe entre dichos puntos. 

Información detallada sobre el algoritmo de Bresenham para Líneas se puede encontrar en Wikipedia:
https://en.wikipedia.org/wiki/Bresenham's_line_algorithm

### Prerequisitos

La máquina donde se desea ejecutar ambas implementaciones debe contar con: 
* Sistema Operativo Linux
* Biblioteca LTI-Lib-2
* Biblioteca OpenCV 3.1.0 o superior*

*La aplicación programada con OpenCV podría funcionar en versiones inferiores a 3.1.0; sin embargo, ésta no ha sido probada.

### Instrucciones de Uso

##### Compilación
En el repositorio se presentan dos carpetas denominadas *LTILib2* y *OpenCV*. Dentro de cada carpeta se encuentra un archivo *Makefile* junto a los demás archivos de código fuente. Basta con abrir una terminal desde la carpeta de la implementación que se desea compilar y ejecutar:
```
make
```

##### Ejecución
Si la compilación finalizó correctamente se generará un archivo ejecutable. En el caso de *LTILib2* dicho archivo se llamará *Tarea_03* y para *OpenCV*, el ejecutable se llamará *opencv_t03*. 

###### * Imagen a Utilizar: 
El *path* de la imagen a utilizar debe colocarse después del nombre del ejecutable de acuerdo a la versión del programa:
```
./Tarea_03 <path_imagen>
```
O en su defecto:
```
./opencv_t03 <path_imagen>
```
###### * Color a Utilizar:
El programa solicitará la selección de un color mediante la introducción de un número entre 0 y 8, de acuerdo a un menú que se despliega al usuario.
- Seleccionar el número en el rango [0,8] y pulsar *Enter*.

###### * Coordenadas de los Puntos:
- Seleccionar la coordenada X del punto inicial (x0) y pulsar *Enter*.
- Seleccionar la coordenada Y del punto inicial (y0) y pulsar *Enter*.
- Seleccionar la coordenada X del punto final (x1) y pulsar *Enter*.
- Seleccionar la coordenada Y del punto final (y1) y pulsar *Enter*.

###### * Cambio de Puntos en Tiempo Real:
La ubicación de los puntos inicial y final se pueden modificar en tiempo real mediante el uso del teclado:
- **Punto inicial:** 
    - Presionar la tecla **W** para decrementar la coordenada Y del punto inicial.
    - Presionar la tecla **Z** para incrementar la coordenada Y del punto inicial.
    - Presionar la tecla **A** para decrementar la coordenada X del punto inicial.
    - Presionar la tecla **D** para incrementar la coordenada X del punto inicial.
- **Punto final:**
    -  Presionar la tecla **direccional hacia arriba** para decrementar la coordenada Y del punto final.
    - Presionar la tecla **direccional hacia abajo** para incrementar la coordenada Y del punto final.
    - Presionar la tecla **direccional hacia la izquierda** para decrementar la coordenada X del punto final.
    - Presionar la tecla **direccional hacia la derecha** para incrementar la coordenada X del punto final.

- **IMPORTANTE:** El código que envían las teclas direccionales puede variar dependiendo del sistema en que se utilice la aplicación.
    
###### * Finalizar la Ejecución:
- Presionar la tecla **X** para finalizar el programa.

###### * Información:
El programa imprimirá en consola las coordenadas actualizadas de los puntos inicial y final de la línea, así como la distancia entre dichos puntos.

