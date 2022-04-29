PAV - P3: estimación de pitch
=============================

Esta práctica se distribuye a través del repositorio GitHub [Práctica 3](https://github.com/albino-pav/P3).
Siga las instrucciones de la [Práctica 2](https://github.com/albino-pav/P2) para realizar un `fork` de la
misma y distribuir copias locales (*clones*) del mismo a los distintos integrantes del grupo de prácticas.

Recuerde realizar el *pull request* al repositorio original una vez completada la práctica.

Ejercicios básicos
------------------

- Complete el código de los ficheros necesarios para realizar la estimación de pitch usando el programa
  `get_pitch`.

   * Complete el cálculo de la autocorrelación e inserte a continuación el código correspondiente.
```cpp
 void PitchAnalyzer::autocorrelation(const vector<float> &x, vector<float> &r) const {

    for (unsigned int l = 0; l < r.size(); ++l) {
  		/// \TODO Compute the autocorrelation r[l]
      /// \DONE Autocorrelation *computed*
      /// - r[l] = sumatorio de n = l a x.size() de x[n] * x[n-l];
      r[l] = 0;
      for(unsigned int n = l; n < x.size(); n++){
        r[l] += x[n] * x[n - l];
      }
    }

    if (r[0] == 0.0F) //to avoid log() and divide zero 
      r[0] = 1e-10; 
  }
  ```

   * Inserte una gŕafica donde, en un *subplot*, se vea con claridad la señal temporal de un segmento de
     unos 30 ms de un fonema sonoro y su periodo de pitch; y, en otro *subplot*, se vea con claridad la
	 autocorrelación de la señal y la posición del primer máximo secundario.
   
     NOTA: es más que probable que tenga que usar Python, Octave/MATLAB u otro programa semejante para
	  hacerlo. Se valorará la utilización de la biblioteca matplotlib de Python.
   >Para realizar este procedimiento, modificamos la función de autocorrelación temporalmente para obtener los datos de autocorrelación por tramas en ficheros:
   >```cpp
   >...
   >#include <fstream>
   >...
   >namespace upc {
   >  unsigned int contador = 1;
   >  ofstream archivo;
   > void PitchAnalyzer::autocorrelation(const vector<float> &x,vector<float> &r) const {
   >    archivo.open("autocor" + to_string(contador) + ".txt");
   >    contador++;
   >    for (unsigned int l = 0; l < r.size(); ++l) {
   >       r[l] = 0;
   >       for(unsigned int n = l; n < x.size(); n++){
   >           r[l] += x[n] * x[n - l];
   >       }
   >    archivo << r[l] << "\n";
   >    }
   >    if (r[0] == 0.0F){ //to avoid log() and divide zero 
   >       r[0] = 1e-10; 
   >    }
   >    contador++;
   >   archivo.close();
   >}
   >```
   >Escogemos el fichero `rl050.wav` para realizar la demostración.
   >>*Gráfica de amplitud temporal del fichero y tramo de 30ms escogido*
   >>![Vista temporal de rl050.wav](img/rl050_time.png)
   >>![Zoom sobre muestra de 30ms de rl050.wav](img/rl050_time_zoom.png)
	 
   * Determine el mejor candidato para el periodo de pitch localizando el primer máximo secundario de la
     autocorrelación. Inserte a continuación el código correspondiente.
```cpp
    vector<float>::const_iterator iR = r.begin(), iRMax = iR + npitch_min;

    /// \TODO 
	/// Find ...
    /// \DONE Búsqueda de segundo máximo de autocorrelación.

    for(iR = r.begin() + npitch_min; iR < r.begin() + npitch_max; iR++){
      if(*iR > *iRMax){
        iRMax = iR;
      }
    }

    unsigned int lag = iRMax - r.begin();

    float pot = 10 * log10(r[0]);

```

   * Implemente la regla de decisión sonoro o sordo e inserte el código correspondiente.
```cpp
bool PitchAnalyzer::unvoiced(float pot, float r1norm, float rmaxnorm) const {
    /// \TODO ...
    /// \DONE Se ha usado una combinación de la potencia, la r1norm y la rmaxnorm
   
    if(r1norm < umb_r1r0 || rmaxnorm < umb_rmaxnorm || pot < umb_pot)
      return true;
    return false;
  }
```
- Una vez completados los puntos anteriores, dispondrá de una primera versión del estimador de pitch. El 
  resto del trabajo consiste, básicamente, en obtener las mejores prestaciones posibles con él.

  * Utilice el programa `wavesurfer` para analizar las condiciones apropiadas para determinar si un
    segmento es sonoro o sordo. 
	
	  - Inserte una gráfica con la estimación de pitch incorporada a `wavesurfer` y, junto a ella, los 
	    principales candidatos para determinar la sonoridad de la voz: el nivel de potencia de la señal
		(r[0]), la autocorrelación normalizada de uno (r1norm = r[1] / r[0]) y el valor de la
		autocorrelación en su máximo secundario (rmaxnorm = r[lag] / r[0]).

		Puede considerar, también, la conveniencia de usar la tasa de cruces por cero.

	    Recuerde configurar los paneles de datos para que el desplazamiento de ventana sea el adecuado, que
		en esta práctica es de 15 ms.

      - Use el estimador de pitch implementado en el programa `wavesurfer` en una señal de prueba y compare
	    su resultado con el obtenido por la mejor versión de su propio sistema.  Inserte una gráfica
		ilustrativa del resultado de ambos estimadores.
     
		Aunque puede usar el propio Wavesurfer para obtener la representación, se valorará
	 	el uso de alternativas de mayor calidad (particularmente Python).
  
  * Optimice los parámetros de su sistema de estimación de pitch e inserte una tabla con las tasas de error
    y el *score* TOTAL proporcionados por `pitch_evaluate` en la evaluación de la base de datos 
	`pitch_db/train`..

Ejercicios de ampliación
------------------------

- Usando la librería `docopt_cpp`, modifique el fichero `get_pitch.cpp` para incorporar los parámetros del
  estimador a los argumentos de la línea de comandos.
  
  Esta técnica le resultará especialmente útil para optimizar los parámetros del estimador. Recuerde que
  una parte importante de la evaluación recaerá en el resultado obtenido en la estimación de pitch en la
  base de datos.

  * Inserte un *pantallazo* en el que se vea el mensaje de ayuda del programa y un ejemplo de utilización
    con los argumentos añadidos.

    >Mensaje de ayuda del programa: 
![Help de get_pitch](img/get_pitch_help.png)

    >Ejemplo de utilización de parámetros adicionales
    ![Ejemplo con argumentos añadidos](img/command_sample.png)
- Implemente las técnicas que considere oportunas para optimizar las prestaciones del sistema de estimación
  de pitch.

  Entre las posibles mejoras, puede escoger una o más de las siguientes:

  * Técnicas de preprocesado: filtrado paso bajo, diezmado, *center clipping*, etc.
    
    >Se ha optado por realizar un center clipping:
    >```cpp
    >   /// \TODO
    >/// Preprocess the input signal in order to ease pitch estimation. For instance,
    >/// central-clipping or low pass filtering may be used.
    >/// \DONE Central-Clipping realizado
    >  for (unsigned int i = 0; i < x.size(); i++){
    >    if (abs(x[i]) < c_thr){
    >       x[i] = 0; 
    >    }
    >  }
    >```

  * Técnicas de postprocesado: filtro de mediana, *dynamic time warping*, etc.
    >Se ha optado por realizar un filtro de mediana de 3 muestras para eliminar ruidos transitorios:
    >```cpp
    >/// \TODO
    >/// Postprocess the estimation in order to supress errors. For instance, a median filter
    >/// or time-warping may be used.
    >/// \DONE
    >/// Filtro de mediana de tres posiciones implementado
    >for (unsigned int i = 1; i < f0.size()-1; i++){
    >   vector<float> aux;
    >   aux.push_back(f0[i-1]);
    >   aux.push_back(f0[i]);
    >   aux.push_back(f0[i+1]);
    >   std::sort (aux.begin(), aux.end());
    >   f0[i] = aux[1];
    >}
    >```
  * Métodos alternativos a la autocorrelación: procesado cepstral, *average magnitude difference function*
    (AMDF), etc.
  * Optimización **demostrable** de los parámetros que gobiernan el estimador, en concreto, de los que
    gobiernan la decisión sonoro/sordo.

    >Se ha optado por una solución conservadora, teniendo en cuenta en el proceso de decisión sonoro/sorda los siguientes parámetros:
      > - **Potencia de la señal**: Alta en fonemas sonoros, baja en sordos.
      > - **Cociente de autocorrelación a una muestra de distancia (r[1]/r[0])**: Los sonidos sordos presentan una gran incorrelación entre muestras debido a la no periodicidad general, por lo que, de nuevo, este cociente será mayor en fonemas sonoros que en los sordos.
      > - **Cociente de autocorrelación a muestra pitch (k) y máximo (origen) (r[k]/r[0])**: En caso de que el algoritmo de búsqueda de pitch halle un segundo máximo en un fonema sordo, la magnitud de la diferencia será muchísimo menor que en el de uno sonoro. 
      >
      >
      >Se ha utilizado el siguiente sistema de decisión, ya citado en el apartado anterior (puesto que no conservamos la primera versión del decisor):
      >```cpp
      >bool PitchAnalyzer::unvoiced(float pot, float >r1norm, float rmaxnorm) const {
      >/// \TODO ...
      >/// \DONE Se ha usado una combinación de la potencia, la r1norm y la rmaxnorm
      >if(r1norm < umb_r1r0 || rmaxnorm < umb_rmaxnorm || pot < umb_pot) return true;
      >return false;
      >}
      >```




  * Cualquier otra técnica que se le pueda ocurrir o encuentre en la literatura.

  Encontrará más información acerca de estas técnicas en las [Transparencias del Curso](https://atenea.upc.edu/pluginfile.php/2908770/mod_resource/content/3/2b_PS%20Techniques.pdf)
  y en [Spoken Language Processing](https://discovery.upc.edu/iii/encore/record/C__Rb1233593?lang=cat).
  También encontrará más información en los anexos del enunciado de esta práctica.

  Incluya, a continuación, una explicación de las técnicas incorporadas al estimador. Se valorará la
  inclusión de gráficas, tablas, código o cualquier otra cosa que ayude a comprender el trabajo realizado.

  También se valorará la realización de un estudio de los parámetros involucrados. Por ejemplo, si se opta
  por implementar el filtro de mediana, se valorará el análisis de los resultados obtenidos en función de
  la longitud del filtro.
   

Evaluación *ciega* del estimador
-------------------------------

Antes de realizar el *pull request* debe asegurarse de que su repositorio contiene los ficheros necesarios
para compilar los programas correctamente ejecutando `make release`.

Con los ejecutables construidos de esta manera, los profesores de la asignatura procederán a evaluar el
estimador con la parte de test de la base de datos (desconocida para los alumnos). Una parte importante de
la nota de la práctica recaerá en el resultado de esta evaluación.
