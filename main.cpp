/*
Jorge Vieira Luna

Métodos algorítmicos para la resolución de problemas, 3º GII

V = Vertice(s)
A = E = Arista(s)
*/

#include <iostream>
#include <list>
#include <vector>
#include <ctime>
#include <time.h>
#include <fstream>

using namespace std;

const int MAXVERTRDM = 1000; //Maximo numero de vertices en la generacion aleatoria de grafos
unsigned long x = 123463789, y = 36242469, z = 52143629; //Valores aleatorios para xorshift

class Grafo {
	struct datosVertices { //Datos de cada vertice para DFSVISIT
		char color;
		int tiempogris;
		int tiemponegro;
		int pi;
	};

public:
	Grafo(const int vertices) {//Constructor, instancia las listas segun el numero de vertices recibido y fija los valores iniciales
		for (int i = 0; i < vertices; i++) {
			list<int> l1;
			adj.push_back(l1);
			//Instanciacion de las estructuras de info sobre vertices
			datosVertices d;
			dv.push_back(d);
		}

		V = vertices;
		tiempo = 0;
		ciclos = false;
	}

	void nuevaArista(const int o, const int d) {
		adj[o].push_back(d);
	}

	int getTiempo() {
		return tiempo;
	}

	void aumTiempo() {
		tiempo++;
	}

	bool hayCiclos(){
		return ciclos;
	}

	void setCiclos(bool cic) {
		ciclos = cic;
	}

	int numVertices() {
		return V;
	}

	//Modificacion de los datos de los vertices
	void setColorV(int v, char c) {
		dv[v].color = c;
		if (c == 'N') {//Para toposort y SCCs, se insertan en orden decreciente
			toposorted.push_front(v);  //Cormen P.466 segunda ed     
		}
	}

	void setTiempoGrisV(int v) {
		dv[v].tiempogris = tiempo;
	}

	void setTiempoNegroV(int v) {
		dv[v].tiemponegro = tiempo;
	}

	void setPiV(int v, int p) {
		dv[v].pi = p;
	}

	//Consulta de los datos de los vertices
	char getColorV(int v) {
		return dv[v].color;
	}

	int getTiempoGrisV(int v) {
		return dv[v].tiempogris;
	}

	int getTiempoNegroV(int v) {
		return dv[v].tiemponegro;
	}

	int getPiV(int v) {
		return dv[v].pi;
	}

	//Iteradores para las listas de adyacencia
	list<int>::iterator begin(int v) {
		list<int>::iterator i;
		i = adj[v].begin();
		return i;
	}

	list<int>::iterator end(int v) {
		list<int>::iterator i;
		i = adj[v].end();
		return i;
	}

	//Iteradores para toposort
	list<int>::iterator begintps() {
		list<int>::iterator i;
		i = toposorted.begin();
		return i;
	}

	list<int>::iterator endtps() {
		list<int>::iterator i;
		i = toposorted.end();
		return i;
	}


private:
	vector<list<int>> adj; //Listas de adyacencia del grafo
	int V; //Numero de vertices del grafo
	list<int> toposorted; //Vertices en orden topologico (Se asocian durante el DFS, es el orden de finalizacion de vertices (se tornan a negro) en orden inverso) (Servira tambien para las SCCs)
	int tiempo; //Para el progreso del DFS
	bool ciclos; //Si el grafo tiene ciclos o no (Se averigua durante el DFS)
	vector<datosVertices> dv;
};

////////////////////////////////////////////////////

