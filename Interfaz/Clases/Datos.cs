using System;
using System.Collections.Generic;
using System.Text;
using Newtonsoft.Json;

namespace Interfaz.Clases
{
    class Datos
    {
        public enum Estancias
        {
            salida,
            sala1
        };
        public enum Sentidos
        {
            parar,
            recto,
            izquierda,
            derecha
        };

        [JsonProperty("Usuario")]
        public String Usuario { set; get; }
        [JsonProperty("Fecha")]
        public DateTime Fecha { set; get; }
        [JsonProperty("Estancia")]
        public Estancias Estancia { get; set; }
        [JsonProperty("Movimiento")]
        public Sentidos Movimiento { set; get; }
     }
}
