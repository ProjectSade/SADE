using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace Supervision.Clases
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
        }
        [JsonProperty("Usuario")]
        public String Usuario { set; get; }
        [JsonProperty("Fecha")]
        public DateTime Fecha { set; get; }
        [JsonProperty("Estancia")]
        public Estancias Estancia { set; get; }
        [JsonProperty("Movimiento")]
        public Sentidos Movimiento { set; get; }
        [JsonProperty("Distancia")]
        public float Distancia { set; get; }
    }

    
}
