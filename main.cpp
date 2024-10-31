// Arturo Rosas y Jan Francisco

#include <iostream> // librerias usadas
#include <ctime>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "SortedLinkedList.h" // Aqui se incluye el .h de la lista

using std::cout; // Declarando los std
using std::endl;
using std::cin;
using std::tm;
using std::ifstream;
using  std::ofstream;
using std::string;
using std::get_time;
using std::getline;
using std::remove;
using std::mktime;
using std::ostream;
using std::istringstream;



// Estructura que representa la IP
struct ip {
    int o1;  // Los valores que componen la IP
    int o2;
    int o3;
    int o4;
    
    bool operator==(const ip &other) const { // Sobrecarga de == para hacer las comparaciones
        return o1 == other.o1 && o2 == other.o2 && o3 == other.o3 && o4 == other.o4;
    }

    bool operator<(const ip &other) const { // Sobrecarga del operador < para hacer la comparacion
        if (o1 != other.o1) return o1 < other.o1;
        if (o2 != other.o2) return o2 < other.o2;
        if (o3 != other.o3) return o3 < other.o3;
        return o4 < other.o4;
    }

    static ip from_string(const string& ip_str) { // conviete el string en una structuRa IP
        ip i;
        sscanf(ip_str.c_str(), "%d.%d.%d.%d", &i.o1, &i.o2, &i.o3, &i.o4);
        return i;
    }

    friend ostream& operator<<(ostream &os, const ip &i) { // Sobrecarga para la impresion de los datos 
        if(i.o1 == 0) {  // si hay un numero invalido pone un "-" para conservar la integridad de los datos 
            os << "-";
        } else {
            os << i.o1 << "." << i.o2 << "." << i.o3 << "." << i.o4;
        }
        return os;
    }
};

// Estructura para poder crear un evento
struct event {
    tm ts = {}; // Componentes de un evento.
    ip ip_o;
    string port_o;
    string domain_o;
    ip ip_d;
    string port_d;
    string domain_d;

    bool operator<(const event &other) const {
    // se compara las IP por fecha, se usa el operador sobrecargado, pero negado
    if (!(ip_d == other.ip_d)) { 
        return ip_d < other.ip_d;
    }

    return mktime(const_cast<tm*>(&ts)) < mktime(const_cast<tm*>(&other.ts)); // Aqui funciona de que si la IP es igual , entonces las compara por fecha
    }

    friend ostream& operator<<(ostream &os, const event &e) { // sobrecarga del operador << para manejar la IP
        char date_output[20];
        strftime(date_output, 20, "%d-%m-%Y %H:%M:%S", &e.ts);
        os << date_output << "," << e.ip_o << "," << e.port_o << ","
           << e.domain_o << "," << e.ip_d << "," << e.port_d << ","
           << e.domain_d;
        return os;
    }
};

tm string_a_tm(const string& fecha_hora_str) { // convierte el string al formato de dia,mes, año y le agrega el campo completo de la hora.
    tm tm = {};
    istringstream ss(fecha_hora_str);
    ss >> get_time(&tm, "%d-%m-%Y %H:%M:%S");
    return tm;
}

int main() {
    SortedLinkedList<event> eventList; // se crea la lista con nodos y con criterio de ordenacion

    ifstream infile("new.csv"); // abre el archivo con los eventos ordenados de la clase pasada 
    if (!infile.is_open()) {
        cout << "Hay un error con la ruta del archivo" << endl;  // un mensaje de error simple para indicar el problema
        return 1;
    }

    string linea;
    getline(infile, linea);  // aqui se salta la primera linea del encabezado 

    
    while (getline(infile, linea)) { // lee cada linea 
        istringstream iss(linea);
        event e;

        // lee la fecha como el dia y la hora
        string fecha_hora_str;
        getline(iss, fecha_hora_str, ',');
        e.ts = string_a_tm(fecha_hora_str);

        string ip_origen_str, ip_destino_str; // lee y asigna los campos separados por comas
        getline(iss, ip_origen_str, ',');
        e.ip_o = ip::from_string(ip_origen_str);
        getline(iss, e.port_o, ',');
        getline(iss, e.domain_o, ',');
        getline(iss, ip_destino_str, ',');
        e.ip_d = ip::from_string(ip_destino_str);
        getline(iss, e.port_d, ',');
        getline(iss, e.domain_d, ',');

        eventList.add(e); // usa el metodo add implementado para agregar el evento el evento.
    }

    infile.close();

    // aqui se solicita al usuario la IP de destino que quieran utilizar 
    ip ipObjetivo;
    cout << "Ingresa la IP de destino que buscas 1.1.1.1 : "<< endl;
    char punto;
    cin >> ipObjetivo.o1 >> punto >> ipObjetivo.o2 >> punto >> ipObjetivo.o3 >> punto >> ipObjetivo.o4;
    // aqui se gurdan los numeros enteros separados por numeros 

    ofstream outfile("resultado.csv"); // se crea el archivo de salida 
    outfile << "Fecha Hora,IP Origen,Puerto Origen,Dominio Origen,IP Destino,Puerto Destino,Dominio Destino\n";
    // en el archivo escribe en el encabezado para organizar mejor los datos

    // encabezado que se muestra antes de mostrar los eventos
    cout << "Fecha -- Hora -- IP Origen -- Puerto Origen -- Dominio Origen -- IP Destino -- Puerto Destino -- Dominio Destino\n";

    bool eventos_encontrados = false; // inicializacion de la variable para el while de busqeuda 

    auto it = eventList.find([&ipObjetivo](const event &e) { return e.ip_d == ipObjetivo; });
    // en la lambda se usa unna referencia de la variable y compara la ip con la que se busca y dependiendo de esto da true

    while (it != eventList.end() && (*it).ip_d == ipObjetivo) { // mientras el iterador esta en el rango de la lista y es igual a la IP que buscamos
        eventos_encontrados = true;
        cout << *it << endl;   // despliega el evento en consola
        outfile << *it << endl;      // despues lo guarda en el archivo
        ++it;   // aumenta el iterador 
    }

    if (!eventos_encontrados) {  // si no hay ningun evento asociado con la IP del usuario
        cout << "Anota bien la IP" << endl;
        remove("resultado.csv"); // ya no gener a el archivo de salida ya que no hay IP en el excel
    }

    outfile.close(); // se cierra el archivo mas que nada como buena practica

    return 0;
}