void DFSVISIT(Grafo & g, int origen) {  //Se hace recursion por cada arista, sobre cada llamada. Al ser llamada V veces y recurrida E veces, el coste total es V + E
	g.setColorV(origen, 'G'); //Se descubre el vertice, pasa a gris
	g.aumTiempo(); //Se actualiza el tiempo del grafo dentro del DFS
	g.setTiempoGrisV(origen); //Se actualiza el tiempo de descubrimiento del vertice

	list<int>::iterator i;
	for (i = g.begin(origen); i != g.end(origen); ++i) { //Se exploran las aristas que salen del vertice origen

		if (g.getColorV(*i) == 'G') { //Si el vertice explorado es gris, es que hay una arista "back edge" -> hay ciclos
			//Cormen P.462 segunda ed
			g.setCiclos(true);
		}

		if (g.getColorV(*i) == 'B') { //Si se llega a un vertice no visitado (Blanco aun), se visita, y se asocia como padre al origen
			g.setPiV(*i, origen);
			DFSVISIT(g, *i); //Se hace recursion por cada arista. Al ser esta funcion llamada V veces, el coste total es V + E
		}
	}

	//Se incrementa el tiempo para descartar el vertice
	g.aumTiempo(); 
	g.setTiempoNegroV(origen); //Se actualiza el tiempo en el que todas las aristas de origen se exploran, y se torna negro (internamente ademas se ordena en una lista de finalizacion de vertices invertida) 
	g.setColorV(origen, 'N');
}

void DFS(Grafo & g) { //DFS sobrecargado
	for (int i = 0; i < g.numVertices(); i++) { //Se inicializan los vertices a color blanco (sin descubrir), tiempo de descubrimiento 0 y pi -1 o null
		g.setColorV(i, 'B');
		g.setPiV(i, -1);
	}

	for (int i = 0; i < g.numVertices(); i++) { //Se llama a DFSVISIT para cada vertice blanco (menos los que dejen de estar blancos a mitad del algoritmo, si los hay).
		if (g.getColorV(i) == 'B')
			DFSVISIT(g, i);
	}
}

void mostrarVerticesSCC(Grafo & g, int actual, vector<bool> & visitados) {
	visitados[actual] = true;
	printf("%d ", actual);

	list<int>::iterator i;
	for (i = g.begin(actual); i != g.end(actual); ++i) { //Se exploran las aristas que salen del vertice origen

		if (g.getPiV(*i) == actual && visitados[*i] == false) { //Si el padre del hijo es el nodo actual (es decir, el hijo forma parte de la SCC) y no se ha visitado aun
			mostrarVerticesSCC(g, *i, visitados);
		}
	}
}

void mostrarVerticesSCC(Grafo & g, int raiz) {
	vector<bool> visitados(g.numVertices(), false);

	visitados[raiz] = true;
	printf("%d ", raiz);

	list<int>::iterator i;
	for (i = g.begin(raiz); i != g.end(raiz); ++i) { //Se exploran las aristas que salen del vertice origen

		if (g.getPiV(*i) == raiz && visitados[*i] == false) { //Si el padre del hijo es el nodo actual (es decir, el hijo forma parte de la SCC) y no se ha visitado aun
			mostrarVerticesSCC(g, *i, visitados);
		}
	}
}

void DFS(Grafo & original, Grafo & traspuesto) { //DFS sobrecargado para SCCs, hace DFS sobre el traspuesto en el orden de finalizacion de vertices y tras ello muestra las SCCs
	for (int i = 0; i < traspuesto.numVertices(); i++) { //Se inicializan los vertices a color blanco (sin descubrir), tiempo de descubrimiento 0 y pi -1 o null
		traspuesto.setColorV(i, 'B');
		traspuesto.setPiV(i, -1);
	}
	
	list<int>::iterator i;
	for (i = original.begintps(); i != original.endtps(); i++) { //Se llama a DFSVISIT para cada vertice blanco del traspuesto (En orden decreciente segun cuando se tornaron a negro en el original)
		if (traspuesto.getColorV(*i) == 'B')
			DFSVISIT(traspuesto, *i);
	}

	//Ya estan los arboles DFS del traspuesto hechos para obtener las SCCs, se parte de las raices y se muestran
	int scc = 1;
	for (int i = 0; i < traspuesto.numVertices(); i++) {
		if (traspuesto.getPiV(i) == -1) {//Si el vertice no tiene antecesores, es decir, es raiz de un arbol
			printf("Componente fuertemente conexa #%d: ", scc);

			mostrarVerticesSCC(traspuesto, i);

			printf("\n");
			scc++;
		}
	}

}

