using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net.Http;
using Newtonsoft.Json;

namespace Supervision.Clases
{
    class Connection
    {
        private const String url = "http://192.168.1.9/htdocs/prueba_json_get.php";
        private HttpClient GetConn()
        {
            HttpClient conn = new HttpClient();
            conn.DefaultRequestHeaders.Add("Accept","application/json");
            conn.DefaultRequestHeaders.Add("Connection","Close");
            return conn;
        }

        public async Task<IEnumerable<Datos>> GetData()
        {
            try
            {

                HttpClient cliente = GetConn();
                var response = await cliente.GetAsync(url);
                if (response.IsSuccessStatusCode)
                {
                    string resul = await response.Content.ReadAsStringAsync();
                    return JsonConvert.DeserializeObject<IEnumerable<Datos>>(resul);
                }
            }
            catch (Exception e)
            {
                Console.WriteLine("Se ha producido un error: " + e.ToString());
            }
            //En caso de no recibir datos devuelve lista vacia
            return Enumerable.Empty<Datos>();
        }
    }
}
