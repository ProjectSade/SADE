using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;


namespace Interfaz.Clases
{
    class Connection
    {
        private const string url = "http://192.168.1.9/htdocs/prueba_json.php"; //para enviar la sala a la base de datos
        private const string url_get = "http://192.168.1.9/htdocs/prueba_json_get.php"; //Para recibir de la base de datos 
        private char[] charToTrim = { '[',']' }; //para eliminar [] del array json
        private HttpClient GetConn()
        {
            HttpClient conn = new HttpClient(); //configura la conexion
            conn.DefaultRequestHeaders.Add("Accept","application/json");
            conn.DefaultRequestHeaders.Add("Connection","Close");
            return conn;
        }
        public async Task<bool> SetSala(object objIns) //metodo que envia la sala seleccionada
        {
            bool exito = false;
            try
            {
                HttpClient cliente = GetConn();
                var jsonObj = JsonConvert.SerializeObject(objIns);
                var content = new StringContent(jsonObj);
                var response = await cliente.PostAsync(url, content);
                if (response.IsSuccessStatusCode)
                {
                    exito = true;
                }
                
            }catch (Exception e1)
            {
                Console.WriteLine("Excepción en la consulta: " + e1.Message);
            }
            return exito;
        }
        public async Task<Datos> GetDatos() //Metodo para recibir los datos
        {
            Datos datos = null;
            try
            {
                HttpClient cliente = GetConn();
                var response = await cliente.GetAsync(url_get);
                if (response.IsSuccessStatusCode)
                {
                    string resul = await response.Content.ReadAsStringAsync();
                    string json = resul.Trim(charToTrim); //elimina los corchetes del array recibido
                    datos = JsonConvert.DeserializeObject<Datos>(json);
                }
            }catch (Exception e2)
            {            
                Console.WriteLine("Excepción en la consulta: " + e2.Message);
            }
            Console.WriteLine("Dato relativo al movivimiento: " + datos.Movimiento.ToString());
            return datos;
        }
        
    }
}