void TS(Grafo & g) { //Coste V
	list<int>::iterator i;
	for (i = g.begintps(); i != g.endtps(); ++i) { 
		//Se recorre la lista de vertices ordenados segun su tiempo de finalizacion de proceso en orden inverso, que es un orden topologico posible
		//Cormen P.466 segunda ed
		printf("%d ", *i);
		
	}
	cout << endl << endl;
}

Grafo trasponer(Grafo & g) { //Coste V + E
	Grafo gt(g.numVertices());
	for (int i = 0; i < g.numVertices(); i++) {
		list<int>::iterator j;
		for (j = g.begin(i); j != g.end(i); ++j) {
			gt.nuevaArista(*j, i);
		}
	}

	return gt;
}

void SCC(Grafo & g) { //Procedimiento Cormen P.469 segunda ed para SCCs
	Grafo gt = trasponer(g);//Se genera un grafo traspuesto a partir del original //Coste V + E
	DFS(g, gt); //Se ejecuta DFS sobre ese grafo, pero siguiendo un orden decreciente de vertices tornados a negro en el original //Coste V + E
	
}

unsigned long xorshf96(void) {
	//Funcion que genera numeros aleatorios muy distintos (a diferencia de rand, que funciona horriblemente mal) a partir de tres enteros x y z
	unsigned long t;
	x ^= x << 16;
	x ^= x >> 5;
	x ^= x << 1;

	t = x;
	x = y;
	y = z;
	z = t ^ x ^ y;

	return z;
}

Grafo generarGrafo(int & nAristasInsertadas, int & nVertices) {

	ofstream datos("aristasgrafo.txt");

	nVertices = xorshf96() % MAXVERTRDM + 1; //Entre 1000 y 1 vertices
	nVertices = 300; //Cambiar aqui para un numero de vertices concreto o comentar para usar la constante MAXVERTRDM
	Grafo g(nVertices);

	int nMaxAristas = nVertices * nVertices - 1;

	vector<vector<int>> verts; //Vectores de adyacencia para conocer las aristas generadas y poder consultar en tiempo constante
	for (int i = 0; i < nVertices; i++) {
		vector<int> t(nVertices, 0); //Va a funcionar como una matriz de adyacencia, pero permite funcionamiento sin conocer un numero de vertices fijo
		verts.push_back(t); //Para una matriz haria falta conocerlo al declarar el array
	}
	

	int nAristas = xorshf96() % nMaxAristas; //Entre el maximo de aristas posibles con los vertices generados y 0 
	nAristas = 300; //Manipulacion del numero de aristas no aleatoria aqui o comentar para usar la variable nMaxAristas obtenida a partir del numero de vertices
	nAristasInsertadas = 0;

	int origen, destino, foo;
	while (nAristasInsertadas < nAristas) {//Insercion aleatoria de aristas
		origen = xorshf96() % nVertices;
		destino = xorshf96() % nVertices;

		if (verts[origen][destino] == 0 && origen != destino) { //Si no es autoarista y no existe esa arista ya, se inserta
			verts[origen][destino] = 1;
			nAristasInsertadas++;
			g.nuevaArista(origen, destino);

			datos << origen << " " << destino << endl;
			
		}
	}


	datos.close();

	//Insercion manual de aristas aqui (PARA TESTEO, POR EJEMPLO CREAR UN CICLO MANUALMENTE Y ASI OBLIGAR A REALIZAR EL PROCEDIMIENTO PARA SCCs)


	return g;
}

int main()
{	

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Test con grafo de ejemplo (Descomentar las aristas que forman ciclo para probar el muestreo de SSCs en vez del orden topologico)
	//Comentar esta parte o la segunda segun se quiera
	
	
	//Creacion del grafo
	Grafo g(6);
	
	//insercion de aristas
	g.nuevaArista(0, 1);
	g.nuevaArista(0, 3);
	g.nuevaArista(1, 4);
	g.nuevaArista(2, 4);
	g.nuevaArista(2, 5);
	//g.nuevaArista(3, 1);//Forma ciclo
	g.nuevaArista(4, 3);
	//g.nuevaArista(5, 5);//Forma ciclo (autoarista)

	DFS(g); //Coste (V + E)

	if (g.hayCiclos()) {
		cout << "Se han encontrado ciclos." << endl << "Mostrando componentes fuertemente conexas..." << endl;
		SCC(g); //Se ha realizado un DFS (V + E) pero ademas se va a trasponer el grafo (Coste V + E) y hacer un DFS sobre el traspuesto. Coste total 3(V + E) + V (este ultimo V es el coste de mostrar las SCCs)
	}
	else {
		cout << "No se han encontrado ciclos." << endl << "Mostrando un orden topologico posible..." << endl;
		TS(g); //Coste V usando los datos del DFS, que cuesta (V + E). Coste total (V + E) + V (este ultimo V es el coste de mostrar el orden topologico)
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	
	

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Creacion de grafos aleatorios (de tamaño especificable en la funcion generarGrafo)
	/*
	srand(time(NULL));

	x = rand() % 10000000;
	y = rand() % 20000000;
	z = rand() % 30000000;

	//Generacion de grafo aleatorio 
	//xorshift genera siempre la misma serie para determinados x y z, pero las series generadas son muy aleatorias entre si y consigo mismas data-wise
	int nAristas;
	int nVertices;
	Grafo g1 = generarGrafo(nAristas, nVertices); //Devuelve en nAristas y nVertices como ha salido el grafo
	//El numero de aristas y vertices se ha especificado a mano dentro de la funcion generaGrafoAleatorio
	//La funcion tambien genera el archivo aristasgrafo.txt, con las aristas generadas
	cout << "Generado grafo con " << nVertices << " vertices y " << nAristas << " aristas." << endl << endl;

	//Copias para hacer la media de tiempos
	Grafo g2 = g1;
	Grafo g3 = g1;
	vector<Grafo> grafos;
	grafos.push_back(g1);
	grafos.push_back(g2);
	grafos.push_back(g3);


	//Calculo de tiempo de ejecucion
	std::clock_t start;
	double duracion = 0;

	for (int i = 0; i < 3; i++) {
		start = std::clock();

		DFS(grafos[i]); //Coste (V + E)

		if (grafos[i].hayCiclos()) {
			cout << "Se han encontrado ciclos." << endl << "Mostrando componentes fuertemente conexas..." << endl;
			SCC(grafos[i]); //Se ha realizado un DFS (V + E) pero ademas se va a trasponer el grafo (Coste V + E) y hacer un DFS sobre el traspuesto. Coste total 3(V + E) + V (este ultimo V es el coste de mostrar las SCCs)
		}
		else {
			cout << "No se han encontrado ciclos." << endl << "Mostrando un orden topologico posible..." << endl;
			TS(grafos[i]); //Coste V usando los datos del DFS, que cuesta (V + E). Coste total (V + E) + V (este ultimo V es el coste de mostrar el orden topologico)
		}

		double iter = (std::clock() - start) / (double)CLOCKS_PER_SEC;
		duracion += iter; 
		cout << "Ha tardado: " << iter << " segundos." << endl << endl;
	}

	cout << endl << "MEDIA DE TIEMPOS: " << duracion/3 << endl;

	ofstream datos("datosgrafo.txt");

	datos << "VERTICES: " << nVertices << endl << "ARISTAS: " << nAristas << endl << "TIEMPO: " << duracion/3 << endl;
	if (grafos[0].hayCiclos())
		datos << "CON CICLOS";
	else
		datos << "SIN CICLOS";
	
	datos.close();
	*/
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	system("pause");

	return 0;
}